Good Display GDEY037T03-FP4G 4-gray sample for Seeed Studio XIAO MG24
Seated in Seeed Studio ePaper Driver Board for XIAO v2 (24-pin connector)

Pinout:
  RST  -> D0
  CS   -> D1
  BUSY -> D2 (Active LOW on UC8253)
  DC   -> D3
  SCK  -> D8
  MOSI -> D10

Panel: GDEY037T03 (3.7" Black/White, 240x416, UC8253)

Purpose: check, independently of epdsi, whether the glass renders four gray levels.
Source: Good Display ESP32 sample for GDEY037T03,
  https://v4.cecdn.yun300.cn/100001_1909185148/A32-GDEY037T03.rar
  (linked from https://www.good-display.com/product/437.html).
  Inner folder name: A32-GDEY037T03-FP4G-20240815. "FP4G" is a folder tag, not a part number.
Only the SPI/pin files and the .ino are changed from Good Display's sample.

Result on the Adafruit 6395 (ZJY240416-037BAAMFGN), 15 Oct 2026: the 4-gray image renders
as plain black and white. Good Display's GDEY037T03 spec (25.01.08) lists "2 Grey Level".
The demo only gets gray if the glass's OTP holds a gray waveform.
