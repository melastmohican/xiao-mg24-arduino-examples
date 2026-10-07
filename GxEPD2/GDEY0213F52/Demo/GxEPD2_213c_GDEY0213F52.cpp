#include "GxEPD2_213c_GDEY0213F52.h"

GxEPD2_213c_GDEY0213F52::GxEPD2_213c_GDEY0213F52(int16_t cs, int16_t dc, int16_t rst, int16_t busy) :
  GxEPD2_EPD(cs, dc, rst, busy, LOW, 40000000, WIDTH, HEIGHT, panel, hasColor, hasPartialUpdate, hasFastPartialUpdate)
{
  _paged = false;
  _use_fast_update = useFastFullUpdate;
}

void GxEPD2_213c_GDEY0213F52::selectFastFullUpdate(bool ff)
{
  if (ff != _use_fast_update)
  {
    _use_fast_update = ff;
    _InitDisplay();
  }
}

void GxEPD2_213c_GDEY0213F52::clearScreen(uint8_t value)
{
  clearScreen(value, 0xFF);
}

void GxEPD2_213c_GDEY0213F52::clearScreen(uint8_t black_value, uint8_t color_value)
{
  writeScreenBuffer(black_value, color_value);
  refresh();
}

void GxEPD2_213c_GDEY0213F52::writeScreenBuffer(uint8_t value)
{
  writeScreenBuffer(value, 0xFF);
}

void GxEPD2_213c_GDEY0213F52::writeScreenBuffer(uint8_t black_value, uint8_t color_value)
{
  if (!_init_display_done) _InitDisplay();
  _writeCommand(0x10);
  _startTransfer();
  for (uint32_t i = 0; i < uint32_t(WIDTH) * uint32_t(HEIGHT) / 4; i++)
  {
    _transfer(0xFF == black_value ? 0x55 : 0x00);
  }
  _endTransfer();
  _initial_write = false;
}

void GxEPD2_213c_GDEY0213F52::writeImage(const uint8_t bitmap[], int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm)
{
  delay(1);
  if (!_init_display_done) _InitDisplay();
  int16_t wb = (w + 7) / 8;
  x -= x % 8;
  w = wb * 8;
  if ((w <= 0) || (h <= 0)) return;
  _writeCommand(0x10);
  _startTransfer();
  for (int16_t i = 0; i < int16_t(HEIGHT); i++)
  {
    for (int16_t j = 0; j < int16_t(WIDTH); j += 8)
    {
      uint8_t data = 0xFF;
      if ((j >= x) && (j <= x + w) && (i >= y) && (i < y + h))
      {
        uint32_t idx = mirror_y ? (j - x) / 8 + uint32_t((h - 1 - (i - y))) * wb : (j - x) / 8 + uint32_t(i - y) * wb;
        data = bitmap[idx];
        if (invert) data = ~data;
      }
      for (int16_t k = 0; k < 2; k++)
      {
        uint8_t data2 = (data & 0x80 ? 0x40 : 0x00) | (data & 0x40 ? 0x10 : 0x00) |
                        (data & 0x20 ? 0x04 : 0x00) | (data & 0x10 ? 0x01 : 0x00);
        data <<= 4;
        _transfer(data2);
      }
    }
  }
  _endTransfer();
  delay(1);
}

void GxEPD2_213c_GDEY0213F52::writeImagePart(const uint8_t bitmap[], int16_t x_part, int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
                                             int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm)
{
  writeImage(bitmap, x, y, w, h, invert, mirror_y, pgm);
}

void GxEPD2_213c_GDEY0213F52::writeImage(const uint8_t* black, const uint8_t* color, int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm)
{
  if (!black && !color) return;
  if (!color) return writeImage(black, x, y, w, h, invert, mirror_y, pgm);
  delay(1);
  if (!_init_display_done) _InitDisplay();
  int16_t wb = (w + 7) / 8;
  x -= x % 8;
  w = wb * 8;
  if ((w <= 0) || (h <= 0)) return;
  _writeCommand(0x10);
  _startTransfer();
  for (int16_t i = 0; i < int16_t(HEIGHT); i++)
  {
    for (int16_t j = 0; j < int16_t(WIDTH); j += 8)
    {
      uint8_t b_data = 0xFF;
      uint8_t c_data = 0xFF;
      if ((j >= x) && (j <= x + w) && (i >= y) && (i < y + h))
      {
        uint32_t idx = mirror_y ? (j - x) / 8 + uint32_t((h - 1 - (i - y))) * wb : (j - x) / 8 + uint32_t(i - y) * wb;
        if (black) b_data = black[idx];
        if (color) c_data = color[idx];
        if (invert) { b_data = ~b_data; c_data = ~c_data; }
      }
      for (int16_t k = 0; k < 2; k++)
      {
        uint8_t out = 0x55;
        for (int b = 0; b < 4; b++) {
          bool is_black = !(b_data & 0x80);
          bool is_color = !(c_data & 0x80);
          uint8_t color_val = 0x01; // white
          if (is_black) color_val = 0x00; // black
          else if (is_color) color_val = 0x03; // red
          out = (out << 2) | color_val;
          b_data <<= 1;
          c_data <<= 1;
        }
        _transfer(out);
      }
    }
  }
  _endTransfer();
  delay(1);
}

void GxEPD2_213c_GDEY0213F52::writeImagePart(const uint8_t* black, const uint8_t* color, int16_t x_part, int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
                                             int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm)
{
  writeImage(black, color, x, y, w, h, invert, mirror_y, pgm);
}

