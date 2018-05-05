# MusicBox
### Personal project where the TM4C1231C3PM microcontroller is turned into a music player controlled by a simple user interface that allows the user to scroll through a list of preprogrammed songs and play them. This is achieved using the hardware setup specified here: (https://github.com/dimembermatt/TivaBoy/blob/master/circuit%20schematic%20v1.pdf) and using the 3D printed chassis courtesy of Jason Nyugen at UT to house components and the overall project. Solidworks files of this chassis can be found at: .

###### - Operation:
    This project operates using the 6 bit Digital to Analog Converter to output sound as a voltage wave (sine wave, square wave, etc) 
    repeating at a frequency labled by a #define lookup chart for each pitch. Notes are organized into an array in which the SysTick
    interrupt timer changes the note pitch period (which is used as the reload value for the seondary onboard timer - in this case
    Timer0A - to output the sine wave).
    
###### - Transcription of the songs:
    Transcription was done by hand to convert each note to a reference in the array. The octaves defined have a range of four scales
    and are denoted by 1/2/3/4 (e.g. F3 - F third octave, C1 - C first octave, BF3 - B flat third octave) to show octave. Their period
    values are determined by the frequency of the note and the bus clock of the microcontroller.

###### - UI and using the program:
    The main function call starts up a main screen that waits for the user to press the 'A' button (check the hardware pinout sheet of 
    the hardware setup noted above) and then enters a selection screen with songs pre programmed into the controller under a struct array
    of Songs, which contain a name and a reference to an array of notes. Upon scrolling and selecting a song, the screen goes to the 
    play music screen in which pressing A plays the song and pressing A again pauses the song. Pressing B exits the menu and returns to 
    the main menu. Pressing B in the selection menu ends the program (the onboard red LED turns off).
