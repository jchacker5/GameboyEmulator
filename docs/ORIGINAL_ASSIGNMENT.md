COMP 406 - Computer Architecture
Spring 2024
 
Project: Emulate a Gameboy
 
 
Objective
 
Write a Nintendo Gameboy emulator.  Your finished project should play Supermarioland, as well as the test ROM images you are given.
 
Partners
 
You may work on this project individually or in groups of two.
 
Resources
 
You are given the following:
 
            Z80.cpp / Z80.h           A Z80 CPU emulator
                                                Test ROM image
 
You will also need to install the QtCreator C++ compiler at  
 
https://www.qt.io/download
 
 
PART 1.  Try out the Z80
 
Here is a simple test program that sums 6+5+4+3+2+1 = 21.
 
void dosum()
{
                   int b=6;
        int a=0;
 
        while(b>0)
        {
                a=a+b;
                b=b-1;
        }
        halt;
}
 
 
In assembly, this is the equivalent program.
 
0      ld      b,6        06 06
2      ld      a,0        3E 00
4      add     b        80
5      dec     b        05
6      jg      L2         C2 04 00
9      halt             76
 
 
You can store this in a single global array as follows:
 
char rom[]={0x06,0x06,0x3e,0x00,0x80,0x05,0xc2,0x04,0x00,0x76};
 
 
Write  gameboy.cpp as follows:
 
1.  Put in the rom[] array and the following two functions:
 
unsigned char memoryRead(int address) {  }
 
void memoryWrite(int address, unsigned char b) { }
 
 
2.  Inside memoryRead, use address to read from the rom array and return the appropriate byte.
 
3.  In main(), construct a Z80 as follows:
 
z80 = new Z80 ( memoryRead, memoryWrite);
 
4.  Call reset() on the z80 object, then set its PC variable to 0 (the beginning of the rom code).
 
5.  Call doInstruction() on the z80 object, and print out its PC, instruction, A, and B variables.  You should see it complete the first ld b,6 instruction, PC should be 2, and B should be 6.
 
6.  Put doInstruction() in a while loop, and stop the while loop when z80->halted is true.
 
Compile and run the program.  On a Linux / Mac system, type:
 
g++ -o gameboy gameboy.cpp Z80.cpp
 
On Windows, open Qt Creator, make a non-Qt C++ project, and copy over the appropriate files.
 
When you run it, you should see it run a couple dozen instructions and stop.  At the end, A should hold 21.
 
Now we'll run the same program, but we will read it from a file instead of hardcode it in an array.
 
7.  Make a global rom array  char* rom;  Don't bother initializing it to 0.
 
8.  Before constructing the z80 object, read from the file testrom.gb and copy it character by character into your rom array:
 
        ifstream romfile("testrom.gb", ios::in|ios::binary|ios::ate);
        streampos size=romfile.tellg();
 
        rom=new char[size];
 
        romSize=size;
 
        romfile.seekg(0,ios::beg);
 
        romfile.read(rom,size);
 
        romfile.close();
 
 
Don't initialize PC to 0 any longer.  
 
Verify that it still prints out A=21.
 
 
PART 2:  Displaying tile graphics
 
You are provided a file screendump that holds a video memory snapshot as a series of integers.  In this part, you will make a video memory, load this snapshot into it, and render that image in a window.
 
You can now include files gameboy.pro, screen.cpp, and screen.h in your project.  Call qmake and make to compile everything together.
 
1.  Make sure your main()  is now written as
 
extern QApplication* app;
 
int main(int argc, char** argv)
{
        setup(argc,argv);
 
            //part 1 code here
 
            //part 2 code here
 
        app->exec();
}
 
 
An empty window should show up, and your program from Part 1 should run as before.
 
 
2. Make some global variables to hold the video memory:
 
unsigned char graphicsRAM[8192];
int palette[4];
int tileset, tilemap, scrollx, scrolly;
 
In main(), read the first 8192 integers from screendump into graphicsRAM.
 
                  int n;
                  ifstream vidfile("screendump.txt",ios::in);
                  for(int i=0; i<8192; i++){
                                    int n;
 
                                    vidfile>>n;
 
                                    graphicsRAM[i]=(unsigned char)n;
                  }
 
