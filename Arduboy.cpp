#include "Arduboy.h"
//#include "glcdfont.c"

#define AVAILABLE_TIMERS 2
const byte PROGMEM tune_pin_to_timer_PGM[] = { 3, 1 };
volatile byte *timer1_pin_port;
volatile byte timer1_pin_mask;
volatile byte *timer3_pin_port;
volatile byte timer3_pin_mask;
byte _tune_pins[AVAILABLE_TIMERS];
byte _tune_num_chans = 0;
volatile unsigned wait_timer_frequency2;       /* its current frequency */
volatile unsigned wait_timer_old_frequency2;   /* its previous frequency */
volatile boolean wait_timer_playing = false;   /* is it currently playing a note? */
volatile boolean doing_delay = false;          /* are we using it for a tune_delay()? */
volatile unsigned long wait_toggle_count;      /* countdown score waits */
volatile unsigned long delay_toggle_count;     /* countdown tune_ delay() delays */
volatile const byte *score_start = 0;
volatile const byte *score_cursor = 0;
volatile boolean Arduboy::tune_playing = false;
const unsigned int PROGMEM tune_frequencies2_PGM[128] = { 
16,17,18,19,21,22,23,24,26,28,29,31,33,35,37,39,41,44,46,49,52,55,58,62,65,
69,73,78,82,87,92,98,104,110,117,123,131,139,147,156,165,175,185,196,208,220,
233,247,262,277,294,311,330,349,370,392,415,440,466,494,523,554,587,622,659,
698,740,784,831,880,932,988,1047,1109,1175,1245,1319,1397,1480,1568,1661,1760,
1865,1976,2093,2217,2349,2489,2637,2794,2960,3136,3322,3520,3729,3951,4186,
4435,4699,4978,5274,5588,5920,6272,6645,7040,7459,7902,8372,8870,9397,9956,
10548,11175,11840,12544,13290,14080,14917,15804,16744,17740,18795,19912,21096,
22351,23680,25088 };
void tune_playnote (byte chan, byte note);
void tune_stopnote (byte chan);
void tune_stepscore (void);

