# Lab-10-C-SpaceInvader
### Final project for UT EE319K (Embedded Systems) where teams of two design their own games and participate in a competition.
### This lab project is a simulation of Pokemon, with the following features:
  1. Solo mode (and proposed Multiplayer Mode)
    - *randomly generated teams* of three Pokemon (an Actor with four attacks and health points and stats that 
      enhance its performance) that participate in a turn based battle in which the goal is to defeat the 
      opponent's three Pokemon.
    - **Solo mode** is a battle between the Player (the user), and a rudimentary NPC that decides its actions based on which attack
      deals the most damage to the current Pokemon on the field.
    - The proposed **Multiplayer mode** allows two Player(s) to compete against each other, with a UART communication protocol
      specified in *MultiplayerCommunicationProtocol.txt*.
  2. *6 bit Digital to Analog Converter* that configured for this game outputs sound effects and background music based on events
    (winning, losing, while the battle is running, main screen). *(See Sound.c, DAC.c, and ADC.c and their related files in order 
    to see the working code)*
  3. *A virtual buffer configured for the ST7735 TFT display*, allowing the game engine to display graphics at 26 frames per second
    by drawing objects in the buffer and then refreshing to the display. This allows for complex manipulation such as object collision,
    multiple object drawing, etc. *(See Graphics.c and its related files in order to see the working code)*
  4. *Hardware*: This project uses the TI TIVA TM4C1231C3PM microcontroller and associated hardware to operate. Schematics of this hardware 
    can be found at:  . This project also contains a 3d printed chassis courtesy of Jason Nyugen at UT to house components and the overall 
    project. Solidworks files of this chassis can be found at: .
    
    
### Proposed features:
  5. *UART multiplayer* communication based on the protocol specified in MultiplayerCommunicationProtocol.txt
  6. *Use of an external SD card* inserted into the ST7735 TFT formatted by the FAT32 file system and accessed using SDC.c and its related 
    files to pull images and sound. This files are currently empty. Notes on implementation of this hardware can be found at:
    