Then read the other variables:
 
        vidfile >> tileset;
        vidfile >> tilemap;
        vidfile >> scrollx;
        vidfile >> scrolly;
 
 
        vidfile >> palette[0];
        vidfile >> palette[1];
        vidfile >> palette[2];
        vidfile >> palette[3];
 
 
 
3.  Your task now is to calculate a color value 0, 1, 2, 3 for each of the 160x144 pixels on the gameboy screen.  Call function updateSquare(x, y, color) to paint the appropriate square on the window.
 
Read http://imrannazar.com/GameBoy-Emulation-in-JavaScript:-Graphics to see how the gameboy converts video memory into an image.  Briefly, it works like as follows.  Given a pixel x, y, use the following steps to figure out its color:
 
	•	First apply a scroll to shift the image by some amount.  Let x = (x + scrollx)&255 and y =( y + scrolly)&255
 
	•	Next figure out which tile the pixel x,y belongs to.  The 160 x 144 screen is divided into 8x8 tiles.  Given a pixel x, y, the tile coordinate is tilex = x/8 , tiley = y/8.  
 
	•	These tiles are organized in rows from left to right and then from top to bottom.  There are 32 tiles in each row -- more than can be seen on the screen.  The list of tile numbers is called the "tile map".  The tile's position in the map is tileposition = tiley * 32 + tilex
 
	•	There are two tile maps.  Map 0 is located in the graphicsRAM at address 0x1800 (6144), Map 1 is located at address 0x1c00 (7168).  The map actually used depends on the value of the tilemap variable.  The tile index is thus:
◦      if tilemap is 0, tileindex = graphicsRAM[0x1800 + tileposition]
◦      if tilemap is 1, tileindex = graphicsRAM[0x1c00 + tileposition]
 
	•	The tile encoding itself -- the pixel colors in the tile -- is located in the bottom of graphicsRAM.  Every tile entry is 16 bytes in size.  The location of the entry, based on the index, is different for  tilesets 0 and 1.  
·       If the tileset is 1, the address is simply: tileaddress = tileindex * 16
·       If it is 0, however, calculating the address becomes more complicated.  The tile indices are treated as a signed number, with negative numbers occuring below 0x1000 and positive above 0x1000.  The complete formula for tilemap 0 becomes:
·      if tileindex >= 128, tileindex = tileindex - 256
·      tileaddress = tileindex * 16 + 0x1000
 
·      Each 8x8 tile is encoded as 8 rows, each row consisting of two bytes.  To get the two bytes for the pixel we want within the tile, calculate xoffset = x % 8 and yoffset = y % 8   The two bytes -- we'll call them row0 and row1 -- are found at
·      row0 = graphicsRAM[tileaddress + yoffset * 2]
·      row1 = graphicsRAM[tileaddress + yoffset * 2 + 1]
 
 
·      Now for the hardest part.  Each bit within the row byte corresponds to a pixel within the row.  For example, a row of 8 pixels might look like:
 
      3 2 0 0 1 2 3 3
 
      In binary, 0 is 00, 1= 01, 2=10, 3=11.  This sequence becomes:
 
      11  10  00  00  01  10  11  11
 
      Now split this into two bytes.  The first byte gets the lower bits, the second gets the higher bits:
 
      row0 =             1 0 0 0 1 0 1 1             =          139
      row1 =             1 1 0 0 0 1 1 1             =          199
 
The following from http://fms.komkon.org/stuff/gameboy.faq shows how this works across an entire tile:
 
Tile:                                   Image:
 
  .33333..                               .33333.. -> 01111100 -> 7Ch
  22...22.                                           01111100 -> 7Ch
  11...11.                               22...22. -> 00000000 -> 00h
  2222222. <-- digits represent                      11000110 -> C6h
  33...33.     color numbers             11...11. -> 11000110 -> C6h
  22...22.                                           00000000 -> 00h
  11...11.                               2222222. -> 00000000 -> 00h
  ........                                           11111110 -> FEh
                                         33...33. -> 11000110 -> C6h
                                                     11000110 -> C6h
                                         22...22. -> 00000000 -> 00h
                                                     11000110 -> C6h
                                         11...11. -> 11000110 -> C6h
                                                     00000000 -> 00h
                                         ........ -> 00000000 -> 00h
                                                     00000000 -> 00h
 