#if TINYFONT == 0
// 5x7 Font
#define FONTWIDTH  5
#define FONTHEIGHT 7
const static unsigned char PROGMEM font[] = {
0x00,0x00,0x00,0x00,0x00,0x3E,0x5B,0x4F,0x5B,0x3E,0x3E,0x6B,0x4F,0x6B,0x3E,
0x1C,0x3E,0x7C,0x3E,0x1C,0x18,0x3C,0x7E,0x3C,0x18,0x1C,0x57,0x7D,0x57,0x1C,
0x1C,0x5E,0x7F,0x5E,0x1C,0x00,0x18,0x3C,0x18,0x00,0xFF,0xE7,0xC3,0xE7,0xFF,
0x00,0x18,0x24,0x18,0x00,0xFF,0xE7,0xDB,0xE7,0xFF,0x30,0x48,0x3A,0x06,0x0E,
0x26,0x29,0x79,0x29,0x26,0x40,0x7F,0x05,0x05,0x07,0x40,0x7F,0x05,0x25,0x3F,
0x5A,0x3C,0xE7,0x3C,0x5A,0x7F,0x3E,0x1C,0x1C,0x08,0x08,0x1C,0x1C,0x3E,0x7F,
0x14,0x22,0x7F,0x22,0x14,0x5F,0x5F,0x00,0x5F,0x5F,0x06,0x09,0x7F,0x01,0x7F,
0x00,0x66,0x89,0x95,0x6A,0x60,0x60,0x60,0x60,0x60,0x94,0xA2,0xFF,0xA2,0x94,
0x08,0x04,0x7E,0x04,0x08,0x10,0x20,0x7E,0x20,0x10,0x08,0x08,0x2A,0x1C,0x08,
0x08,0x1C,0x2A,0x08,0x08,0x1E,0x10,0x10,0x10,0x10,0x0C,0x1E,0x0C,0x1E,0x0C,
0x30,0x38,0x3E,0x38,0x30,0x06,0x0E,0x3E,0x0E,0x06,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x5F,0x00,0x00,0x00,0x07,0x00,0x07,0x00,0x14,0x7F,0x14,0x7F,0x14,
0x24,0x2A,0x7F,0x2A,0x12,0x23,0x13,0x08,0x64,0x62,0x36,0x49,0x56,0x20,0x50,
0x00,0x08,0x07,0x03,0x00,0x00,0x1C,0x22,0x41,0x00,0x00,0x41,0x22,0x1C,0x00,
0x2A,0x1C,0x7F,0x1C,0x2A,0x08,0x08,0x3E,0x08,0x08,0x00,0x80,0x70,0x30,0x00,
0x08,0x08,0x08,0x08,0x08,0x00,0x00,0x60,0x60,0x00,0x20,0x10,0x08,0x04,0x02,
0x3E,0x51,0x49,0x45,0x3E,0x00,0x42,0x7F,0x40,0x00,0x72,0x49,0x49,0x49,0x46,
0x21,0x41,0x49,0x4D,0x33,0x18,0x14,0x12,0x7F,0x10,0x27,0x45,0x45,0x45,0x39,
0x3C,0x4A,0x49,0x49,0x31,0x41,0x21,0x11,0x09,0x07,0x36,0x49,0x49,0x49,0x36,
0x46,0x49,0x49,0x29,0x1E,0x00,0x00,0x14,0x00,0x00,0x00,0x40,0x34,0x00,0x00,
0x00,0x08,0x14,0x22,0x41,0x14,0x14,0x14,0x14,0x14,0x00,0x41,0x22,0x14,0x08,
0x02,0x01,0x59,0x09,0x06,0x3E,0x41,0x5D,0x59,0x4E,0x7C,0x12,0x11,0x12,0x7C,
0x7F,0x49,0x49,0x49,0x36,0x3E,0x41,0x41,0x41,0x22,0x7F,0x41,0x41,0x41,0x3E,
0x7F,0x49,0x49,0x49,0x41,0x7F,0x09,0x09,0x09,0x01,0x3E,0x41,0x41,0x51,0x73,
0x7F,0x08,0x08,0x08,0x7F,0x00,0x41,0x7F,0x41,0x00,0x20,0x40,0x41,0x3F,0x01,
0x7F,0x08,0x14,0x22,0x41,0x7F,0x40,0x40,0x40,0x40,0x7F,0x02,0x1C,0x02,0x7F,
0x7F,0x04,0x08,0x10,0x7F,0x3E,0x41,0x41,0x41,0x3E,0x7F,0x09,0x09,0x09,0x06,
0x3E,0x41,0x51,0x21,0x5E,0x7F,0x09,0x19,0x29,0x46,0x26,0x49,0x49,0x49,0x32,
0x03,0x01,0x7F,0x01,0x03,0x3F,0x40,0x40,0x40,0x3F,0x1F,0x20,0x40,0x20,0x1F,
0x3F,0x40,0x38,0x40,0x3F,0x63,0x14,0x08,0x14,0x63,0x03,0x04,0x78,0x04,0x03,
0x61,0x59,0x49,0x4D,0x43,0x00,0x7F,0x41,0x41,0x41,0x02,0x04,0x08,0x10,0x20,
0x00,0x41,0x41,0x41,0x7F,0x04,0x02,0x01,0x02,0x04,0x40,0x40,0x40,0x40,0x40,
0x00,0x03,0x07,0x08,0x00,0x20,0x54,0x54,0x78,0x40,0x7F,0x28,0x44,0x44,0x38,
0x38,0x44,0x44,0x44,0x28,0x38,0x44,0x44,0x28,0x7F,0x38,0x54,0x54,0x54,0x18,
0x00,0x08,0x7E,0x09,0x02,0x18,0xA4,0xA4,0x9C,0x78,0x7F,0x08,0x04,0x04,0x78,
0x00,0x44,0x7D,0x40,0x00,0x20,0x40,0x40,0x3D,0x00,0x7F,0x10,0x28,0x44,0x00,
0x00,0x41,0x7F,0x40,0x00,0x7C,0x04,0x78,0x04,0x78,0x7C,0x08,0x04,0x04,0x78,
0x38,0x44,0x44,0x44,0x38,0xFC,0x18,0x24,0x24,0x18,0x18,0x24,0x24,0x18,0xFC,
0x7C,0x08,0x04,0x04,0x08,0x48,0x54,0x54,0x54,0x24,0x04,0x04,0x3F,0x44,0x24,
0x3C,0x40,0x40,0x20,0x7C,0x1C,0x20,0x40,0x20,0x1C,0x3C,0x40,0x30,0x40,0x3C,
0x44,0x28,0x10,0x28,0x44,0x4C,0x90,0x90,0x90,0x7C,0x44,0x64,0x54,0x4C,0x44,
0x00,0x08,0x36,0x41,0x00,0x00,0x00,0x77,0x00,0x00,0x00,0x41,0x36,0x08,0x00,
0x02,0x01,0x02,0x04,0x02,0x3C,0x26,0x23,0x26,0x3C,0x1E,0xA1,0xA1,0x61,0x12,
0x3A,0x40,0x40,0x20,0x7A,0x38,0x54,0x54,0x55,0x59,0x21,0x55,0x55,0x79,0x41,
0x21,0x54,0x54,0x78,0x41,0x21,0x55,0x54,0x78,0x40,0x20,0x54,0x55,0x79,0x40,
0x0C,0x1E,0x52,0x72,0x12,0x39,0x55,0x55,0x55,0x59,0x39,0x54,0x54,0x54,0x59,
0x39,0x55,0x54,0x54,0x58,0x00,0x00,0x45,0x7C,0x41,0x00,0x02,0x45,0x7D,0x42,
0x00,0x01,0x45,0x7C,0x40,0xF0,0x29,0x24,0x29,0xF0,0xF0,0x28,0x25,0x28,0xF0,
0x7C,0x54,0x55,0x45,0x00,0x20,0x54,0x54,0x7C,0x54,0x7C,0x0A,0x09,0x7F,0x49,
0x32,0x49,0x49,0x49,0x32,0x32,0x48,0x48,0x48,0x32,0x32,0x4A,0x48,0x48,0x30,
0x3A,0x41,0x41,0x21,0x7A,0x3A,0x42,0x40,0x20,0x78,0x00,0x9D,0xA0,0xA0,0x7D,
0x39,0x44,0x44,0x44,0x39,0x3D,0x40,0x40,0x40,0x3D,0x3C,0x24,0xFF,0x24,0x24,
0x48,0x7E,0x49,0x43,0x66,0x2B,0x2F,0xFC,0x2F,0x2B,0xFF,0x09,0x29,0xF6,0x20,
0xC0,0x88,0x7E,0x09,0x03,0x20,0x54,0x54,0x79,0x41,0x00,0x00,0x44,0x7D,0x41,
0x30,0x48,0x48,0x4A,0x32,0x38,0x40,0x40,0x22,0x7A,0x00,0x7A,0x0A,0x0A,0x72,
0x7D,0x0D,0x19,0x31,0x7D,0x26,0x29,0x29,0x2F,0x28,0x26,0x29,0x29,0x29,0x26,
0x30,0x48,0x4D,0x40,0x20,0x38,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x38,
0x2F,0x10,0xC8,0xAC,0xBA,0x2F,0x10,0x28,0x34,0xFA,0x00,0x00,0x7B,0x00,0x00,
0x08,0x14,0x2A,0x14,0x22,0x22,0x14,0x2A,0x14,0x08,0xAA,0x00,0x55,0x00,0xAA,
0xAA,0x55,0xAA,0x55,0xAA,0x00,0x00,0x00,0xFF,0x00,0x10,0x10,0x10,0xFF,0x00,
0x14,0x14,0x14,0xFF,0x00,0x10,0x10,0xFF,0x00,0xFF,0x10,0x10,0xF0,0x10,0xF0,
0x14,0x14,0x14,0xFC,0x00,0x14,0x14,0xF7,0x00,0xFF,0x00,0x00,0xFF,0x00,0xFF,
0x14,0x14,0xF4,0x04,0xFC,0x14,0x14,0x17,0x10,0x1F,0x10,0x10,0x1F,0x10,0x1F,
0x14,0x14,0x14,0x1F,0x00,0x10,0x10,0x10,0xF0,0x00,0x00,0x00,0x00,0x1F,0x10,
0x10,0x10,0x10,0x1F,0x10,0x10,0x10,0x10,0xF0,0x10,0x00,0x00,0x00,0xFF,0x10,
0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0xFF,0x10,0x00,0x00,0x00,0xFF,0x14,
0x00,0x00,0xFF,0x00,0xFF,0x00,0x00,0x1F,0x10,0x17,0x00,0x00,0xFC,0x04,0xF4,
0x14,0x14,0x17,0x10,0x17,0x14,0x14,0xF4,0x04,0xF4,0x00,0x00,0xFF,0x00,0xF7,
0x14,0x14,0x14,0x14,0x14,0x14,0x14,0xF7,0x00,0xF7,0x14,0x14,0x14,0x17,0x14,
0x10,0x10,0x1F,0x10,0x1F,0x14,0x14,0x14,0xF4,0x14,0x10,0x10,0xF0,0x10,0xF0,
0x00,0x00,0x1F,0x10,0x1F,0x00,0x00,0x00,0x1F,0x14,0x00,0x00,0x00,0xFC,0x14,
0x00,0x00,0xF0,0x10,0xF0,0x10,0x10,0xFF,0x10,0xFF,0x14,0x14,0x14,0xFF,0x14,
0x10,0x10,0x10,0x1F,0x00,0x00,0x00,0x00,0xF0,0x10,0xFF,0xFF,0xFF,0xFF,0xFF,
0xF0,0xF0,0xF0,0xF0,0xF0,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,
0x0F,0x0F,0x0F,0x0F,0x0F,0x38,0x44,0x44,0x38,0x44,0x7C,0x2A,0x2A,0x3E,0x14,
0x7E,0x02,0x02,0x06,0x06,0x02,0x7E,0x02,0x7E,0x02,0x63,0x55,0x49,0x41,0x63,
0x38,0x44,0x44,0x3C,0x04,0x40,0x7E,0x20,0x1E,0x20,0x06,0x02,0x7E,0x02,0x02,
0x99,0xA5,0xE7,0xA5,0x99,0x1C,0x2A,0x49,0x2A,0x1C,0x4C,0x72,0x01,0x72,0x4C,
0x30,0x4A,0x4D,0x4D,0x30,0x30,0x48,0x78,0x48,0x30,0xBC,0x62,0x5A,0x46,0x3D,
0x3E,0x49,0x49,0x49,0x00,0x7E,0x01,0x01,0x01,0x7E,0x2A,0x2A,0x2A,0x2A,0x2A,
0x44,0x44,0x5F,0x44,0x44,0x40,0x51,0x4A,0x44,0x40,0x40,0x44,0x4A,0x51,0x40,
0x00,0x00,0xFF,0x01,0x03,0xE0,0x80,0xFF,0x00,0x00,0x08,0x08,0x6B,0x6B,0x08,
0x36,0x12,0x36,0x24,0x36,0x06,0x0F,0x09,0x0F,0x06,0x00,0x00,0x18,0x18,0x00,
0x00,0x00,0x10,0x10,0x00,0x30,0x40,0xFF,0x01,0x01,0x00,0x1F,0x01,0x01,0x1E,
0x00,0x19,0x1D,0x17,0x12,0x00,0x3C,0x3C,0x3C,0x3C,0x00,0x00,0x00,0x00,0x00
};
#else
// 4x5 Font
#define FONTWIDTH  4
#define FONTHEIGHT 5
const static unsigned char PROGMEM font[] = {
    0x04,0x04,0x04,0x00,0x04,   // <space> !
    0xAA,0xAE,0x0A,0x0E,0x0A,   // " #
    0x6A,0xC2,0xE4,0x68,0xCA,   // 0x %
    0x44,0xE4,0xC0,0xA0,0xE0,   // & '
    0x24,0x42,0x42,0x42,0x24,   // ( )
    0x00,0xA4,0x4E,0xA4,0x00,   // * +
    0x00,0x00,0x0E,0x40,0x80,   // , -
    0x02,0x02,0x04,0x08,0x88,   // . /
    0x4C,0xA4,0xA4,0xA4,0x4E,   // 0 1
    0xCE,0x22,0x44,0x82,0xEE,   // 2 3
    0xAE,0xA8,0xEC,0x22,0x2C,   // 4 5
    0x6E,0x82,0xE4,0xA4,0xE4,   // 6 7
    0xEE,0xAA,0x4E,0xA2,0xEC,   // 8 9
    0x00,0x44,0x00,0x44,0x08,   // : //
    0x20,0x4E,0x80,0x4E,0x20,   // < =
    0x8C,0x42,0x24,0x40,0x84,   // > ?
    0x44,0xAA,0xEE,0x8A,0x6A,   // @ A
    0xEE,0xA8,0xC8,0xA8,0xEE,   // B C
    0xCE,0xA8,0xAC,0xA8,0xCE,   // D E
    0xE6,0x88,0xCA,0x8A,0x8E,   // F G
    0xAE,0xA4,0xE4,0xA4,0xAE,   // H I
    0x2A,0x2A,0x2C,0xAA,0xEA,   // J K
    0x8A,0x8E,0x8E,0x8A,0xEA,   // L M
    0xCE,0xAA,0xAA,0xAA,0xAE,   // N O
    0xEE,0xAA,0xEA,0x8E,0x8E,   // P Q
    0xCE,0xA8,0xCE,0xA2,0xAE,   // R S
    0xEA,0x4A,0x4A,0x4A,0x4E,   // T U
    0xAA,0xAA,0xAE,0xAE,0x4A,   // V W
    0xAA,0xAA,0x4E,0xA4,0xA4,   // X Y
    0xE6,0x24,0x44,0x84,0xE6,   // Z [
    0x86,0x82,0x42,0x22,0x26,   // \ ]
    0x40,0xA0,0x00,0x00,0x0E,   // ^ _
    0x80,0x40,0x0E,0x0A,0x06,   // ` a
    0x00,0x80,0xC6,0xA8,0xC6,   // b c
    0x00,0x24,0x6A,0xAC,0x66,   // d e
    0x00,0x24,0x4A,0x66,0x4C,   // f g
    0x00,0x84,0xC0,0xA4,0xA4,   // h i
    0x40,0x08,0x4A,0x4C,0x8A,   // j k
    0x00,0x40,0x4E,0x4E,0x2A,   // l m
    0x00,0x00,0xCE,0xAA,0xAE,   // n o
    0x00,0x44,0xAA,0xC6,0x82,   // p q
    0x00,0x04,0x28,0x44,0x48,   // r s
    0x00,0x40,0x6A,0x4A,0x26,   // t u
    0x00,0x00,0xAA,0xAE,0x4E,   // v w
    0x00,0x00,0xAA,0x46,0xAC,   // x y
    0x06,0x84,0x48,0x84,0x46,   // z {
    0x0C,0x44,0x42,0x44,0x4C,   // | }
    0x0E,0x2E,0xEE,0x8E,0x0E    // ~ <block>
};
#endif

