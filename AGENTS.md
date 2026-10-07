# AGENTS.md

This file provides guidance for AI coding agents (Claude Code, Antigravity, Codex) when working with code in this repository.

## What this is

Arduino sketches for the **Seeed Studio XIAO MG24 (Sense)** — a Silicon Labs
EFR32MG24 board. Every sketch here is a standalone `.ino`; there is no shared
build system, no tests, and no library sources in-tree.

Repository setup: This directory is a standalone Git repository
([xiao-mg24-arduino-examples](https://github.com/melastmohican/xiao-mg24-arduino-examples)).
Libraries resolve from the local Arduino sketchbook `libraries` folder (GxEPD2 1.6.9, Adafruit GFX).
Cross-repository references: Only reference other repositories if they exist under the `melastmohican` GitHub account (`gh:melastmohican`). Verify existence with `gh repo view melastmohican/<repo>` before citing or linking.

## Build / upload

`arduino-cli` 1.5.1 with the `SiliconLabs:silabs` 4.0.0 core. The FQBN is
`SiliconLabs:silabs:xiao_mg24`, and **the `protocol_stack` menu option is not
optional** — picking the wrong one is the most common failure mode, because the
radio stack is compiled into the core, not linked from the sketch:

| Sketch | Required option |
| --- | --- |
| `ble_scan/` | `protocol_stack=ble_silabs` (has an `#error` guard) |
| `matter_lightbulb/`, `matter_decommission/` | `protocol_stack=matter` |
| `Blink/`, `hwinfo/`, `GxEPD2/*/Demo/` | `protocol_stack=none` |
| `Waveshare_3in52/`, `Adafruit_EPD/XIAO_Waveshare_3in52/` | `protocol_stack=none` |
| `Waveshare_2in66br/`, `Adafruit_EPD/XIAO_Waveshare_2in66b/` | `protocol_stack=none` |
| `good_display/GDEY0266Z90/` | `protocol_stack=none` |
| `Waveshare_2in66/`, `Adafruit_EPD/XIAO_Waveshare_2in66/` | `protocol_stack=none` |
| `good_display/GDEY0266T90/` | `protocol_stack=none` |
| `Waveshare_1in54g/`, `Adafruit_EPD/XIAO_Waveshare_1in54g/` | `protocol_stack=none` |
| `good_display/GDEM0154F51H/` | `protocol_stack=none` |
| `Waveshare_3in7g/`, `good_display/GDEM037F51/`, `GxEPD2/GDEM037F51/Demo/` | `protocol_stack=none` |
| `Adafruit_EPD/XIAO_GDEW0215T12/`, `GxEPD2/GDEW0215T12/Demo/`, `good_display/GDEW0215T12/` | `protocol_stack=none` |
| `good_display/GDEM0154F61H/`, `GxEPD2/GDEM0154F61H/Demo/`, `Adafruit_EPD/XIAO_GDEM0154F61H/` | `protocol_stack=none` |
| `good_display/GDEY0213F52/`, `GxEPD2/GDEY0213F52/Demo/`, `Adafruit_EPD/XIAO_GDEY0213F52/` | `protocol_stack=none` |
| `good_display/GDEY0266T90H/`, `GxEPD2/GDEY0266T90H/Demo/`, `Adafruit_EPD/XIAO_GDEY0266T90H/` | `protocol_stack=none` |

```sh
# from repository root
arduino-cli compile -b SiliconLabs:silabs:xiao_mg24:protocol_stack=none GxEPD2/GDEQ0426T82/Demo
arduino-cli compile -b SiliconLabs:silabs:xiao_mg24:protocol_stack=ble_silabs ble_scan
```

Non-Matter sketches compile in a few seconds; Matter builds are far slower.

Upload defaults to `openocd` over SWD/CMSIS-DAP, i.e. it expects a debug probe
(e.g. XIAO Debug Mate). To flash over the USB serial port instead, use the
Simplicity Commander programmer:

```sh
arduino-cli upload -p /dev/cu.usbmodemXXXX -b SiliconLabs:silabs:xiao_mg24:protocol_stack=none -P commander GxEPD2/GDEQ0426T82/Demo
arduino-cli monitor -p /dev/cu.usbmodemXXXX -c baudrate=115200
```

All sketches use `Serial.begin(115200)`.

## Four sketch families

**Silicon Labs core examples** (`Blink/`, `hwinfo/`, `ble_scan/`,
`matter_lightbulb/`) are mostly upstream examples by Tamas Jozsi, kept close to
original. They lean on core-specific APIs that do not exist on other Arduino
cores: `getCurrentBoardType()` / `silabs_board_t`, `getCurrentRadioStackType()`,
`getCPUTemp()`, `getHeapHighWatermark()`, and the active-low LED constants
`LED_BUILTIN_ACTIVE` / `LED_BUILTIN_INACTIVE` (use these, not `HIGH`/`LOW`).

`ble_scan` is event-driven: there is no polling in `loop()`, all work happens in
the `sl_bt_on_event()` callback that the Silabs BLE stack invokes.

`matter_decommission/` is a utility, not a demo — flash it to clear stale Matter
commissioning credentials when re-pairing fails, then re-flash the real sketch.

**GxEPD2 ePaper demos** (`GxEPD2/<PANEL>/Demo/Demo.ino`) target the *Seeed Studio
ePaper Driver Board for XIAO v2*. Eleven panels: `GDEM0213B74` (2.13" BW),
`GDEY0266T90` (2.66" BW 152x296), `GDEY037T03` (3.7" BW), `GDEQ0426T82` (4.26" BW 800x480),
`GDEH0154Z90` (1.54" 3-color), `GDEY0266Z90` (2.66" 3-color 152x296), `GDEY0213F51` (2.13" 4-color),
`GDEM0154F51H` (1.54" 4-color 200x200), `GDEM037F51` (3.7" 4-color 240x416),
`GDEW0215T12` (2.15" BW 208x112), `GDEM0154F61H` (1.54" 4-color 200x200), `GDEY0213F52` (2.13" 4-color 250x122), and `GDEY0266T90H` (2.66" BW 360x184).

## The GxEPD2 demo template

The seven demos are deliberate near-copies of one template. When editing one,
check whether the change belongs in all seven.

Fixed across every panel — the driver board wiring, do not change per-panel:

```c
#define EPD_RST_PIN D0 / EPD_CS_PIN D1 / EPD_BUSY_PIN D2
#define EPD_DC_PIN  D3 / EPD_SCK_PIN D8 / EPD_MOSI_PIN D10
```

What actually varies per panel is the display declaration — the color depth
picks the header and template class, which then dictates which color constants
the drawing code may use:

- BW → `<GxEPD2_BW.h>` + `GxEPD2_BW<...>`
- 3-color → `<GxEPD2_3C.h>` + `GxEPD2_3C<...>` (adds `GxEPD_RED`)
- 4-color → `<GxEPD2_4C.h>` + `GxEPD2_4C<...>` (adds `GxEPD_RED`, `GxEPD_YELLOW`)

`GDEQ0426T82` is the exception: 800x480 will not fit in RAM, so it caps the
buffer with a `MAX_HEIGHT(EPD)` macro against `MAX_DISPLAY_BUFFER_SIZE 65536u`
and relies on multi-page rendering. The others pass `EPD::HEIGHT` directly.

Structure: all work happens in `setup()` — six screens (`showSplashScreen()`,
palette, typography, geometry, patterns, `showDashboard()`) each drawn, held for
`PAGE_HOLD_MS`, then `display.hibernate()`; `loop()` is empty. Every screen
follows the GxEPD2 paged idiom, and the whole body must be inside the do/while
because it may run more than once:

```c
display.setRotation(1);
display.setFullWindow();
display.firstPage();
do { /* draw everything */ } while (display.nextPage());
```

Text is centered via the shared `drawCenteredText(text, y, font)` helper
(`getTextBounds` + `setCursor`), and BW demos fake gray with
`fillDitheredRect()`. Screen progress is echoed over Serial with `F()` strings.

## The Waveshare 3.52" (B) sketches

Two sketches drive one panel that neither display library covers: a bare
Waveshare 3.52" e-Paper (B) (FPC `SE0352N01FPC-A`, 240x360 BWR, UC8253
controller), seated in the same Seeed driver board as the GxEPD2 demos and using
the same six pins. **GxEPD2 1.6.9 has no 240x360 panel class at all** — only
`GxEPD2_371` is 240 wide — which is why neither sketch is a `GxEPD2/` demo.

- `Waveshare_3in52/` — Waveshare's vendored driver (`DEV_Config`, `EPD_3in52b`,
  `GUI_Paint`, `font*`, `ImageData`), kept close to their `Arduino_R4` original.
  Only `DEV_Config.h/.cpp` and the `.ino` are board-specific; everything else is
  plain portable C. All work happens in `setup()`.
- `Adafruit_EPD/XIAO_Waveshare_3in52/` — the same panel through Adafruit_EPD's
  `Adafruit_UC8253`. The `ThinkInk_352_Tricolor_Waveshare` panel class lives
  *inside the sketch* so the library stays stock; its three non-obvious
  parameters are load-bearing and documented inline: the constructor takes
  `(360, 240, ...)` **long axis first**, `setBlackBuffer(0, false)` because this
  panel wants the black plane uninverted, and the init code needs an explicit
  `0x61` TRES entry that Adafruit's own UC8253 panel does not.

Both were ported from [adafruit-feather-thinkink-examples](https://github.com/melastmohican/adafruit-feather-thinkink-examples). Three Silicon Labs core
facts shaped the port and are worth knowing before touching either:

- `SPI` (not `SPI1`) is the EPD bus, and no remap is needed or possible — the
  variant already puts SCK on `D8` and MOSI on `D10`, and there is no
  `setSCK()`/`setTX()`.
- `SilabsSPI::beginTransaction()` takes a FreeRTOS mutex with `portMAX_DELAY`
  and returns *still holding it* when the settings are unchanged. A second call
  without an intervening `endTransaction()` deadlocks. `DEV_Module_Init()` opens
  the one transaction the sketch uses and never closes it.
- `Serial` is EUSART0 bridged through the on-board CMSIS-DAP chip, not USB CDC,
  so `while (!Serial)` must always be bounded. `Serial.printf()` does exist
  (`UARTClass::printf`) but has a 128-byte buffer.

Hardware note: a cleared BWR panel looks grey next to a mono one. That is the
panel's white point (~20-25% reflectance), not a fault.

## The Waveshare 2.66" (B) / GDEY0266Z90 sketches

One panel, **four** independent driver paths — the widest coverage in this repo.
The Waveshare 2.66" e-Paper (B) is 152x296 BWR on an **SSD1680Z8**; it is
register-compatible with Good Display `GDEY0266Z90`, and GxEPD2 says so itself in
`GxEPD2.h` (`GDEY0266Z90, Waveshare_2_66_bwr = GDEY0266Z90`). The physical panel
on hand is silkscreened `DEPG0266RWS800F34HP`, a DKE part — treat "= GDEY0266Z90"
as a claim about registers and waveforms, not provenance. Same driver board, same
six pins as everything else here.

- `Waveshare_2in66br/` — Waveshare's vendored driver, but their **older** Arduino
  layout (`epdif`, `epdpaint`, `imagedata`, an `Epd` class), not the
  `DEV_Config`/`GUI_Paint` layout `Waveshare_3in52/` uses. Deliberately left that
  way. Only `epdif.h/.cpp` is board-specific.
- `Adafruit_EPD/XIAO_Waveshare_2in66b/` — stock `ThinkInk_266_Tricolor_MFGNR`, no
  in-sketch panel class needed (unlike the 3.52"): MFGNR is Adafruit's SSD1680**Z**
  class and already passes `(296, 152)` long axis first. Just a pin block.
- `GxEPD2/GDEY0266Z90/Demo/` — stock `GxEPD2_266c`, the standard six-screen demo.
- `good_display/GDEY0266Z90/` — Good Display's own sample (`Display_EPD_W21*`,
  `Ap_29demo.h`). The only one of the four exposing fast-update and partial-update
  modes, and the only sketch in this repo whose demo runs in `loop()` rather than
  `setup()` — it ends in an unconditional `while(1)`, because the original guards
  that halt with `#ifdef Arduino_UNO` and would otherwise repeat forever. Board
  specifics live in `Display_EPD_W21_spi.h`, the analogue of `DEV_Config.h`.

**All four need a 180 degree correction**, and each library needs a different
mechanism — the panel's origin is the corner opposite where content should start.
This is a panel/FPC fact, not a board fact: the values carried over unchanged from
the Feather ThinkInk versions.

| Sketch | Knob |
| --- | --- |
| `Waveshare_2in66br/` | `EPD_2IN66B_ROTATE_180` in `epd2in66b.cpp` — no rotation layer exists, so the raster is walked backwards with each byte's bits mirrored (exact: 152 px = 19 bytes, no sub-byte shift) |
| `Adafruit_EPD/XIAO_Waveshare_2in66b/` | `setRotation(2)`, and it must come *after* `begin()` |
| `GxEPD2/GDEY0266Z90/Demo/` | `DEMO_ROTATION 3` instead of the usual landscape 1 |
| `good_display/GDEY0266Z90/` | `EPD_INIT_180` → Good Display's ready-made `EPD_HW_Init_180()` |

Two SSD1680 facts that make the 3.52" UC8253 sketches a bad template:

- **BUSY is active HIGH** here; it is active LOW on the 3.52".
- **No `POWER_OFF` before deep sleep.** SSD1680 deep-sleeps straight from
  `0x10`/`0x01`; the `0x02` in the 3.52" driver is a UC8253 command with no
  equivalent.

Timing: **~20 s for a full refresh, in every mode.** Good Display's "fast" update
is ~19 s and its partial-update section runs ~129 s for 7 refreshes. Tricolor
panels need the long waveform for red, so neither mode is actually fast — GxEPD2's
own header annotates `hasPartialUpdate` with "but refresh is full screen" and sets
`hasFastPartialUpdate = false`. Do not expect the datasheet's 1.6 s.

Never send `0x30` (Program OTP of Waveform Setting) or `0x39` (OTP program mode)
to this panel — both are irreversible.

## The Waveshare 2.66" (monochrome) / GDEY0266T90 sketches

The monochrome counterpart to the 2.66" (B) above: 296x152, SSD1680 controller,
monochrome black & white with 4-level grayscale and true fast / partial refresh
support (Waveshare SKU 18401, FPC-7510 REV.C, Good Display `GDEY0266T90`). Same
Seeed driver board, same six pins. Covered by the same four driver paths:

- `Waveshare_2in66/` — Waveshare's vendored monochrome driver (`epd2in66`,
  `epdif`, `epdpaint`, `imagedata`). Fixed upstream C++ `static` error on
  out-of-class member definitions, added 40s BUSY timeout, and applies
  `EPD_2IN66_ROTATE_180` in `epd2in66.cpp`.
- `Adafruit_EPD/XIAO_Waveshare_2in66/` — stock `ThinkInk_266_Grayscale4_MFGN`.
  Supports both 1-bit monochrome and 4-level grayscale. Demonstrates typography
  and 4-level gray gradient swatches.
- `GxEPD2/GDEY0266T90/Demo/` — stock `GxEPD2_266_GDEY0266T90` with `<GxEPD2_BW.h>`,
  following the standard six-screen monochrome suite with `fillDitheredRect()`.
- `good_display/GDEY0266T90/` — Good Display's vendor sample (`AU-GDEY0266T90`),
  exposing official full refresh (2s), fast refresh 1 (1.5s), fast refresh 2
  (1.0s), and partial refresh clock demo.

**180 degree correction**: Like the tricolor panel, this panel's origin is at the
opposite corner on the 24-pin FPC connector:

| Sketch | Knob |
| --- | --- |
| `Waveshare_2in66/` | `EPD_2IN66_ROTATE_180` in `epd2in66.cpp` |
| `Adafruit_EPD/XIAO_Waveshare_2in66/` | `setRotation(2)` after `begin()` |
| `GxEPD2/GDEY0266T90/Demo/` | `DEMO_ROTATION 3` |
| `good_display/GDEY0266T90/` | `EPD_INIT_180` → Good Display's `EPD_HW_Init_180()` |

Unlike the tricolor panel, monochrome refresh on the SSD1680 is genuinely fast:
full refresh takes ~1.7s to 2s, fast refresh takes ~1.0s to 1.5s, and partial
refresh updates in ~0.5s without flashing.

## The Waveshare 1.54" (G) / GDEM0154F51H sketches

The 4-color e-Paper panel: 200x200, JD79660AA controller, 4 native colors
(Black, White, Yellow, Red) at 2 bits per pixel (Waveshare SKU 30441, FPC-8101,
Good Display `GDEM0154F51H`). Same Seeed driver board, same six pins (`RST D0`,
`CS D1`, `BUSY D2`, `DC D3`, `SCK D8`, `MOSI D10`). Covered by four independent
driver paths:

- `Waveshare_1in54g/` — Waveshare's vendored driver (`DEV_Config`, `EPD_1in54g`,
  `GUI_Paint`, `ImageData`, `fonts`). Displays the official 200x200 4-color bitmap
  (`Image4color`), renders in-memory 4-color geometric and typography primitives,
  and exercises fast-init mode. Added 40s BUSY timeout.
- `Adafruit_EPD/XIAO_Waveshare_1in54g/` — in-sketch `ThinkInk_154_Quadcolor_Waveshare`
  panel class subclassing `Adafruit_JD79661`. Injects the JD79660 200x200
  initialization sequence and exercises 4-color typography and concentric shapes.
- `GxEPD2/GDEM0154F51H/Demo/` — stock `GxEPD2_154c_GDEM0154F51H` with `<GxEPD2_4C.h>`,
  running the standard 6-screen 4-color demo suite. Fits completely in one 10KB
  RAM buffer on the XIAO MG24.
- `good_display/GDEM0154F51H/` — Good Display's vendor sample (`AU-GDEM0154F51H`),
  demonstrating full refresh (~20s) and fast update mode (~12-15s) with `gImage_1`.

Hardware and controller specifics:
- **BUSY is active LOW** on JD79660 (0 = busy, 1 = ready/idle), opposite of SSD1680.
- **Pixel packing**: 2 bits per pixel (00 = Black, 01 = White, 10 = Yellow, 11 = Red),
  4 pixels per byte (50 bytes/row, 10,000 bytes total).
- **Refresh timing**: Full refresh takes ~20s; fast refresh takes ~12-15s. No partial refresh.

## The Waveshare 3.7" (G) / GDEM037F51 sketches

The 3.7-inch 4-color e-Paper panel: 240x416, IST7163 controller, 4 native colors
(Black, White, Yellow, Red) at 2 bits per pixel (Waveshare SKU 31065, FPC-2303,
Good Display `GDEM037F51`). Same Seeed driver board, same six pins (`RST D0`,
`CS D1`, `BUSY D2`, `DC D3`, `SCK D8`, `MOSI D10`). Covered by three independent
driver paths:

- `Waveshare_3in7g/`: Waveshare's vendored driver (`DEV_Config`, `EPD_3in7g`,
  `GUI_Paint`, `ImageData`, `fonts`). Renders 4-color geometric and typography
  primitives in memory and exercises `EPD_3IN7G_Init_Fast()`. Added 40s BUSY timeout.
- `good_display/GDEM037F51/`: Good Display's vendor structure adapted for XIAO MG24,
  demonstrating full refresh (~20s), fast update mode (~12-15s), and color fills.
- `GxEPD2/GDEM037F51/Demo/`: In-sketch `GxEPD2_370c_GDEM037F51` panel class subclassing
  `GxEPD2_EPD` with `<GxEPD2_4C.h>`, running the standard 6-screen 4-color suite.
  Fits in a single 25KB RAM buffer on the XIAO MG24.

Hardware and controller specifics:
- **BUSY is active LOW** on IST7163 (0 = busy, 1 = ready/idle).
- **Pixel packing**: 2 bits per pixel (00 = Black, 01 = White, 10 = Yellow, 11 = Red),
  4 pixels per byte (60 bytes/row, 24,960 bytes total).
- **Refresh timing**: Full refresh takes ~20s; fast refresh takes ~12-15s. No partial refresh.

## The Good Display 2.15" (monochrome) / GDEW0215T12 (GDEW0215T11) sketches

The 2.15-inch monochrome e-Paper panel: 208x112, UC8151D controller, 1-bit monochrome black & white with partial refresh support (Good Display `GDEW0215T12`, formerly `GDEW0215T11`, FPC `WFT0215CZA4`). Same Seeed driver board, same six pins (`RST D0`, `CS D1`, `BUSY D2`, `DC D3`, `SCK D8`, `MOSI D10`). Covered by three independent driver paths:

- `Adafruit_EPD/XIAO_GDEW0215T12/`: In-sketch `ThinkInk_215_Mono_GDEW0215T12` panel class subclassing `Adafruit_UC8151D`. Injects exact 208x112 resolution setting (`0x61`) and LUT initializations, testing typography, concentric geometry, and partial refresh bounding box updates.
- `GxEPD2/GDEW0215T12/Demo/`: In-sketch `GxEPD2_215_GDEW0215T12` panel class subclassing `GxEPD2_EPD` with `<GxEPD2_BW.h>`, running the standard 6-screen monochrome demo suite with `fillDitheredRect()`. Fits in a single 2912-byte RAM buffer on the XIAO MG24.
- `good_display/GDEW0215T12/`: Good Display official vendor sample adapted for the XIAO MG24, demonstrating full refresh (3s) and partial refresh clock mode (0.5s).

Hardware and controller specifics:
- **BUSY is active LOW** on UC8151D (0 = busy, 1 = ready/idle).
- **Pixel packing**: 1 bit per pixel (0 = Black, 1 = White), 8 pixels per byte (14 bytes/row, 2,912 bytes total).
- **Refresh timing**: Full refresh takes ~3s; partial refresh takes ~0.5s without full-screen flicker.

## The Good Display 1.54" (4-color) / GDEM0154F61H sketches

The 1.54-inch 4-color e-Paper panel: 200x200, SSD2681 controller, 4 native colors (Black, White, Yellow, Red) at 2 bits per pixel (Good Display `GDEM0154F61H`, FPC `FPC-8101`). Same Seeed driver board, same six pins (`RST D0`, `CS D1`, `BUSY D2`, `DC D3`, `SCK D8`, `MOSI D10`). Covered by three independent driver paths:

- `good_display/GDEM0154F61H/`: Good Display official vendor sample adapted for the XIAO MG24, demonstrating full refresh (~20s), fast update mode (~12-15s), and color fills.
- `GxEPD2/GDEM0154F61H/Demo/`: In-sketch `GxEPD2_154c_GDEM0154F61H` panel class subclassing `GxEPD2_EPD` with `<GxEPD2_4C.h>`, running the standard 6-screen 4-color demo suite. Fits in a single 10KB RAM buffer on the XIAO MG24.
- `Adafruit_EPD/XIAO_GDEM0154F61H/`: In-sketch `ThinkInk_154_Quadcolor_GDEM0154F61H` panel class subclassing `Adafruit_JD79661`. Injects SSD2681 initialization commands (`0xE9 0x01`, `0x04`), testing typography, geometric shapes, and 4-color swatches.

Hardware and controller specifics:
- **BUSY is active LOW** on SSD2681 (0 = busy, 1 = ready/idle).
- **Pixel packing**: 2 bits per pixel (00 = Black, 01 = White, 10 = Yellow, 11 = Red), 4 pixels per byte (50 bytes/row, 10,000 bytes total).
- **Refresh timing**: Full refresh takes ~20s; fast refresh takes ~12-15s. No partial refresh.

## The Good Display 2.13" (4-color) / GDEY0213F52 sketches

The 2.13-inch 4-color e-Paper panel: 250x122, JD79676A controller, 4 native colors (Black, White, Yellow, Red) at 2 bits per pixel (Good Display `GDEY0213F52`, FPC `FPC-J002`, DESPI-C02 adapter pinout). Not to be confused with `GDEY0213F51` (JD79661, stock GxEPD2 class) which is the same size. Same Seeed driver board, same six pins (`RST D0`, `CS D1`, `BUSY D2`, `DC D3`, `SCK D8`, `MOSI D10`). Covered by three independent driver paths; GxEPD2 and Adafruit_EPD have no F52 support, so both classes live in-sketch:

- `good_display/GDEY0213F52/`: Good Display official vendor sample (`AU-GDEY0213F52` V2.0) adapted for the XIAO MG24, demonstrating full refresh (~11s), fast update mode (`gImage_2`), and color fills.
- `GxEPD2/GDEY0213F52/Demo/`: In-sketch `GxEPD2_213c_GDEY0213F52` panel class subclassing `GxEPD2_EPD` with `<GxEPD2_4C.h>`, running the standard 6-screen 4-color suite. Reports `GxEPD2::GDEY0213F51` as its panel enum. Single 8KB RAM buffer.
- `Adafruit_EPD/XIAO_GDEY0213F52/`: In-sketch `ThinkInk_213_Quadcolor_GDEY0213F52` panel class subclassing `Adafruit_JD79661` (122x250, width padded to 128 by the library) with the short init (`0xE9 0x01`, `0x04`) instead of Adafruit's JD79661 init.

Hardware and controller specifics:
- **BUSY is active LOW** on JD79676A (0 = busy, 1 = ready/idle).
- **Pixel packing**: 2 bits per pixel (00 = Black, 01 = White, 10 = Yellow, 11 = Red), 4 pixels per byte. RAM is 128 pixels wide (122 visible): 32 bytes/row, 250 rows, 8,000 bytes total.
- **Init**: after reset only `0xE9 0x01` and `0x04` (POWER_ON) are sent; waveform and panel settings come from OTP. Fast init prepends `0xE0 0x02`, `0xE6 90`, `0xA5`. Re-init is required before every full update. Sleep is `0x02 0x00` then `0x07 0xA5`.
- **Refresh timing**: Full refresh takes ~11s. No partial refresh.
- **Orientation**: all three sketches are compile-verified only; the 180 degree question (`DEMO_ROTATION` in the GxEPD2 and Adafruit sketches) has not been checked on hardware.

## The Good Display 2.66" (monochrome, high resolution) / GDEY0266T90H sketches

The higher-resolution sibling of the GDEY0266T90: 360x184, SSD1685 controller, black/white with full, fast and partial refresh (Good Display `GDEY0266T90H`, FPC `FPC-H011`). Not to be confused with `GDEY0266T90` (152x296, SSD1680, stock GxEPD2 class) or the 2.9" `GDEY029T71H` (also SSD1685, 168x384). Same Seeed driver board, same six pins. GxEPD2 and Adafruit_EPD have no GDEY0266T90H support, so both classes live in-sketch. Three independent driver paths:

- `good_display/GDEY0266T90H/`: Good Display official vendor sample (`AU-GDEY0266T90H-2FP-20230915`) adapted for the XIAO MG24: full refresh, fast refresh 1 and 2, partial-refresh clock, `EPD_INIT_180` knob. The vendor sample ships only `gImage_1` (no `gImage_2`), unlike the T90.
- `GxEPD2/GDEY0266T90H/Demo/`: In-sketch `GxEPD2_266_GDEY0266T90H` class with `<GxEPD2_BW.h>`, derived from the library's `GxEPD2_290_GDEY029T71H` (same SSD1685 command set) with this panel's geometry. Standard six-screen demo, single 8,280-byte page buffer.
- `Adafruit_EPD/XIAO_GDEY0266T90H/`: In-sketch `ThinkInk_266_Mono_GDEY0266T90H` subclassing `Adafruit_SSD1680` with `(360, 184)` long axis first, `_xram_offset = 0`, and a mono init table.

Hardware and controller specifics:
- **BUSY is active HIGH** (1 = busy), like the SSD1680 and unlike the 4-color panels in this repo.
- **RAM**: 184 pixels wide (23 bytes/row) by 360 rows, 8,280 bytes. 1 bit per pixel, 0 = black, 1 = white. Command `0x24` is the current image, `0x26` the previous one (vendor full refresh writes `0x00` to it).
- **RAM mapping**: Good Display's sample uses data entry `0x01` (x increase, y decrease) with images pre-flipped for it; `EPD_HW_Init_180()` uses `0x02`. The GxEPD2 class uses the library's `0x03` instead: `0x01` with a reversed y window showed GxEPD2's raster mirrored on hardware.
- **Update values**: full `0x22 0xF4` (GxEPD2 class uses `0xF7`), fast `0xC7` after loading temperature `0x6E` (fast 1) or `0x5A` (fast 2), partial `0x1C`. Deep sleep `0x10 0x01`. Re-init before every full refresh.
- **Refresh timing**: full ~2s, fast 1 ~1.5s, fast 2 ~1.0s, partial ~0.4s.
- **Orientation**: Unlike the GDEY0266T90, this panel does not need the 180 degree correction in the GxEPD2 demo: `DEMO_ROTATION` is 1 (checked on hardware), and Adafruit uses rotation 2. `EPD_INIT_180` in the vendor sketch is unverified.