So how do you get pixel x's color?  Use the bit shifting operator >> to push all the bits to the right.  Then use bitwise AND (&) to zero out all the bits except the one you're interested in.
 
Example:  Suppose xoffset is 1, meaning we want the second pixel from the left (in bold)
 
      row0 = 139 = 1 0 0 0 1 0 1 1
      row0shifted = row0 >>(7 - 1) = 1 0
      row0capturepixel = row0shifted & 1 = 0
 
Now do this again for row1:
 
      row1 = 199 = 1 1 0 0 0 1 1 1
      row1shifted = row1 >> (7 - 1) = 1 1
      row1capturepixel = row1shifted & 1 = 1
 
Put them together:
 
      pixel = row1capturepixel * 2 + row0capturepixel = 1*2 + 0 = 2
 
You're not completely done yet.  pixel doesn't hold the actual color, but an index to a "palette" or array of colors.  Your final color is:
 
      color = palette [pixel];
 
Now apply this to every pixel on the screen:
 
 
      for (int row = 0; row<144; row++)
      for(int column=0; column<160; column++)
                  int x=row, y=column;
                  //do the above calculations to get color
                  updateSquare(column,row,color);
      onFrame();      
                                          
If the test image of a house surrounded by trees shows up, congratulations!  Now put those for loops and computation into a function void renderScreen();
 
 
 
PART 3:  Memory and Timing
 
In this step we will connect your work from the first two parts by adding on memory management and video timing.  When you finish this part you will be able to load and partially run several games on your emulator.
 
Step A.  Memory Map
 
You should now alter your memoryRead and memoryWrite functions so that they not only access the rom but several other memory units as well.  Based on the range of the address, you should implement the following memory layout:
 
Address range (in hex)           memory Read                                     memoryWrite
----------------------------------------------------------------------------------------------------------------------
0 - 0x3FFF                              rom[ address ]                                     setRomMode(address)
0x4000 - 0x7FFF                    rom[ romOffset+address%0x4000]    nothing
0x8000 - 0x9FFF                    graphicsRAM[address%0x2000]       graphicsRAM[address%0x2000]
0xC000 - 0xDFFF                  workingRAM[address%0x2000]        workingRAM[address%0x2000]
0xFF80 - 0xFFFF                   page0RAM[ address % 0x80 ]           page0RAM[ address % 0x80 ]
0xFF00                                    getKey()                                              keyboardColumn
0xFF40                                                                                                setControlByte()
0xFF41                                    getVideoState()                                   videostate
0xFF42                                    scrolly                                                 scrolly
0xFF43                                    scrollx                                                 scrollx
0xFF44                                    line                                                      line
0xFF45                                    cmpline                                               cmpline
0xFF47                                                                                                setPalette()
anything else                           nothing - just return 0
 
 
Now add the following global variables:
 
int HBLANK=0, VBLANK=1, SPRITE=2, VRAM=3;
unsigned char workingRAM[0x2000];
 
unsigned char page0RAM[0x80];
 
int line=0, cmpline=0, videostate=0, keyboardColumn=0, horizontal=0;
int gpuMode=HBLANK;
int romOffset = 0x4000;
long totalInstructions=0;
 
And functions:
 
unsigned char getKey() { return 0xf; }
void setRomMode(int address, unsigned char b) { }
void setControlByte(unsigned char b) { 
        tilemap=(b&8)!=0?1:0;
 
        tileset=(b&16)!=0?1:0;
 }
void setPalette(unsigned char b) { 
            palette[0]=b&3; palette[1]=(b>>2)&3; palette[2]=(b>>4)&3; palette[3]=(b>>6)&3;}
 
