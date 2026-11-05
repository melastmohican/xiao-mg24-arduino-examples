// GxEPD2 panel driver class for Good Display GDEY029F52
// 2.9" 4-color (128x296 native, shown as 296x128 landscape with rotation 1), JD79661 controller
// No stock GxEPD2 class exists for this panel. The pipeline (2 bits per pixel,
// 00=Black 01=White 10=Yellow 11=Red, BUSY active LOW, 0x10 RAM, 0x12 refresh) is the
// same as GxEPD2_213c_GDEY0213F52 in this repo; the init sequences are Good Display's
// AU-GDEY029F52-F-20250609 sample.
// Black, White, Yellow, Red (2 bits per pixel: 00=Black, 01=White, 10=Yellow, 11=Red)

#ifndef _GxEPD2_290c_GDEY029F52_H_
#define _GxEPD2_290c_GDEY029F52_H_

#include <GxEPD2_EPD.h>

class GxEPD2_290c_GDEY029F52 : public GxEPD2_EPD
{
  public:
    static const uint16_t WIDTH = 128;      // 128 source pixels, already a byte multiple
    static const uint16_t WIDTH_VISIBLE = WIDTH;
    static const uint16_t HEIGHT = 296;
    // GxEPD2 has no enum value for this panel; GDEY029F51H is the same size and color mode.
    static const GxEPD2::Panel panel = GxEPD2::GDEY029F51H;
    static const bool hasColor = true;
    static const bool hasPartialUpdate = false;
    static const bool hasFastPartialUpdate = false;
    static const bool useFastFullUpdate = false;
    static const uint16_t power_on_time = 200;
    static const uint16_t power_off_time = 100;
    static const uint16_t full_refresh_time = 22000;
    static const uint16_t partial_refresh_time = 22000;

    GxEPD2_290c_GDEY029F52(int16_t cs, int16_t dc, int16_t rst, int16_t busy);

    void selectFastFullUpdate(bool ff);

    void clearScreen(uint8_t value = 0xFF);
    void clearScreen(uint8_t black_value, uint8_t color_value);
    void writeScreenBuffer(uint8_t value = 0xFF);
    void writeScreenBuffer(uint8_t black_value, uint8_t color_value);

    void writeImage(const uint8_t bitmap[], int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false, bool pgm = false);
    void writeImagePart(const uint8_t bitmap[], int16_t x_part, int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
                        int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false, bool pgm = false);
    void writeImage(const uint8_t* black, const uint8_t* color, int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false, bool pgm = false);
    void writeImagePart(const uint8_t* black, const uint8_t* color, int16_t x_part, int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
                        int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false, bool pgm = false);

    void writeNative(const uint8_t* data1, const uint8_t* data2, int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false, bool pgm = false);
    void writeNativePart(const uint8_t* data1, const uint8_t* data2, int16_t x_part, int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
                         int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false, bool pgm = false);

    void drawImage(const uint8_t bitmap[], int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false, bool pgm = false);
    void drawImagePart(const uint8_t bitmap[], int16_t x_part, int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
                       int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false, bool pgm = false);
    void drawImage(const uint8_t* black, const uint8_t* color, int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false, bool pgm = false);
    void drawImagePart(const uint8_t* black, const uint8_t* color, int16_t x_part, int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
                       int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false, bool pgm = false);

    void drawNative(const uint8_t* data1, const uint8_t* data2, int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false, bool pgm = false);

    void refresh(bool partial_update_mode = false);
    void refresh(int16_t x, int16_t y, int16_t w, int16_t h);
    void powerOff();
    void hibernate();
    void setPaged();

  private:
    void _refresh(bool partial_update_mode);
    void _InitDisplay();
    void _PowerOn();
    void _PowerOff();
    bool _paged;
    bool _use_fast_update;
};

#endif