void GxEPD2_213c_GDEY0213F52::writeNative(const uint8_t* data1, const uint8_t* data2, int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm)
{
  if (!data1) return;
  delay(1);
  if (!_init_display_done) _InitDisplay();

  int16_t wb = (w + 3) / 4;
  x -= x % 4;
  w = wb * 4;
  int16_t x1 = x < 0 ? 0 : x;
  int16_t y1 = y < 0 ? 0 : y;
  int16_t w1 = x + w < int16_t(WIDTH) ? w : int16_t(WIDTH) - x;
  int16_t h1 = y + h < int16_t(HEIGHT) ? h : int16_t(HEIGHT) - y;
  int16_t dx = x1 - x;
  int16_t dy = y1 - y;
  w1 -= dx;
  h1 -= dy;
  if ((w1 <= 0) || (h1 <= 0)) return;

  _writeCommand(0x10);
  _startTransfer();
  for (int16_t i = 0; i < h1; i++)
  {
    for (int16_t j = 0; j < w1 / 4; j++)
    {
      uint32_t idx = mirror_y ? j + dx / 4 + uint32_t((h - 1 - (i + dy))) * wb : j + dx / 4 + uint32_t(i + dy) * wb;
      uint8_t data = data1[idx];
      _transfer(data);
    }
  }
  _endTransfer();
  _initial_write = false;
  delay(1);
}

void GxEPD2_213c_GDEY0213F52::writeNativePart(const uint8_t* data1, const uint8_t* data2, int16_t x_part, int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
                                              int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm)
{
  writeNative(data1, data2, x, y, w, h, invert, mirror_y, pgm);
}

void GxEPD2_213c_GDEY0213F52::drawImage(const uint8_t bitmap[], int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm)
{
  writeImage(bitmap, x, y, w, h, invert, mirror_y, pgm);
  refresh();
}

void GxEPD2_213c_GDEY0213F52::drawImagePart(const uint8_t bitmap[], int16_t x_part, int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
                                            int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm)
{
  writeImagePart(bitmap, x_part, y_part, w_bitmap, h_bitmap, x, y, w, h, invert, mirror_y, pgm);
  refresh();
}

void GxEPD2_213c_GDEY0213F52::drawImage(const uint8_t* black, const uint8_t* color, int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm)
{
  writeImage(black, color, x, y, w, h, invert, mirror_y, pgm);
  refresh();
}

void GxEPD2_213c_GDEY0213F52::drawImagePart(const uint8_t* black, const uint8_t* color, int16_t x_part, int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
                                            int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm)
{
  writeImagePart(black, color, x_part, y_part, w_bitmap, h_bitmap, x, y, w, h, invert, mirror_y, pgm);
  refresh();
}

void GxEPD2_213c_GDEY0213F52::drawNative(const uint8_t* data1, const uint8_t* data2, int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm)
{
  writeNative(data1, data2, x, y, w, h, invert, mirror_y, pgm);
  refresh();
}

void GxEPD2_213c_GDEY0213F52::refresh(bool partial_update_mode)
{
  _refresh(partial_update_mode);
}

void GxEPD2_213c_GDEY0213F52::refresh(int16_t x, int16_t y, int16_t w, int16_t h)
{
  _refresh(false);
}

void GxEPD2_213c_GDEY0213F52::_refresh(bool partial_update_mode)
{
  _writeCommand(0x12);
  _writeData(0x00);
  delay(1);
  _waitWhileBusy("_refresh", full_refresh_time);
  _init_display_done = false;
}

void GxEPD2_213c_GDEY0213F52::_PowerOn()
{
  if (!_power_is_on)
  {
    _writeCommand(0x04);
    _waitWhileBusy("_PowerOn", power_on_time);
  }
  _power_is_on = true;
}

void GxEPD2_213c_GDEY0213F52::_PowerOff()
{
  if (_power_is_on)
  {
    _writeCommand(0x02);
    _writeData(0x00);
    _waitWhileBusy("_PowerOff", power_off_time);
  }
  _power_is_on = false;
}

void GxEPD2_213c_GDEY0213F52::powerOff()
{
  _PowerOff();
}

void GxEPD2_213c_GDEY0213F52::hibernate()
{
  _PowerOff();
  if (_rst >= 0)
  {
    _writeCommand(0x07);
    _writeData(0xA5);
    _hibernating = true;
    _init_display_done = false;
  }
}

void GxEPD2_213c_GDEY0213F52::setPaged()
{
  _paged = true;
}

void GxEPD2_213c_GDEY0213F52::_InitDisplay()
{
  if (_rst >= 0)
  {
    digitalWrite(_rst, HIGH);
    delay(20);
    digitalWrite(_rst, LOW);
    delay(40);
    digitalWrite(_rst, HIGH);
    delay(50);
    _waitWhileBusy("_InitDisplay reset", power_on_time);
    _hibernating = false;
    _power_is_on = false;
  }

  if (_use_fast_update)
  {
    // Good Display fast-update init: E0/E6 set the temperature shortcut, A5 latches it
    _writeCommand(0xE0);
    _writeData(0x02);

    _writeCommand(0xE6);
    _writeData(90);

    _writeCommand(0xA5);
    _waitWhileBusy("_InitDisplay (0xA5)", power_on_time);
  }

  _writeCommand(0xE9);
  _writeData(0x01);

  _writeCommand(0x04); // Power on
  _waitWhileBusy("_InitDisplay (0x04)", power_on_time);
  _power_is_on = true;
  _init_display_done = true;
}
