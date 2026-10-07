# Seeed Studio XIAO MG24 Sketches and E-Paper Drivers

[![GitHub](https://img.shields.io/badge/GitHub-xiao--mg24--arduino--examples-blue?logo=github)](https://github.com/melastmohican/xiao-mg24-arduino-examples)

Arduino sketches and display drivers for the Seeed Studio XIAO MG24 (Sense), powered by the Silicon Labs EFR32MG24 microcontroller (ARM Cortex-M33, 2.4 GHz multi-protocol radio for BLE 5.4, Matter, and Thread).

This repository contains standalone test sketches, protocol examples, and e-paper display drivers targeting the Seeed Studio ePaper Driver Board for XIAO v2 across multiple display controllers (SSD1680, UC8253, JD79660, IST7163, UC8151D, SSD2681, JD79676A, SSD1685).

## Hardware Setup

- Board: Seeed Studio XIAO MG24 or XIAO MG24 Sense
- Microcontroller: Silicon Labs EFR32MG24
- Display Shield: Seeed Studio ePaper Driver Board for XIAO v2
- Default Pinout:
  - `EPD_RST_PIN`: `D0`
  - `EPD_CS_PIN`: `D1`
  - `EPD_BUSY_PIN`: `D2`
  - `EPD_DC_PIN`: `D3`
  - `EPD_SCK_PIN`: `D8`
  - `EPD_MOSI_PIN`: `D10`

Hardware SPI uses fixed variant pins (D8 for SCK, D10 for MOSI). Serial communication runs over EUSART0 bridged via CMSIS-DAP at 115200 baud.

## Software Prerequisites

- Tool: `arduino-cli` 1.5.1 or later
- Core: `SiliconLabs:silabs` version 4.0.0
- Board FQBN: `SiliconLabs:silabs:xiao_mg24`
- Required Libraries (placed in the Arduino sketchbook `libraries` folder):
  - GxEPD2 (version 1.6.9)
  - Adafruit_GFX
  - Adafruit_EPD

### Protocol Stack Selection

The Silicon Labs Arduino core compiles the radio stack directly into the binary. You must supply the correct `protocol_stack` option during compilation:

- `protocol_stack=none`: Used for Blink, hwinfo, all GxEPD2 demos, all Adafruit_EPD sketches, and vendor drivers.
- `protocol_stack=ble_silabs`: Required for `ble_scan`.
- `protocol_stack=matter`: Required for `matter_lightbulb` and `matter_decommission`.

## Directory Structure and Sketches

### 1. Silicon Labs Core Examples

- `Blink/`: Onboard LED toggle test using core active-low definitions (`LED_BUILTIN_ACTIVE`, `LED_BUILTIN_INACTIVE`).
- `hwinfo/`: Diagnostics reporting board type, radio stack type, internal CPU temperature (`getCPUTemp()`), and heap high watermark (`getHeapHighWatermark()`).
- `ble_scan/`: Event-driven BLE 5.4 scanner processing scan responses via the `sl_bt_on_event()` callback.
- `matter_lightbulb/`: Matter-over-Thread lightbulb accessory demo.
- `matter_decommission/`: Utility sketch that clears non-volatile Matter fabric credentials when re-pairing fails.

### 2. GxEPD2 Display Demos (`GxEPD2/`)

Standardized 6-page demonstration suites (splash, palette, typography, geometry, patterns, dashboard) using paged memory rendering:

- `GxEPD2/GDEM0213B74/Demo/`: 2.13 inch Black and White (122x250, SSD1680)
- `GxEPD2/GDEY0266T90/Demo/`: 2.66 inch Black and White (152x296, SSD1680)
- `GxEPD2/GDEY037T03/Demo/`: 3.7 inch Black and White (280x480)
- `GxEPD2/GDEQ0426T82/Demo/`: 4.26 inch Black and White (800x480, segmented buffer capped at 64 KB)
- `GxEPD2/GDEH0154Z90/Demo/`: 1.54 inch 3-color Red/Black/White (200x200)
- `GxEPD2/GDEY0266Z90/Demo/`: 2.66 inch 3-color Red/Black/White (152x296)
- `GxEPD2/GDEY0213F51/Demo/`: 2.13 inch 4-color Red/Yellow/Black/White (122x250)
- `GxEPD2/GDEM0154F51H/Demo/`: 1.54 inch 4-color Red/Yellow/Black/White (200x200, JD79660)
- `GxEPD2/GDEM037F51/Demo/`: 3.7 inch 4-color Red/Yellow/Black/White (240x416, IST7163). Dashboard text is right-aligned and shortened to fit the 240 px width.
- `GxEPD2/GDEW0215T12/Demo/`: 2.15 inch Black and White (208x112, UC8151D) with custom in-sketch panel driver class for GDEW0215T12 (formerly GDEW0215T11).
- `GxEPD2/GDEM0154F61H/Demo/`: 1.54 inch 4-color Red/Yellow/Black/White (200x200, SSD2681) with custom in-sketch panel driver class.
- `GxEPD2/GDEY0213F52/Demo/`: 2.13 inch 4-color Red/Yellow/Black/White (250x122, JD79676A) with custom in-sketch panel driver class.
- `GxEPD2/GDEY0266T90H/Demo/`: 2.66 inch Black and White (360x184, SSD1685) with custom in-sketch panel driver class (derived from the library's SSD1685 `GDEY029T71H`).

### 3. Adafruit_EPD Driver Implementations (`Adafruit_EPD/`)

- `Adafruit_EPD/XIAO_Waveshare_3in52/`: Waveshare 3.52 inch tricolor (240x360, UC8253) with an in-sketch panel class and custom timing initialization.
- `Adafruit_EPD/XIAO_Waveshare_2in66b/`: Waveshare 2.66 inch tricolor (152x296, SSD1680) using `ThinkInk_266_Tricolor_MFGNR`.
- `Adafruit_EPD/XIAO_Waveshare_2in66/`: Waveshare 2.66 inch monochrome (152x296, SSD1680) supporting 1-bit monochrome and 4-level grayscale via `ThinkInk_266_Grayscale4_MFGN`.
- `Adafruit_EPD/XIAO_Waveshare_1in54g/`: Waveshare 1.54 inch 4-color (200x200, JD79660) subclassing `Adafruit_JD79661`.
- `Adafruit_EPD/XIAO_GDEW0215T12/`: Good Display 2.15 inch monochrome (208x112, UC8151D) with in-sketch panel class and partial refresh, for GDEW0215T12 (formerly GDEW0215T11).
- `Adafruit_EPD/XIAO_GDEM0154F61H/`: Good Display 1.54 inch 4-color (200x200, SSD2681) subclassing `Adafruit_JD79661`.
- `Adafruit_EPD/XIAO_GDEY0213F52/`: Good Display 2.13 inch 4-color (250x122, JD79676A) subclassing `Adafruit_JD79661` with the short Good Display init.
- `Adafruit_EPD/XIAO_GDEY0266T90H/`: Good Display 2.66 inch Black and White (360x184, SSD1685) subclassing `Adafruit_SSD1680` with a mono init table.

### 4. Waveshare Vendored Drivers (`Waveshare_*/`)

Self-contained C drivers adapted from Waveshare reference implementations:

- `Waveshare_3in52/`: Bare 3.52 inch tricolor driver (UC8253) using DEV_Config and GUI_Paint.
- `Waveshare_2in66br/`: 2.66 inch tricolor driver (SSD1680) with 180-degree byte-level raster inversion.
- `Waveshare_2in66/`: 2.66 inch monochrome driver (SSD1680) with 40-second busy timeout guard and 180-degree orientation correction.
- `Waveshare_1in54g/`: 1.54 inch 4-color driver (JD79660) with 2-bit-per-pixel buffer handling (10,000 bytes total).
- `Waveshare_3in7g/`: 3.7 inch 4-color driver (IST7163) with 2-bit-per-pixel buffer handling (24,960 bytes total).

### 5. Good Display Official Samples (`good_display/`)

Low-level vendor examples demonstrating specific controller waveform modes:

- `good_display/GDEY0266Z90/`: 2.66 inch tricolor sample with fast and partial refresh routines.
- `good_display/GDEY0266T90/`: 2.66 inch monochrome sample with standard full refresh (2s), fast refresh (1.0s to 1.5s), and partial refresh clock demo (0.5s).
- `good_display/GDEM0154F51H/`: 1.54 inch 4-color sample testing full refresh (20s) and fast update mode (12s to 15s).
- `good_display/GDEM037F51/`: 3.7 inch 4-color sample testing full refresh (20s) and fast update mode (12s to 15s).
- `good_display/GDEW0215T12/`: 2.15 inch monochrome sample testing full refresh (3s) and partial refresh clock (0.5s), for GDEW0215T12 (formerly GDEW0215T11).
- `good_display/GDEM0154F61H/`: 1.54 inch 4-color sample (SSD2681) testing full refresh (20s) and fast update mode (12s to 15s).
- `good_display/GDEY0213F52/`: 2.13 inch 4-color sample (JD79676A) testing full refresh (11s) and fast update mode.
- `good_display/GDEY0266T90H/`: 2.66 inch B/W 360x184 sample (SSD1685) testing full refresh (2s), fast refresh 1 (1.5s) and 2 (1.0s), and a partial refresh clock.

## Build and Flash Examples

Compile a standard display demo:

```sh
arduino-cli compile -b SiliconLabs:silabs:xiao_mg24:protocol_stack=none GxEPD2/GDEQ0426T82/Demo
```

Compile the BLE scanner:

```sh
arduino-cli compile -b SiliconLabs:silabs:xiao_mg24:protocol_stack=ble_silabs ble_scan
```

Compile a Matter example:

```sh
arduino-cli compile -b SiliconLabs:silabs:xiao_mg24:protocol_stack=matter matter_lightbulb
```

Upload using the Simplicity Commander programmer over USB serial:

```sh
arduino-cli upload -p /dev/cu.usbmodemXXXX -b SiliconLabs:silabs:xiao_mg24:protocol_stack=none -P commander GxEPD2/GDEQ0426T82/Demo
```

Open serial monitor:

```sh
arduino-cli monitor -p /dev/cu.usbmodemXXXX -c baudrate=115200
```

## Hardware and Controller Notes

- BUSY Pin Polarity: The SSD1680 controller asserts BUSY high when updating (1 = busy, 0 = idle). The UC8253, JD79660, IST7163, UC8151D, and SSD2681 controllers assert BUSY low (0 = busy, 1 = idle).
- Panel Orientation: 2.66 inch panels (SSD1680) have the FPC connector positioned so that standard coordinate zero sits at the opposite corner. Each driver applies a 180-degree rotation fix (`DEMO_ROTATION 3` in GxEPD2, `setRotation(2)` in Adafruit_EPD, `EPD_INIT_180` in Good Display, and raster reversal in Waveshare C drivers).
- 2.15" GDEW0215T12 (UC8151D): Resolution is 208x112 monochrome (14 bytes per gate row across 208 gates = 2,912 bytes total). In Adafruit_EPD, rotation 0 provides the native landscape orientation, and partial refresh uses the 0.5s differential waveform LUT across the full 2,912-byte framebuffer without full-screen flicker.
- 1.54" GDEM0154F61H (SSD2681): Resolution is 200x200 with 4 native colors (Black, White, Yellow, Red) at 2 bits per pixel (50 bytes per row, 10,000 bytes total). Full refresh takes ~20s and fast refresh takes ~12-15s. Supported across Good Display vendor code, in-sketch GxEPD2 panel driver (`GxEPD2_154c_GDEM0154F61H`) with `<GxEPD2_4C.h>`, and an in-sketch Adafruit_EPD subclass (`ThinkInk_154_Quadcolor_GDEM0154F61H`) deriving from `Adafruit_JD79661`.
- 2.13" GDEY0213F52 (JD79676A): Resolution is 250x122 (128x250 native RAM, 122 visible) with 4 native colors (Black, White, Yellow, Red) at 2 bits per pixel (32 bytes per row, 8,000 bytes total). Full refresh takes ~11s. Supported across Good Display vendor code, in-sketch GxEPD2 panel driver (`GxEPD2_213c_GDEY0213F52`) with `<GxEPD2_4C.h>`, and an in-sketch Adafruit_EPD subclass (`ThinkInk_213_Quadcolor_GDEY0213F52`) deriving from `Adafruit_JD79661`. Stock GxEPD2 has no F52 class. The GxEPD2 class needs a 40 s BUSY timeout because a refresh takes ~11s. Orientation (`DEMO_ROTATION 1`) is not yet checked on hardware.
- 2.66" GDEY0266T90H (SSD1685): Resolution is 360x184 (184x360 native RAM, 23 bytes per row, 8,280 bytes total), black/white, 1 bit per pixel. Full refresh ~2s, fast ~1-1.5s, partial ~0.4s. Supported across Good Display vendor code, in-sketch GxEPD2 panel driver (`GxEPD2_266_GDEY0266T90H`) with `<GxEPD2_BW.h>`, and an in-sketch Adafruit_EPD subclass (`ThinkInk_266_Mono_GDEY0266T90H`) deriving from `Adafruit_SSD1680`. Stock GxEPD2 and Adafruit_EPD have no GDEY0266T90H support. Orientation differs from the 152x296 2.66" panels: GxEPD2 uses `DEMO_ROTATION 1` and Adafruit_EPD `setRotation(2)` (both checked on hardware). `GxEPD2_266_GDEY0266T90H` uses data entry `0x03`, not Good Display's `0x01`, and the `0x21` source resolution must be `01` (184), not the power-on `00` (200), or a noise strip appears.
- SPI Mutex Guard: In the Silicon Labs Arduino core, `SilabsSPI::beginTransaction()` acquires a FreeRTOS mutex. Re-calling `beginTransaction()` without `endTransaction()` can halt execution.
- Tricolor Refresh Times: Full refresh on tricolor panels takes roughly 20 seconds because of the physical electrophoretic requirements of red pigment particles. Fast and partial updates on tricolor panels still require full-screen cycling.
