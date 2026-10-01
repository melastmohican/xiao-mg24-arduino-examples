# Seeed Studio XIAO MG24 Sketches and E-Paper Drivers

[![GitHub](https://img.shields.io/badge/GitHub-xiao--mg24--arduino--examples-blue?logo=github)](https://github.com/melastmohican/xiao-mg24-arduino-examples)

Arduino sketches and display drivers for the Seeed Studio XIAO MG24 (Sense), powered by the Silicon Labs EFR32MG24 microcontroller (ARM Cortex-M33, 2.4 GHz multi-protocol radio for BLE 5.4, Matter, and Thread).

This repository contains standalone test sketches, protocol examples, and e-paper display drivers targeting the Seeed Studio ePaper Driver Board for XIAO v2 across multiple display controllers (SSD1680, UC8253, JD79660).

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
- `GxEPD2/GDEM037F51/Demo/`: 3.7 inch 4-color Red/Yellow/Black/White (240x416, IST7163)

### 3. Adafruit_EPD Driver Implementations (`Adafruit_EPD/`)

- `Adafruit_EPD/XIAO_Waveshare_3in52/`: Waveshare 3.52 inch tricolor (240x360, UC8253) with an in-sketch panel class and custom timing initialization.
- `Adafruit_EPD/XIAO_Waveshare_2in66b/`: Waveshare 2.66 inch tricolor (152x296, SSD1680) using `ThinkInk_266_Tricolor_MFGNR`.
- `Adafruit_EPD/XIAO_Waveshare_2in66/`: Waveshare 2.66 inch monochrome (152x296, SSD1680) supporting 1-bit monochrome and 4-level grayscale via `ThinkInk_266_Grayscale4_MFGN`.
- `Adafruit_EPD/XIAO_Waveshare_1in54g/`: Waveshare 1.54 inch 4-color (200x200, JD79660) subclassing `Adafruit_JD79661`.

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

- BUSY Pin Polarity: The SSD1680 controller asserts BUSY high when updating (1 = busy, 0 = idle). The UC8253 and JD79660 controllers assert BUSY low (0 = busy, 1 = idle).
- Panel Orientation: 2.66 inch panels (SSD1680) have the FPC connector positioned so that standard coordinate zero sits at the opposite corner. Each driver applies a 180-degree rotation fix (`DEMO_ROTATION 3` in GxEPD2, `setRotation(2)` in Adafruit_EPD, `EPD_INIT_180` in Good Display, and raster reversal in Waveshare C drivers).
- SPI Mutex Guard: In the Silicon Labs Arduino core, `SilabsSPI::beginTransaction()` acquires a FreeRTOS mutex. Re-calling `beginTransaction()` without `endTransaction()` can halt execution.
- Tricolor Refresh Times: Full refresh on tricolor panels takes roughly 20 seconds because of the physical electrophoretic requirements of red pigment particles. Fast and partial updates on tricolor panels still require full-screen cycling.