unsigned char getVideoState() {
        int by=0;
 
        if(line==cmpline) by|=4;
 
        if(gpuMode==VBLANK) by|=1;
 
        if(gpuMode==SPRITE) by|=2;
 
        if(gpuMode==VRAM) by|=3;
 
        return (unsigned char)((by|(videostate&0xf8))&0xff);
 }
 
Make sure your program still compiles and runs without segmentation faulting.
 
 
Step B.  Video Timing
 
 
Now you will expand your main loop to include video timing.  
Previously, your main loop looked like this:
 
while(true)
            z80->doInstruction();
            if(z80->halted) break;
 
Now you should change this to do the following
 
	•	while true:
·       if not halted, do an instruction
·       check for and handle interrupts
·       figure out the screen position and set the video mode
·       redraw the screen
 
Check for and handle interrupts:
 
When an interrupt comes in from the screen the Z80 doesn't immediately respond but waits for a few instructions.  The following code will delay and call an interrupt at the correct time.
 
                if(z80->interrupt_deferred>0)
 
                {
 
                        z80->interrupt_deferred--;
 
                        if(z80->interrupt_deferred==1)
 
                        {
 
                                z80->interrupt_deferred=0;
 
                                z80->FLAG_I=1;
 
                        }
 
                }
 
                z80->checkForInterrupts();
 
 
Figure out screen position and set the video mode
 
Recall that the gameboy video is handled by a beam moving row by row, from left to right.  Two variables determine what position on the screen is currently being drawn: horizontal and line.  You will use the total number of instructions to update these variables.
 
First keep track of the total instructions.  Put totalInstructions++ at the end of your loop.
 
Now determine your horizontal position.  Given that the gameboy runs approximately 61 instruction for each row of the screen, you can say:
 
horizontal = (int) ((totalInstructions+1)%61);
 
Now set your gpuMode variable using horizontal and line, as follows:
 
	1.	if line is 145 or bigger, your gpuMode is VBLANK
	2.	otherwise, if horizontal is less than or equal to 30, your mode is HBLANK
	3.	otherwise, if horizontal is between 31 and 40, your mode is SPRITE
	4.	otherwise your mode is VRAM
 
 
Check if your horizontal is 0.  If so, do the following steps:
	1.	 Increment line
	2.	If line  is 144, call z80->throwInterrupt(1) to signify that you've reached the VBLANK state
	3.	``There is a special variable cmpline that gameboy programs use to wait until a certain line is reached.  If line%153==cmpline and (videostate&0x40)!=0 , then call z80->throwInterrupt(2)
	4.	If line is 153, set line back to 0, and call your renderScreen function that your wrote in Part 2.
 
Don't call app->exec() anymore.  Just make sure you call onFrame() in your renderScreen function.
 
You can now try loading and running the games TETRIS, OPUS5, and TTT.  Although they won't respond to keystrokes, you should see the games start on your emulator.
Parts 4, 5, and 6 are not yet discussed in this writeup.  To learn them, refer to the online tutorial by imrannazar.
 
PART 4:  ROM Banks
 
Most gameboy games more advanced than Tetris require more memory than there are addresses available.  They accomplish this by dividing the ROM into "banks" which are swapped in and out of memory while the game is running.  Because this is done on the cartridge, rather than in the gameboy itself, there are several different approaches used.
 
Most early gameboy games, including Supermarioland, use a system detailed as "memory bank control 1" here:
 
http://imrannazar.com/GameBoy-Emulation-in-JavaScript:-Memory-Banking
 
 
PART 5:  Keyboard
 
Use the online tutorial.  Uncomment the lines keydown and keyup in the screen.cpp file and implement the functions in your gameboy.cpp.  When this step is completed your game should respond to your computer's keyboard.  You may need to figure out which keycodes map to which keys by trial and error.
 
PART 6:  Sprites
 
Use imrannazar's tutorial.
Grading
 
Part 1               60 %
Part 2               70 %
Part 3               80 %
Part 4, Part 5, Part 6:               10% each
 
 
Submission
 
By the deadline, you should submit on Blackboard:
            - all your .cpp and .h files
            - a zip file of your project directory
            - a statement explaining what you did, what you attempted, and how far you got
 
