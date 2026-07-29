# ESP32-C3 Firmware — LSwarm Drone Launcher

Power/status co-processor that runs on the **ESP32-C3-MINI-1U-N4 (U6)** of the
Compute Hat. It measures the battery, watches the protection-IC fault line, and
drives the status LED and buzzer, exposing everything to the Raspberry Pi over
a USB-CDC line protocol.

## Hardware map (from the schematic)

| Function      | GPIO | Notes                                                        |
|---------------|------|--------------------------------------------------------------|
| Status LED    | 2    | Active-**low** (sinks from 3V3 via R21 1k). Boot strap.      |
| DW03C fault   | 3    | `DW_IRQ`, active-**low** input, internal pull-up.            |
| Battery VMON  | 0    | ADC1_CH0, reads `BATT+ / 2` (R25 220k / R26 220k divider).   |
| Buzzer        | 8    | Active-**high**, MOSFET-driven MLT-5020, PWM tone. Strap.    |
| Boot          | 9    | Strap / test point — not driven by firmware.                 |

## Build / flash

```bash
pio run                 # build
pio run -t upload       # flash over the USB-C port
pio device monitor      # open the CDC serial monitor (115200)
```

## Link protocol

ASCII, one message per line (`\n`). Full definition in
[`include/protocol.h`](include/protocol.h) — kept identical to the Pi firmware.

**Pi → ESP32**

| Command            | Effect                                             |
|--------------------|----------------------------------------------------|
| `PING`             | replies `PONG`                                     |
| `ID?`              | replies `ID LSwarm-ESP32C3 fw=<ver>`               |
| `STATUS?`          | emits one `TELEM` frame immediately                |
| `LED <mode>`       | `off` `on` `blink` `beat` `sos`                    |
| `BUZZ <hz> <ms>`   | play a tone for `<ms>` (`0` = stop)                |
| `BEEP <count>`     | N short feedback beeps                             |
| `TELEM <period_ms>`| set auto-telemetry period (`0` = off)              |

**ESP32 → Pi**

| Message                                    | When                     |
|--------------------------------------------|--------------------------|
| `RDY <ver>`                                | once at boot             |
| `TELEM v=<mV> soc=<pct> fault=<0/1> up=<ms>` | periodic / on `STATUS?` |
| `EVT fault=<0/1>`                          | on a DW_IRQ edge         |
| `OK <cmd>` / `ERR <reason>` / `PONG`       | command replies          |