Arduboy::Arduboy() {
}
void Arduboy::start() {
    pinMode(DC, OUTPUT);
    pinMode(CS, OUTPUT);
    pinMode(8, INPUT_PULLUP);  // D-Pad Up
    pinMode(9, INPUT_PULLUP);  // D-Pad Left
    pinMode(10, INPUT_PULLUP); // D-Pad Down
    pinMode(5, INPUT_PULLUP);  // D-Pad Right
    pinMode(A0, INPUT_PULLUP); // A? Button
    pinMode(A1, INPUT_PULLUP); // B? Button	
    tune_initchan(A2);  // Speaker Pin 1
    tune_initchan(A3);  // Speaker Pin 2
	
    csport      = portOutputRegister(digitalPinToPort(CS));
    cspinmask   = digitalPinToBitMask(CS);
    dcport      = portOutputRegister(digitalPinToPort(DC));
    dcpinmask   = digitalPinToBitMask(DC);
    // Setup reset pin direction (used by both SPI and I2C)  
    pinMode(RST, OUTPUT);
    digitalWrite(RST, HIGH);
    delay(1);               // VDD (3.3V) goes high at start, lets just chill for a ms
    digitalWrite(RST, LOW); // bring reset low
    delay(10);              // wait 10ms
    digitalWrite(RST, HIGH);// bring out of reset
    *csport |= cspinmask;
    *dcport &= ~dcpinmask;
    *csport &= ~cspinmask;
    SPI.transfer(0xAE);	// Display Off
    SPI.transfer(0XD5);	// Set Display Clock Divisor v
    SPI.transfer(0xF0); // 0x80 is default 
    SPI.transfer(0xA8);	// Set Multiplex Ratio v
    SPI.transfer(0x3F); //
    SPI.transfer(0xD3); // Set Display Offset v
    SPI.transfer(0x0);	//
    SPI.transfer(0x40);	// Set Start Line (0)
    SPI.transfer(0x8D);	// Charge Pump Setting v
    SPI.transfer(0x14); // Enable
    SPI.transfer(0x20);	// Set Memory Mode v
    SPI.transfer(0x00); // Horizontal Addressing
    SPI.transfer(0xA1); // Set Segment Re-map (A0) | (b0001)
    SPI.transfer(0xC8); // Set COM Output Scan Direction
    SPI.transfer(0xDA); // Set COM Pins v
    SPI.transfer(0x12); //
    SPI.transfer(0x81); // Set Contrast v
    SPI.transfer(0xCF); //
    SPI.transfer(0xD9); // Set Precharge
    SPI.transfer(0xF1); //
    SPI.transfer(0xDB); // Set VCom Detect
    SPI.transfer(0x40); //
    SPI.transfer(0xA4); // Entire Display ON
    SPI.transfer(0xA6); // Set normal/inverse display
    SPI.transfer(0xAF); // Display On
    *csport |= cspinmask;
    *dcport &= ~dcpinmask;
    *csport &= ~cspinmask;
    unsigned char end = WIDTH-1;
    SPI.transfer(0x20); // set display mode
    SPI.transfer(0x00); // Horizontal Addressing
    SPI.transfer(0x21); // set col address
    SPI.transfer(0x00);
    SPI.transfer(end & 0x7F);
    SPI.transfer(0x22); // set page address
    end = (HEIGHT/8)-1;
    SPI.transfer(0x00);
    SPI.transfer(end & 0x07);	
    *dcport |= dcpinmask;
    *csport &= ~cspinmask;
    clearDisplay();
    display();
	setTextSize(1);
}
void Arduboy::clearDisplay() {
    for (int a = 0; a < (HEIGHT*WIDTH)/8; a++) sBuffer[a] = 0x00;
}
void Arduboy::drawPixel(int x, int y, uint16_t value) {
    if (x < 0 || x > (WIDTH-1) || y < 0 || y > (HEIGHT-1)) return;
    int row = y / 8;
    if (value)  sBuffer[(row*WIDTH) + x] |=   1 << (y % 8);
    else        sBuffer[(row*WIDTH) + x] &= ~(1 << (y % 8));
}
void Arduboy::drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  drawPixel(x0, y0+r, color);
  drawPixel(x0, y0-r, color);
  drawPixel(x0+r, y0, color);
  drawPixel(x0-r, y0, color);

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
  
    drawPixel(x0 + x, y0 + y, color);
    drawPixel(x0 - x, y0 + y, color);
    drawPixel(x0 + x, y0 - y, color);
    drawPixel(x0 - x, y0 - y, color);
    drawPixel(x0 + y, y0 + x, color);
    drawPixel(x0 - y, y0 + x, color);
    drawPixel(x0 + y, y0 - x, color);
    drawPixel(x0 - y, y0 - x, color);
    
  }
}
void Arduboy::drawCircleHelper( int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color) {
  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;
    if (cornername & 0x4) {
      drawPixel(x0 + x, y0 + y, color);
      drawPixel(x0 + y, y0 + x, color);
    } 
    if (cornername & 0x2) {
      drawPixel(x0 + x, y0 - y, color);
      drawPixel(x0 + y, y0 - x, color);
    }
    if (cornername & 0x8) {
      drawPixel(x0 - y, y0 + x, color);
      drawPixel(x0 - x, y0 + y, color);
    }
    if (cornername & 0x1) {
      drawPixel(x0 - y, y0 - x, color);
      drawPixel(x0 - x, y0 - y, color);
    }
  }
}
void Arduboy::fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
  drawFastVLine(x0, y0-r, 2*r+1, color);
  fillCircleHelper(x0, y0, r, 3, 0, color);
}
void Arduboy::fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color) {
  // used to do circles and roundrects!
  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;
  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;
    if (cornername & 0x1) {
      drawFastVLine(x0+x, y0-y, 2*y+1+delta, color);
      drawFastVLine(x0+y, y0-x, 2*x+1+delta, color);
    }
    if (cornername & 0x2) {
      drawFastVLine(x0-x, y0-y, 2*y+1+delta, color);
      drawFastVLine(x0-y, y0-x, 2*x+1+delta, color);
    }
  }
}
void Arduboy::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
  // bresenham's algorithm - thx wikpedia
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    swap(x0, y0);
    swap(x1, y1);
  }
  if (x0 > x1) {
    swap(x0, x1);
    swap(y0, y1);
  }
  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);
  int16_t err = dx / 2;
  int16_t ystep;
  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }
  for (; x0<=x1; x0++) {
    if (steep) {
      drawPixel(y0, x0, color);
    } else {
      drawPixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}
void Arduboy::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  drawFastHLine(x, y, w, color);
  drawFastHLine(x, y+h-1, w, color);
  drawFastVLine(x, y, h, color);
  drawFastVLine(x+w-1, y, h, color);
}
void Arduboy::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
  int end = y+h;
  for (int a = max(0,y); a < min(end,HEIGHT); a++) drawPixel(x,a,color);
}
void Arduboy::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
  int end = x+w;
  for (int a = max(0,x); a < min(end,WIDTH); a++) drawPixel(a,y,color);
}
void Arduboy::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t color) {
  // stupidest version - update in subclasses if desired!
  for (int16_t i=x; i<x+w; i++) {
    drawFastVLine(i, y, h, color); 
  }
}
void Arduboy::drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
  // smarter version
  drawFastHLine(x+r  , y    , w-2*r, color); // Top
  drawFastHLine(x+r  , y+h-1, w-2*r, color); // Bottom
  drawFastVLine(  x    , y+r  , h-2*r, color); // Left
  drawFastVLine(  x+w-1, y+r  , h-2*r, color); // Right
  // draw four corners
  drawCircleHelper(x+r    , y+r    , r, 1, color);
  drawCircleHelper(x+w-r-1, y+r    , r, 2, color);
  drawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
  drawCircleHelper(x+r    , y+h-r-1, r, 8, color);
}
void Arduboy::fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
  // smarter version
  fillRect(x+r, y, w-2*r, h, color);

  // draw four corners
  fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
  fillCircleHelper(x+r    , y+r, r, 2, h-2*r-1, color);
}
void Arduboy::drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
  drawLine(x0, y0, x1, y1, color);
  drawLine(x1, y1, x2, y2, color);
  drawLine(x2, y2, x0, y0, color);
}
void Arduboy::fillTriangle (int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
  int16_t a, b, y, last;
  // Sort coordinates by Y order (y2 >= y1 >= y0)
  if (y0 > y1) {
    swap(y0, y1); swap(x0, x1);
  }
  if (y1 > y2) {
    swap(y2, y1); swap(x2, x1);
  }
  if (y0 > y1) {
    swap(y0, y1); swap(x0, x1);
  }
  if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
    a = b = x0;
    if(x1 < a)      a = x1;
    else if(x1 > b) b = x1;
    if(x2 < a)      a = x2;
    else if(x2 > b) b = x2;
    drawFastHLine(a, y0, b-a+1, color);
    return;
  }
  int16_t
    dx01 = x1 - x0,
    dy01 = y1 - y0,
    dx02 = x2 - x0,
    dy02 = y2 - y0,
    dx12 = x2 - x1,
    dy12 = y2 - y1,
    sa = 0,
    sb = 0;

  // For upper part of triangle, find scanline crossings for segments
  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
  // is included here (and second loop will be skipped, avoiding a /0
  // error there), otherwise scanline y1 is skipped here and handled
  // in the second loop...which also avoids a /0 error here if y0=y1
  // (flat-topped triangle).
  if(y1 == y2) last = y1;   // Include y1 scanline
  else         last = y1-1; // Skip it

  for(y=y0; y<=last; y++) {
    a   = x0 + sa / dy01;
    b   = x0 + sb / dy02;
    sa += dx01;
    sb += dx02;
    /*	longhand:
      a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
      b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if(a > b) swap(a,b);
    drawFastHLine(a, y, b-a+1, color);
  }

  // For lower part of triangle, find scanline crossings for segments
  // 0-2 and 1-2.  This loop is skipped if y1=y2.
  sa = dx12 * (y - y1);
  sb = dx02 * (y - y0);
  for(; y<=y2; y++) {
    a   = x1 + sa / dy12;
    b   = x0 + sb / dy02;
    sa += dx12;
    sb += dx02;
    /*	longhand:
      a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
      b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if(a > b) swap(a,b);
    drawFastHLine(a, y, b-a+1, color);
  }
}
void Arduboy::drawBitmap
(int16_t x,
 int16_t y,
 const uint8_t *bitmap,
 int16_t w,
 int16_t h,
 uint16_t color
)
{
  //bitmap is off screen
  if (x+w < 0 || x > WIDTH-1 || y+h < 0 || y > HEIGHT-1) return;

  int yOffset = abs(y) % 8;
  int sRow = y / 8;

  if (y < 0)
  {
    sRow--;
    yOffset = 8 - yOffset;
  }

  for (int a = 0; a < h/8; a++)
  {
    int bRow = sRow + a;
    if (bRow > (HEIGHT/8)-1) break;
    if (bRow > -2) {
      for (int iCol = 0; iCol<w; iCol++)
      {
        if (iCol + x > (WIDTH-1))
        {
          break;
        }

        if (iCol + x > 0)
        {
          if (bRow >= 0)
          {
            if (color)
            {
              this->sBuffer[ (bRow*WIDTH) + x + iCol  ]
                //|= pgm_read_byte(bitmap+(a*w)+iCol) << yOffset;
                = pgm_read_byte(bitmap+(a*w)+iCol) << yOffset; //changed to equals in order to overwrite the background, no clear required
            }
            else
            {
              this->sBuffer[ (bRow*WIDTH) + x + iCol  ]
                &= ~(pgm_read_byte(bitmap+(a*w)+iCol) << yOffset);
            }
          }
          if (yOffset && bRow<(HEIGHT/8)-1 && bRow > -2)
          {
            if (color)
            {
              this->sBuffer[ ((bRow+1)*WIDTH) + x + iCol  ]
                //|= pgm_read_byte(bitmap+(a*w)+iCol) >> (8-yOffset);
                = pgm_read_byte(bitmap+(a*w)+iCol) >> (8-yOffset); //changed to equals in order to overwrite the background, no clear required
            }
            else
            {
              this->sBuffer[ ((bRow+1)*WIDTH) + x + iCol  ]
                &= ~(pgm_read_byte(bitmap+(a*w)+iCol) >> (8-yOffset));
            }
          }
        }
      }
    }
  }
}

void Arduboy::drawMaskedBitmap
(int16_t x,
 int16_t y,
 const uint8_t *bitmap,
 const uint8_t *mask,
 int16_t w,
 int16_t h,
 uint16_t color
)
{
  //bitmap is off screen
  if (x+w < 0 || x > WIDTH-1 || y+h < 0 || y > HEIGHT-1) return;

  int yOffset = abs(y) % 8;
  int sRow = y / 8;

  if (y < 0)
  {
    sRow--;
    yOffset = 8 - yOffset;
  }

  for (int a = 0; a < h/8; a++)
  {
    int bRow = sRow + a;
    if (bRow > (HEIGHT/8)-1) break;
    if (bRow > -2) {
      for (int iCol = 0; iCol<w; iCol++)
      {
        if (iCol + x > (WIDTH-1))
        {
          break;
        }

        if (iCol + x > 0)
        {
          if (bRow >= 0)
          {
//            if (color)
//            {
              this->sBuffer[ (bRow*WIDTH) + x + iCol  ]
                &= pgm_read_byte(mask+(a*w)+iCol) << yOffset;
              this->sBuffer[ (bRow*WIDTH) + x + iCol  ]
                ^= pgm_read_byte(bitmap+(a*w)+iCol) << yOffset;
//            }
//            else
//            {
//              this->sBuffer[ (bRow*WIDTH) + x + iCol  ]
//                &= ~(pgm_read_byte(bitmap+(a*w)+iCol) << yOffset);
//            }
          }
          if (yOffset && bRow<(HEIGHT/8)-1 && bRow > -2)
          {
//            if (color)
//            {
              this->sBuffer[ ((bRow+1)*WIDTH) + x + iCol  ]
                &= pgm_read_byte(mask+(a*w)+iCol) >> (8-yOffset);
              this->sBuffer[ ((bRow+1)*WIDTH) + x + iCol  ]
                ^= pgm_read_byte(bitmap+(a*w)+iCol) >> (8-yOffset);
//            }
//            else
//            {
//              this->sBuffer[ ((bRow+1)*WIDTH) + x + iCol  ]
//                &= ~(pgm_read_byte(bitmap+(a*w)+iCol) >> (8-yOffset));
//            }
          }
        }
      }
    }
  }
}

void Arduboy::drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size) {
#if TINYFONT == 0
  if((x >= WIDTH)            || // Clip right
    (y >= HEIGHT)            || // Clip bottom
    ((x + 6 * size - 1) < 0) || // Clip left
    ((y + 8 * size - 1) < 0))   // Clip top
    return;
  for (int8_t i=0; i<6; i++ ) {
    uint8_t line;
    if (i == 5) 
      line = 0x0;
    else 
      line = pgm_read_byte(font+(c*5)+i);
    for (int8_t j = 0; j<8; j++) {
      if (line & 0x1) {
        if (size == 1) // default size
          drawPixel(x+i, y+j, color);
        else {  // big size
          fillRect(x+(i*size), y+(j*size), size, size, color);
        } 
      } else if (bg != color) {
        if (size == 1) // default size
          drawPixel(x+i, y+j, bg);
        else {  // big size
          fillRect(x+i*size, y+j*size, size, size, bg);
        } 	
      }
      line >>= 1;
    }
  }
#else
    if(
        (x >= WIDTH) || // Clip right
        (y >= HEIGHT) || // Clip bottom
        ((x + (4+1) * size - 1) < 0) || // Clip left
        ((y + (5+1) * size - 1) < 0) // Clip top
        )
    return;
    
    unsigned long currentBit;
    if (c % 2)  currentBit = (c-33) * (5 * 4) + 4;
    else        currentBit = (c-32) * (5 * 4);
    for (uint8_t b = 0; b < 5; b++) {
        for (uint8_t a = 0; a < 4; a++) {
          if ( pgm_read_byte(font+(currentBit/8)) & ( 1 << (7-currentBit%8) ) ) {
            if (size == 1) drawPixel(x+a, y+b, color);
            else fillRect(x+a*size, y+b*size, size, size, color);
          }
          else if (bg != color) {
            if (size == 1) drawPixel(x+a, y+b, bg);
            else fillRect(x+a*size, y+b*size, size, size, bg);
          }
          currentBit++;
        }
      currentBit+=4;
    }
#endif
}
void Arduboy::setCursor(int16_t x, int16_t y) {
  cursor_x = x;
  cursor_y = y;
}
void Arduboy::setTextSize(uint8_t s) {
  textsize = (s > 0) ? s : 1;
}
void Arduboy::setTextWrap(boolean w) {
  wrap = w;
}
size_t Arduboy::write(uint8_t c) {
  if (c == '\n') {
    cursor_y += textsize*(FONTHEIGHT+1);
    cursor_x = 0;
  } else if (c == '\r') {
    // skip em
  } else {
    drawChar(cursor_x, cursor_y, c, 1, 0, textsize);
    cursor_x += textsize*(FONTWIDTH+1);
    if (wrap && (cursor_x > (WIDTH - textsize*(FONTWIDTH+1)))) {
      cursor_y += textsize*(FONTHEIGHT+1);
      cursor_x = 0;
    }
  }
}
void Arduboy::display() {
  drawScreen(sBuffer);
}
void Arduboy::drawScreen(const unsigned char *image) {
  for (int a = 0; a < (HEIGHT*WIDTH)/8; a++) { SPI.transfer(pgm_read_byte(image + a)); }
}
void Arduboy::drawScreen(unsigned char image[]) {
  for (int a = 0; a < (HEIGHT*WIDTH)/8; a++) { SPI.transfer(image[a]); }
}
uint8_t Arduboy::width() { return WIDTH; }
uint8_t Arduboy::height() { return HEIGHT; }
uint8_t Arduboy::getInput() {
  // b00lurdab
  uint8_t value = B00000000;
  if (digitalRead(9) == 0) { value = value | B00100000; }	// left
  if (digitalRead(8) == 0) { value = value | B00010000; }	// up
  if (digitalRead(5) == 0) { value = value | B00001000; }	// right
  if (digitalRead(10) == 0) { value = value | B00000100; }	// down
  if (digitalRead(A0) == 0) 	   { value = value | B00000010; }	// a?
  if (digitalRead(A1) == 0)		   { value = value | B00000001; }	// b?
  return value;
}
void Arduboy::swap(int16_t& a, int16_t& b) {
  int temp = a;
  a = b;
  b = temp;
}
void Arduboy::tune_initchan(byte pin) {
  byte timer_num;

  if (_tune_num_chans < AVAILABLE_TIMERS) {
    timer_num = pgm_read_byte(tune_pin_to_timer_PGM + _tune_num_chans);
    _tune_pins[_tune_num_chans] = pin;
    _tune_num_chans++;
    pinMode(pin, OUTPUT);
    switch (timer_num) {
      case 1:
        // 16 bit timer
        TCCR1A = 0;
        TCCR1B = 0;
        bitWrite(TCCR1B, WGM12, 1);
        bitWrite(TCCR1B, CS10, 1);
        timer1_pin_port = portOutputRegister(digitalPinToPort(pin));
        timer1_pin_mask = digitalPinToBitMask(pin);
        break;
      case 3:
        // 16 bit timer
        TCCR3A = 0;
        TCCR3B = 0;
        bitWrite(TCCR3B, WGM32, 1);
        bitWrite(TCCR3B, CS30, 1);
        timer3_pin_port = portOutputRegister(digitalPinToPort(pin));
        timer3_pin_mask = digitalPinToBitMask(pin);
        tune_playnote (0, 60);  /* start and stop channel 0 (timer 3) on middle C so wait/delay works */
        tune_stopnote (0);
        break;
    }
  }
}
void tune_playnote (byte chan, byte note) {
  byte timer_num;
  byte prescalarbits = 0b001;
  unsigned int frequency2; /* frequency times 2 */
  unsigned long ocr;
  if (chan < _tune_num_chans) {
    timer_num = pgm_read_byte(tune_pin_to_timer_PGM + chan);
    if (note>127) note=127;
    frequency2 = pgm_read_word (tune_frequencies2_PGM + note);
    //******  16-bit timer  *********
    // two choices for the 16 bit timers: ck/1 or ck/64
    ocr = F_CPU / frequency2 - 1;
    prescalarbits = 0b001;
    if (ocr > 0xffff) {
      ocr = F_CPU / frequency2 / 64 - 1;
      prescalarbits = 0b011;
    }
    if (timer_num == 1)        TCCR1B = (TCCR1B & 0b11111000) | prescalarbits;
    else if (timer_num == 3)        TCCR3B = (TCCR3B & 0b11111000) | prescalarbits;
    // Set the OCR for the given timer, then turn on the interrupts
    switch (timer_num) {
      case 1:
        OCR1A = ocr;
        bitWrite(TIMSK1, OCIE1A, 1);
        break;
      case 3:
        OCR3A = ocr;
        wait_timer_frequency2 = frequency2;  // for "tune_delay" function
        wait_timer_playing = true;
        bitWrite(TIMSK3, OCIE3A, 1);
        break;
    }
  }
}
void tune_stopnote (byte chan) {
  byte timer_num;
  timer_num = pgm_read_byte(tune_pin_to_timer_PGM + chan);
  switch (timer_num) {
    case 1:
      TIMSK1 &= ~(1 << OCIE1A);                 // disable the interrupt
      *timer1_pin_port &= ~(timer1_pin_mask);   // keep pin low after stop
      break;
    case 3:
      wait_timer_playing = false;
      *timer3_pin_port &= ~(timer3_pin_mask);   // keep pin low after stop
      break;
  }
}
void Arduboy::tune_playscore (const byte *score) {
  score_start = score;
  score_cursor = score;
  tune_stepscore();  /* execute initial commands */
  Arduboy::tune_playing = true;  /* release the interrupt routine */
}
/* Do score commands until a "wait" is found, or the score is stopped.
This is called initially from tune_playcore, but then is called
from the interrupt routine when waits expire.
*/
#define CMD_PLAYNOTE	0x90	/* play a note: low nibble is generator #, note is next byte */
#define CMD_STOPNOTE	0x80	/* stop a note: low nibble is generator # */
#define CMD_RESTART	0xe0	/* restart the score from the beginning */
#define CMD_STOP	0xf0	/* stop playing */
/* if CMD < 0x80, then the other 7 bits and the next byte are a 15-bit big-endian number of msec to wait */
void tune_stepscore (void) {
  byte cmd, opcode, chan;
  unsigned duration;
  while (1) {
    cmd = pgm_read_byte(score_cursor++);
    if (cmd < 0x80) { /* wait count in msec. */
      duration = ((unsigned)cmd << 8) | (pgm_read_byte(score_cursor++));
      wait_toggle_count = ((unsigned long) wait_timer_frequency2 * duration + 500) / 1000;
      if (wait_toggle_count == 0) wait_toggle_count = 1;
      break;
    }
    opcode = cmd & 0xf0;
    chan = cmd & 0x0f;
    if (opcode == CMD_STOPNOTE) { /* stop note */
      tune_stopnote (chan);
    }
    else if (opcode == CMD_PLAYNOTE) { /* play note */
      tune_playnote (chan, pgm_read_byte(score_cursor++));
    }
    else if (opcode == CMD_RESTART) { /* restart score */
      score_cursor = score_start;
    }
    else if (opcode == CMD_STOP) { /* stop score */
      Arduboy::tune_playing = false;
      break;
    }
  }
}
void Arduboy::tune_stopscore (void) {
  int i;
  for (i=0; i<_tune_num_chans; ++i)
    tune_stopnote(i);
  Arduboy::tune_playing = false;
}
void Arduboy::tune_delay (unsigned duration) {
  boolean notdone;
  noInterrupts();
  delay_toggle_count = ((unsigned long) wait_timer_frequency2 * duration + 500) / 1000;
  doing_delay = true;
  interrupts();
  do { // wait until the interrupt routines decrements the toggle count to zero
    noInterrupts();
    notdone = delay_toggle_count != 0;  /* interrupt-safe test */
    interrupts();
  }
  while (notdone);
  doing_delay = false;
}
void Arduboy::tune_stopchans(void) {
  byte chan;
  byte timer_num;
  for (chan=0; chan<_tune_num_chans; ++chan) {
    timer_num = pgm_read_byte(tune_pin_to_timer_PGM + chan);
    switch (timer_num) {
      case 1:
        TIMSK1 &= ~(1 << OCIE1A);
        break;
      case 3:
        TIMSK3 &= ~(1 << OCIE3A);
        break;
    }
    digitalWrite(_tune_pins[chan], 0);
  }
  _tune_num_chans = 0;
}
ISR(TIMER1_COMPA_vect) {  // TIMER 1
  *timer1_pin_port ^= timer1_pin_mask;  // toggle the pin
}
ISR(TIMER3_COMPA_vect) {  // TIMER 3
  // Timer 3 is the one assigned first, so we keep it running always
  // and use it to time score waits, whether or not it is playing a note.
  if (wait_timer_playing) { // toggle the pin if we're sounding a note
    *timer3_pin_port ^= timer3_pin_mask;
  }
  if (Arduboy::tune_playing && wait_toggle_count && --wait_toggle_count == 0) {
    // end of a score wait, so execute more score commands
    wait_timer_old_frequency2 = wait_timer_frequency2;  // save this timer's frequency
    tune_stepscore();  // execute commands
    // If this timer's frequency has changed and we're using it for a tune_delay(),
    // recompute the number of toggles to wait for
    if (doing_delay && wait_timer_old_frequency2 != wait_timer_frequency2) {
      if (delay_toggle_count >= 0x20000UL && wait_timer_frequency2 >= 0x4000U) {
        // Need to avoid 32-bit overflow...
        delay_toggle_count = ( (delay_toggle_count+4>>3) * (wait_timer_frequency2+2>>2) / wait_timer_old_frequency2 )<<5;
      }
      else {
        delay_toggle_count = delay_toggle_count * wait_timer_frequency2 / wait_timer_old_frequency2;
      }
    }
  }
  if (doing_delay && delay_toggle_count) --delay_toggle_count;	// countdown for tune_delay()
}

