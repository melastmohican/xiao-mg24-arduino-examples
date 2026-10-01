// GxEPD2 panel driver class for GDEM037F51 / Waveshare 3.7inch e-Paper (G)
// 3.7" 4-color (240x416), IST7163 controller
// Black, White, Yellow, Red (2 bits per pixel)

#ifndef _GxEPD2_370c_GDEM037F51_H_
#define _GxEPD2_370c_GDEM037F51_H_

#include <GxEPD2_EPD.h>

class GxEPD2_370c_GDEM037F51 : public GxEPD2_EPD
{
  public:
    static const uint16_t WIDTH = 240;
    static const uint16_t WIDTH_VISIBLE = WIDTH;
    static const uint16_t HEIGHT = 416;
    static const GxEPD2::Panel panel = GxEPD2::GDEM0154F51H;
    static const bool hasColor = true;
    static const bool hasPartialUpdate = false;
    static const bool hasFastPartialUpdate = false;
    static const bool useFastFullUpdate = true;
    static const uint16_t power_on_time = 200;
    static const uint16_t power_off_time = 100;
    static const uint16_t full_refresh_time = 25000;
    static const uint16_t partial_refresh_time = 25000;

    GxEPD2_370c_GDEM037F51(int16_t cs, int16_t dc, int16_t rst, int16_t busy);

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
    void _PowerOn();
    void _PowerOff();
    void _InitDisplay();
    void selectFastFullUpdate(bool ff);

  private:
    bool _paged;
    bool _use_fast_update;
};

#endif
