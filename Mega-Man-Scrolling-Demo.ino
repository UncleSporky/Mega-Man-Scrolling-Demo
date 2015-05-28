//Mega Man Scrolling Demo
//Created by UncleSporky, 5/2015
//
//This program draws a simple scrolling background with Mega Man
//running in front of it as music plays.  The graphics and music
//are both from Mega Man 2, Wily's castle stage 1, on the NES.
//
//The d-pad changes the greyscale display mode, and A and B
//control the music.  Hold A as you turn on the Arduboy to start
//with the music muted.
//
//Program code copyright (c) UncleSporky 2015
//All rights reserved
//All other copyrights belong to their respective copyright holders

#include <SPI.h>
#include "arduboy.h"
#include "bitmaps.h"
#include "music.h"

#define B_BUTTON 1
#define A_BUTTON 2
#define DOWN 4
#define RIGHT 8
#define UP 16
#define LEFT 32

Arduboy display;

byte frame = 0;       //greyscale frame - 0 displays one grey frame,
                        //1 and 2 display the other, emphasizing some
                        //bits and giving us two shades of grey
byte BGmove = 0;      //counts how many pixels the background has moved
int barrelx = 64;     //location of the background barrels
int cloudx = 16;      //location of the cloud
byte megaCounter = 0; //a counter so we don't advance Mega Man's animation frames too quickly
byte megaFrame = 0;   //Mega Man's animation frame
long delta = 0;       //the time that has passed between one frame of motion and the next
byte buttons;         //stores the current button state
byte drawMode = 2;    //the current user-selected greyscale drawing mode
byte musicMode = 1;   //the current user-selected music mode (1 = play music)

#define SCREENCOUNTER 0  //change this to 1 to display a microsecond counter on the screen

#if SCREENCOUNTER  //microsecond counter variables
byte microCounter = 0;
long microDelta = 0;
long displayDelta = 0;
long lastCheck = 0;
#endif

void setup()
{
    SPI.begin();
    display.start();
    
    buttons = display.getInput();  //begin with a quick check of the A button
    if(buttons & A_BUTTON) {musicMode = 0;}  //if the user wants to start the program muted
}

void loop() {
  //In the drawBitmap code below, (bool)frame * SIZE_WHATEVER is basically saying,
  //if the frame is greater than 0, then skip the first bitmap and render the second
  //one instead.  This is what alternates rapidly between the two bitmaps and displays
  //the image with two shades of grey.
  
  //I have modified the default arduboy.cpp drawBitmap function.
  //It used to | (OR) bitmaps with the background, which meant that you couldn't draw
  //bitmaps on top of existing ones to overwrite them.  I simply changed this to = (equals)
  //to overwrite those pixels.  Without this, a display.clearDisplay() would be
  //necessary here.  Instead, we just paint over the whole screen with background slices.
  
  //I also copied drawBitmap and modified it to create a drawMaskedBitmap function, for
  //drawing Mega Man with transparency.  First it does an & (AND) with the background and
  //the mask to draw black pixels in the shape of Mega Man, and then it does a ^ (XOR) on
  //top of that with the bitmap.  Background in transparent areas is untouched, but the
  //black space gets Mega Man painted on it.
  
  for(byte x = 0; x < 5; x++) {  //the background "slice" is 32 pixels wide, so we need to draw
                                 //5 of them - one will usually be partly offscreen on the left
                                 //side, and one on the right side
    display.drawBitmap((x * 32) - BGmove, 0, bg + ((bool)frame * SIZE_BG), 32, 64, 1);
  }
  
  //draw three barrels
  display.drawBitmap(barrelx, 32, barrel + ((bool)frame * SIZE_BARREL), 16, 16, 1);
  display.drawBitmap(barrelx + 32, 32, barrel + ((bool)frame * SIZE_BARREL), 16, 16, 1);
  display.drawBitmap(barrelx + 32, 24, barrel + ((bool)frame * SIZE_BARREL), 16, 16, 1);

  //draw a cloud
  display.drawBitmap(cloudx / 2, 0, cloud + ((bool)frame * SIZE_CLOUD), 48, 8, 1);
  
  //draw Mega Man, with his sprite masked in order to do transparency
  //the megaFrame calculation skips downward to later frames of his run animation
  display.drawMaskedBitmap(52, 24, megaman + (megaFrame * (SIZE_MEGAMAN * 3)) + ((bool)frame * SIZE_MEGAMAN), 
                                   megaman + (megaFrame * (SIZE_MEGAMAN * 3)) + (SIZE_MEGAMAN * 2), 24, 24, 1);

  //If screencounter is set to 1, this block of code will keep track of
  //how many microseconds has passed every iteration through the main loop
  //and will display it in the top left corner on the screen.
  //As of 5/2015, this appears to hover in the range of 11,400, but there
  //can be a wide variance from 10,000 to 12,000. (10 to 12 milliseconds.)
  #if SCREENCOUNTER
  microDelta = micros() - lastCheck;
  lastCheck = micros();

  microCounter++;
  if(microCounter > 200) {
    microCounter = 0;
    displayDelta = microDelta;
  }
  display.setCursor(0,0);
  display.print(displayDelta);
  #endif
  
  //copy the buffer to the screen and show us the picture we have built!
  display.display();
  
  //advance the greyscale control frame
  frame++;
  if(frame > drawMode) {frame = 0;}
  if(drawMode == 0) {frame = 2;}
  
  if(musicMode && !display.tune_playing) {display.tune_playscore(wily);}  //play music, and loop it if it finishes playing
  if(!musicMode && display.tune_playing) {display.tune_stopscore();}  //stop music if the user has chosen to turn it off
  
  //this delta calculation keeps the animation from happening too fast
  if(millis() >= delta) {
    BGmove++;  //move the background
    if(BGmove > 31) {BGmove = 0;}

    barrelx--;  //move the set of barrels
    if(barrelx < -47) {barrelx = 128;}
    
    cloudx--;  //move the cloud
    if(cloudx < -95) {cloudx = 256;}
    
    megaCounter++;  //animate Mega Man; the 6 below controls how quickly his run animates
    if(megaCounter > 6) {megaCounter = 0; megaFrame++;}
    if(megaFrame > 3) {megaFrame = 0;}  //Mega Man has 4 frames in his run animation
    
    buttons = display.getInput();  //get the current button state
    
    if(buttons & LEFT) {drawMode = 2;}  //when you press left, you're in mode 0, which shows two shades of grey
    else if(buttons & RIGHT) {drawMode = 1;}  //when you press right, you're in mode 1, which shows one shade of grey
    else if(buttons & UP) {drawMode = 0;}  //when you press up, you're in mode 2, which draws in black and white
    
    if(buttons & A_BUTTON) {musicMode = 0;}  //stop playing music when the player presses A
    else if(buttons & B_BUTTON) {musicMode = 1;}  //resume playing music when the player presses B
    
    //change the 10 here to make the animation faster or slower!  less is faster    
    delta = millis() + 10;
  }
}
