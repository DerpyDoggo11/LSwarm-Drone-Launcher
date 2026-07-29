# Raspberry Pi Zero 2 W Firmware — LSwarm Drone Launcher

The **central compute** node. The Pi Zero 2 W is a Linux SBC, so this is a
PlatformIO `native` C++ project written in the same Arduino `setup()`/`loop()`
style as the ESP32 firmware — a small Linux shim
([`include/arduino_compat.h`](include/arduino_compat.h)) provides `millis()`,
`delay()`, and a `Serial`-like port over the USB-CDC link.

Today it owns the link to the ESP32 co-processor (battery/fault telemetry,
LED/buzzer commands, keep-alive) and forwards operator input. Swarm / Wi-Fi
coordination logic builds on top of `loop()` in [`src/main.cpp`](src/main.cpp).

## How it connects

The ESP32-C3 native USB is shared with the Pi's USB-OTG lines on the USB-C
port, so the co-processor enumerates on the Pi as **`/dev/ttyACM0`**. The wire
protocol is defined in [`include/protocol.h`](include/protocol.h) (identical to
the ESP32's copy).

## Build & run

```bash
# On the Pi (or any Linux host), with PlatformIO Core:
pio run                                   # -> .pio/build/pizero/program
.pio/build/pizero/program                 # default: /dev/ttyACM0 @ 115200
.pio/build/pizero/program /dev/ttyACM1    # override port/baud

# Or straight from g++ if you'd rather skip PlatformIO:
g++ -std=c++17 -O2 -Iinclude src/*.cpp -o program
```

Add your user to the `dialout` group for serial access:
`sudo usermod -aG dialout $USER` (re-login after).

Run it as a service with the included [`lswarm-compute.service`](lswarm-compute.service).

## Using it

While running, type a command and press Enter — it goes straight to the ESP32:

```
LED blink          # status LED pattern: off | on | blink | beat | sos
BEEP 2             # two feedback beeps
BUZZ 2700 300      # 2.7 kHz tone for 300 ms
STATUS?            # request one telemetry frame
TELEM 500          # set auto-telemetry to 500 ms (0 = off)
quit               # exit
```

Incoming telemetry is printed live, e.g.
`[   12345] BATT 4013 mV  soc=79%  fault=0`.
