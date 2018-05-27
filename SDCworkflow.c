SD card testing
set 1 (valvano)
~unformatted test
~file system test
~read test
~write test

set 2
~read name, read data after and format by data\ndata\ndata and display onto ST7735
~grab_image
  problem: get image data from file ImageAssets.txt and put into a local array
  that can be called by Draw_Sprite, Undraw_Sprite, and other graphical methods.

  path to solution:
1  format data in SDcard file in the form:
    id arrSize width height
      0xFFFF, 0xFFFF, hexColor, ...,;
    id2 arrSize width height
      hexColor, hexColor, hexColor, ...,

2    the SDCfile.c function Grab_Image is of prototype:
      void Grab_Image(char *id);
      it takes in a string array of the id to find and returns void.
      it also modifies a global array image in SDCfile.c with the .h prototype
        extern unsigned short image[],
      as well as the width and height of the new image
        extern uint16_t width, height
      the functions in Graphics.c call from these prototypes to operate.

3   the function is must work such that
    void Grab_Image(char *id){
      unsigned char c = 0, val;       //char read, value for Draw_Sprite/Undraw_Sprite;
      unsigned char cVal[6];          //array for characters to be turned into val
      char name[] = *id;
      uint32_t idx = 0, arrSize;          //idx for spot in array to put current val
                                      //arrSize for max idx
      uint8_t maxSize, nFound = 0;        //maxSize for adder, nFound for nameFound

      Open_File();


      //find name - ***expand upon
      while(c == !eof && nFound == 0){
        c = Read_File();
      }


      //if name is found, find array size
      if(nFound == 1){
        c = Read_File(); //consume space
        idx = 0;
        do{
          c = Read_File() - '0';
          if(47 < c < 58){
            cVal[idx] = c;
            idx++;
          }
        }while(47 < c < 58);

        //get largest multiplier
        maxSize = 10 * (idx - 1);
        //get arrSize
        for(uint8_t i = 0; maxSize > 0; i++){
          arrSize += cVal[i] * maxSize;
          maxSize /= 10;
        }
        //now find width since space is consumed
        idx = 0;
        do{
          c = Read_File() - '0';
          if(47 < c < 58){
            cVal[idx] = c;
            idx++;
          }
        }while(47 < c < 58);

        //get largest multiplier
        maxSize = 10 * (idx - 1);
        //get arrSize
        for(uint8_t i = 0; maxSize > 0; i++){
          width += cVal[i] * maxSize;
          maxSize /= 10;
        }
        //now find height since space is consumed
        idx = 0;
        do{
          c = Read_File() - '0';
          if(47 < c < 58){
            cVal[idx] = c;
            idx++;
          }
        }while(47 < c < 58);

        //get largest multiplier
        maxSize = 10 * (idx - 1);
        //get height
        for(uint8_t i = 0; maxSize > 0; i++){
          height += cVal[i] * maxSize;
          maxSize /= 10;
        }


        //now grab chars between commas until next id char pops up
        //if c is a comma or not at next identifier ;
        idx = 0;
        do{
          c = Read_File();
          if(c == 42 || c == 32);
            //ignore commas and space
          else{
            //assumption that character read must be 0 of 0xNNNN to get here
            //proceed to read next 5 chars of format: xNNNN
            //consume useless hex signifier
            c = Read_File();
            for(int i = 0; i < 4; i++){
              c = Read_File();
              //config for 0-9 and A-F
              if(c <= 58)
                c -= '0';
              else
                c -= 'A';

              cVal[i] = c;
            }
            //adder into dec val
            uint32_t maxmul = 16 * 16 * 16;
            for(int j = 0; j < 4; j++){
              val += cVal[j] * maxmul;
              maxmul /= 16;
            }
            //store val into array
            image[idx] = val;
            idx++;
        }while(c != 59);
      }
    }



~grab_sound
  problem: get sound data from file SoundAssets.txt and put into a local array
  that can be called by new funct setSFX/setBGM that is of prototype:
    void setSFX(char *id);

  and of content
    void setSFX(char *id){
      uint32_t arrSize;

      //grab_image based on id
        //modify global buffer arrays based on image array

      //turn clock interrupt back on
      TIMER1_TAILR_R = period;
      TIMER1_TAPR_R = 0;
      TIMER1_CTL_R = 0x00000001;

      //change Timer1 arrIdx size
      size_SFX = size;

      //Play_SFX calls from sound[] now
      //sfx = buttonPress;
    }
    which allows the interrupt to call Play_SFX with new the new array size and array.

    path to solution:
1    format data in SDcard file in the form:
      id1 arrSize
        11,13,02,val,...
      id2 arrSize
        val,val,val,val,...

2    the SDCfile.c function Grab_Sound is of prototype:
      uint32_t Grab_Sound(char *id);
      it takes in a string array of the id to find and returns an array size.
      it also modifies a global array SFX in SDCfile.c with the .h prototype
        extern char sound[].
      the functions in Sound.c call from this prototype to operate.

3   the function is must work such that
    uint32_t Grab_Sound(char *id){
      unsigned char c = 0, val;       //char read, value for Draw_Sprite/Undraw_Sprite;
      unsigned char cVal[6];          //array for characters to be turned into val
      char name[] = *id;
      uint32_t idx = 0, arrSize;          //idx for spot in array to put current val
                                      //arrSize for max idx
      uint8_t maxSize, nFound = 0;    //maxSize for adder, nFound for nameFound

      Open_File();
      //find name
      while(c == !eof && nFound == 0){
        c = Read_File();
      }
      //if name is found, find array size
      if(nFound == 1){
        c = Read_File(); //consume space
        idx = 0;
        do{
          c = Read_File() - '0';
          if(47 < c < 58){
            cVal[idx] = c;
            idx++;
          }
        }while(47 < c < 58);

        //get largest multiplier
        maxSize = 10 * (idx - 1);
        //get arrSize
        for(uint8_t i = 0; maxSize > 0; i++){
          arrSize += cVal[i] * maxSize;
          maxSize /= 10;
        }

        idx = 0;
        //if name is found, grab chars between commas until
        //if c is a comma or not at next id
        //assume space is already consumed
        do{
          c = Read_File() - '0';
          if(c == 42);
            //ignore commas
          else
            //if one char (,1,), put into cVal[1] and clear cVal[0]
              val = cVal[0]*10 + cVal[1];
              sound[idx] = val; //store into global array and increment idx
              idx++;
            //if two char (,10,), put 1st char into cVal[0] and 2nd in cVal[1]
              val = cVal[0]*10 + cVal[1];
              sound[idx] = val;
              idx++;
        }while(47 < c < 58 || c == 42);

        //when last value is found return array size
        return arrSize;
      }
      //if not in file, return size 0
      return 0;
    }


//flowchart of current interation
first run through:
1)  { is encountered, internal control flag is set. initfPointer is set to the first element of the .txt array
2)  3 is encountered, since { sets delim flag, 3 is read as the first of possibly 2 number ASCII characters.
    flag is reset.
3)a. 2 is encountered, and is read as the second of 2 number ASCII characters. flag is reset.
  b. , is encountered. since the flag has been reset, convert what has been found into a number (excluding
    values that are 0xFF), insert into the global array, and raise the flag again. reset value holders to default 0xFF.
4)  }/EOF is encountered, and the current pointer is reset to the initfPointer. 2 and 3 repeat.
5) the array has been filled. the internal idx hits 400 (or 399), the current fPointer + 1 is saved, and the
   function exits, returning 1 for success.

second run through:
0) since the song text array has been assumed to be static (no current implementation of switching songs),
   the internal control flag has already been set to 1, and the initfPointer has been initialized.
1) on the next call, the control flag is checked (1), and fPointer is set to the current fPointer.
2) continue reading as expected.
