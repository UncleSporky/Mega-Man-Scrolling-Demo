#ifndef Arduboy_h
#define Arduboy_h

#include <SPI.h>
#include <Print.h>


// Pins
#define CS		6
#define DC		4
#define RST		12
#define D_UP    8
#define D_LEFT  9
#define D_DOWN  10
#define D_RIGHT 5

#define WIDTH 	128
#define HEIGHT	64
#define TINYFONT 0        // 0 for 5x7 font (default), 1 for 4x5
#define VERTICALIMAGES 1  // 0 for horizontally oriented images (default)
                          // 1 for faster vertical images

class Arduboy : public Print {
  public:
    Arduboy();
    uint8_t getInput();
    void start();

    // Adafruit functions
    void clearDisplay();
    void display();
    void drawScreen(const unsigned char *image);
    void drawScreen(unsigned char image[]);
    void drawPixel(int x, int y, uint16_t value);
    void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
    void drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color);
    void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
    void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color);
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
    void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t color);
    void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
    void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
    void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
    void fillTriangle (int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
    void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);
    void drawMaskedBitmap(int16_t x, int16_t y, const uint8_t *bitmap, const uint8_t *mask, int16_t w, int16_t h, uint16_t color);
    void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);
    void setCursor(int16_t x, int16_t y);
    void setTextSize(uint8_t s);
    void setTextWrap(boolean w);
    uint8_t width();
    uint8_t height();
    virtual size_t write(uint8_t);

    // Playtune Functions
    void tune_initchan (byte pin);			// assign a timer to an output pin
    void tune_playscore (const byte *score);	// start playing a polyphonic score
    volatile static boolean tune_playing;	// is the score still playing?
    void tune_stopscore (void);			// stop playing the score
    void tune_delay (unsigned msec);		// delay in milliseconds
    void tune_stopchans (void);			// stop all timers

  private:
    unsigned char sBuffer[(HEIGHT*WIDTH)/8];
    uint8_t readCapacitivePin(int pinToMeasure);
    uint8_t readCapXtal(int pinToMeasure);
    volatile uint8_t *mosiport, *clkport, *csport, *dcport;
    uint8_t mosipinmask, clkpinmask, cspinmask, dcpinmask;
    void swap(int16_t& a, int16_t& b);

  protected:
    int16_t  cursor_x, cursor_y;
    uint8_t  textsize;
    boolean  wrap; // If set, 'wrap' text at right edge of display
};

#endif

