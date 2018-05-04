; Print.s
; Student names: Matthew Yu and Tatian Flores
; Last modification date: 3/24/18
; Runs on LM4F120 or TM4C123
; EE319K lab 7 device driver for any LCD
;
; As part of Lab 7, students need to implement these LCD_OutDec and LCD_OutFix
; This driver assumes two low-level LCD functions
; ST7735_OutChar   outputs a single 8-bit ASCII character
; ST7735_OutString outputs a null-terminated string 

    IMPORT   ST7735_OutChar
    IMPORT   ST7735_OutString
    EXPORT   LCD_OutDec
    EXPORT   LCD_OutFix

overflow	EQU		0x270F
    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB

  

;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
decPlace	EQU 0	;uint32_t
input 		EQU 4	;uint32_t

LCD_OutDec
	SUB 	SP, #8	;alloc
	;store input
	STR 	R0, [SP, #input]
	
	;get # of decimal places (max divisor)
	MOV 	R1, #10	;base divisor
	MOV 	R2, #0	;decimal places
	CMP		R0, #0
	BNE		checkPlace
	ADD		R2, #1
	B		endPlace
checkPlace
	CMP 	R0, #0
	BEQ 	endPlace
	UDIV 	R0, R1	;9/10 = 0, 19/10 = 1.9 = 1
	ADD 	R2, #1
	B 		checkPlace
endPlace

	;store divisor
	STRB	R2, [SP, #decPlace]
	
	;loop run - get largest decimal place value, output, then continue until no more places

loopOutput	
	;restore input
	LDR 	R0, [SP, #input]
	LDRB	R2, [SP, #decPlace]
	MOV		R3, #1
	MOV		R1, #10
	CMP		R2, #0
	BEQ		exit		;exit if (div) place divisor is 0
	
	;get place divisor
placeDiv
	CMP		R2, #1
	BEQ		display
	MUL		R3, R1
	SUB		R2, #1
	B		placeDiv	;10^(div-1) = (div) place divisor 
	;get decimal place
	
display
	;find modulo of 10^n
	UDIV	R0, R3
	;store new inp
	LDR		R1, [SP, #input]
	MLS		R1, R0, R3, R1
	STR		R1, [SP, #input]
	
	;display
	PUSH	{R2, LR}
	;output - 0ffset + output
	ADD		R0, #48
	BL		ST7735_OutChar
	POP		{R2, LR}
	
	;reduce divisor for next loop, if any
	LDRB	R2, [SP, #decPlace]
	SUB		R2, #1
	STRB	R2, [SP, #decPlace]
	B 		loopOutput
	
exit
	ADD 	SP, #8	;dealloc
    BX  	LR
;* * * * * * * * End of LCD_OutDec * * * * * * * *

; -----------------------LCD _OutFix----------------------
; Output characters to LCD display in fixed-point format
; unsigned decimal, resolution 0.001, range 0.000 to 9.999
; Inputs:  R0 is an unsigned 32-bit number
; Outputs: none
; E.g., R0=0,    then output "0.000 "
;       R0=3,    then output "0.003 "
;       R0=89,   then output "0.089 "
;       R0=123,  then output "0.123 "
;       R0=9999, then output "9.999 "
;       R0>9999, then output "*.*** "
; Invariables: This function must not permanently modify registers R4 to R11
decPlace2	EQU 0	;uint32_t
input2 		EQU 4	;uint32_t

LCD_OutFix
	LDR		R1, =overflow
	CMP		R1, R0
	BLO		invalid
	PUSH	{R4, LR}
	SUB 	SP, #8	;alloc
	;store input
	STR 	R0, [SP, #input2]
	
	;get # of decimal places (max divisor)
	MOV 	R1, #10	;base divisor
	MOV 	R2, #0	;decimal places
	CMP		R0, #0
	BNE		checkPlace2
	ADD		R2, #1
	B		endPlace2
checkPlace2
	CMP 	R0, #0
	BEQ 	endPlace2
	UDIV 	R0, R1	;9/10 = 0, 19/10 = 1.9 = 1
	ADD 	R2, #1
	B 		checkPlace2
	
endPlace2
	;store divisor
	STRB	R2, [SP, #decPlace2]
	;switch: decPlace case
	CMP		R2, #1
	BEQ		case1
	CMP		R2, #2
	BEQ		case2
	CMP		R2, #3
	BEQ		case3
	B		case4

case1
	MOV		R0, #48
	BL		ST7735_OutChar
	MOV		R0, #46
	BL		ST7735_OutChar
	MOV		R0, #48
	BL		ST7735_OutChar
	MOV		R0, #48
	BL		ST7735_OutChar
	B		loopOutput2
	
case2
	MOV		R0, #48
	BL		ST7735_OutChar
	MOV		R0, #46
	BL		ST7735_OutChar
	MOV		R0, #48
	BL		ST7735_OutChar
	B		loopOutput2	
	
case3
	MOV		R0, #48
	BL		ST7735_OutChar
	B		loopOutput2	

case4
loopOutput2	
	;restore input
	LDR 	R0, [SP, #input2]
	LDRB	R2, [SP, #decPlace2]
	MOV		R3, #1
	MOV		R1, #10
	CMP		R2, #0
	BEQ		exit2		;exit if (div) place divisor is 0
	;get place divisor
placeDiv2
	CMP		R2, #1
	BEQ		display2
	MUL		R3, R1
	SUB		R2, #1
	B		placeDiv2	;10^(div-1) = (div) place divisor 
	;get decimal place
	
display2
	;find modulo of 10^n
	UDIV	R0, R3
	;store new inp
	LDR		R1, [SP, #input2]
	MLS		R1, R0, R3, R1
	STR		R1, [SP, #input2]
	
	;display
	LDRB	R2, [SP, #decPlace2]
	PUSH	{R2, LR}
	;display asterisk at pos[1] if case 4 hits R2 == 3
	CMP		R2, #3
	BNE		continue
	
	PUSH	{R0, LR}
	MOV		R0, #46
	BL		ST7735_OutChar
	POP		{R0, LR}
continue
	ADD		R0, #48
	BL		ST7735_OutChar
	POP		{R2, LR}
	
	;reduce divisor for next loop, if any
	LDRB	R2, [SP, #decPlace2]
	SUB		R2, #1
	STRB	R2, [SP, #decPlace2]
	B 		loopOutput2
	
	
exit2
	ADD 	SP, #8	;alloc
	POP		{R4, LR}
    BX   	LR

invalid
	PUSH	{R0, LR}
	MOV		R0, #42
	BL		ST7735_OutChar
	MOV		R0, #46
	BL		ST7735_OutChar
	MOV		R0, #42
	BL		ST7735_OutChar
	MOV		R0, #42
	BL		ST7735_OutChar
	MOV		R0, #42
	BL		ST7735_OutChar
	POP		{R0, LR}
	BX		LR

 
    ALIGN
;* * * * * * * * End of LCD_OutFix * * * * * * * *

     ALIGN                           ; make sure the end of this section is aligned
     END                             ; end of file
