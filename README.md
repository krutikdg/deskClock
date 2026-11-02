# deskClock

A USB powered desk clock that shows the time, date, weather and room conditions on a 2.4" display. The ESP32 syncs the time over WiFi, pulls the weather from Open-Meteo, and reads room temperature, humidity and pressure from a BME280. A light sensor dims the backlight at night, and a PIR sensor wakes the screen when someone is at the desk. Two buttons switch between a Pomodoro timer, a stopwatch and my GitHub streak, and a buzzer sounds alarms and timer alerts. When nothing is happening, the clock drops into an idle mode with a small animated face. I built it to keep me on task while it sits on my desk, and help me focus study better with pomodoro timer. 

## Built with

- ESP32 DevKit V1
- 2.4" SPI TFT display (ILI9341, 240x320)
- BME280 (room temperature, humidity and pressure)
- BH1750 light sensor (automatic backlight dimming)
- HC-SR501 PIR sensor (wakes the screen)
- Passive buzzer driven by a BC547 transistor (alarms and timers)
- 2x tactile push buttons
- Arduino (C++) with the LovyanGFX, ArduinoJson, Adafruit BME280 and BH1750 libraries
- Open-Meteo API (weather) and GitHub GraphQL API (contribution streak)
- Cirkit Designer (circuit design)
- Fusion 360 (case design)
- PLA, 3D printed

## Bill of Materials

| Component | Quantity | Price/Unit (USD) | Purpose | Purchase Link |
|---|---|---|---|---|
| ESP32 DevKit V1 (30-pin) | 1 | 4.50 | Main controller with WiFi for time sync and weather and GitHub data | [Link][esp32] |
| 2.4 Inch SPI TFT Display Module 240x320 (ILI9341) | 1 | 7.50 | Shows the clock and all the screens | [Link][tft] |
| BME280 I2C Temperature Humidity Pressure Sensor Module | 1 | 5.70 | Measures room temperature and humidity and air pressure | [Link][bme280] |
| BH1750 (GY-302) Ambient Light Sensor Module | 1 | 1.20 | Dims the display backlight automatically at night | [Link][bh1750] |
| HC-SR501 PIR Motion Sensor Module | 1 | 1.60 | Wakes the screen when someone is at the desk | [Link][pir] |
| Passive Buzzer 12mm | 1 | 1.60 | Plays alarm and timer tones | [Link][buzzer] |
| BC547 NPN Transistor (Pack of 20) | 1 | 1.60 | Switches the buzzer so the ESP32 pin is not overloaded | [Link][bc547] |
| 1K Ohm 1/4W Resistors (Pack of 10) | 1 | 0.07 | Base resistor for the buzzer transistor | [Link][resistor] |
| 6x6x5mm Tactile Push Button Switch (Pack of 10) | 1 | 0.21 | Two buttons to change screens and start or stop timers | [Link][tactile] |
| Micro-USB Data Cable 1m | 1 | 0.41 | Powers the clock and uploads the code | [Link][cable] |
| 5V 1A USB Power Adapter | 1 | 2.03 | Powers the clock from a wall socket | [Link][adapter] |
| 830 Point Breadboard | 1 | 0.73 | Testing the circuit before soldering | [Link][breadboard] |
| Jumper Wires Kit (M-M and M-F and F-F) | 1 | 1.68 | Connections between the boards during testing | [Link][jumpers] |
| 7x9 cm Universal PCB Prototype Board (Single Sided) | 1 | 0.52 | Mounts the buzzer transistor and resistor circuit permanently | [Link][perfboard] |
| eSUN PLA Matte Deep Black 1.75mm Filament | 1 | 13.70 | 3D printing the case in black | [Link][filament] |

**Total: $43.05** 

[esp32]: https://robocraze.com/products/esp32-development-board-with-cp2102-wifi-bluetooth-dual-core-30-pin?variant=53806381531360
[tft]: https://robu.in/product/2-4-inch-spi-interface-240x320-touch-screen-tft-display-module/
[bme280]: https://robu.in/product/smartelex-bme280-atmospheric-sensor-breakout-board/
[bh1750]: https://robocraze.com/products/gy-302-bh1750-light-intensity-module?variant=42268132016352
[pir]: https://www.google.com/aclk?sa=L&ai=DChsSEwjYrbXZjemXAxVDJYMDHbWUNDUYACICCAEQARoCc2Y&co=1&ase=2&gclid=CjwKCAiAhqHXBhAXEiwATbksU8TWhiooQNbmEkpQAU9mmC0AL7-XYMtwO0NxRW9AufBt1GtDD9xMXRoCih8QAvD_BwE&cid=CAAS3QHkaCf8eiUkRyEXCBGaCocuhSLUgknyotJO-Hz_3nhUBjfV9PkY_xcGm2_b6IfyiuERK8CNACV34upkVf03Dh7_f0vOmZTHsHHS2l6w3jbBd38aBnlgV-jWhM0QHiEi0SCuD-3Bug2T6Sny6iX2Tzl4oAk3TMd1jsduB6_PDXQ8nm6FJcCpZFsGClt5DCYqJUrJR2A-DPUIMG7HDB69XYyBZ46UpJLZCD76adR-ze7ymlHja6oqnJSSnFWz73sP4WAujkXQoPs812zEWMMLgPd3NaXscOq4Lf6blQvx6Q&cce=2&category=acrcp_v1_33&sig=AOD64_2NaySK2JEsGeOAiHg29WBCdvwKBg&ctype=5&q&nis=4&adurl
[buzzer]: https://www.google.com/search?ibp=oshop&prds=pvt:hg,pvo:48,imageDocid:6826502872093707244,headlineOfferDocid:11410333701803615999,productDocid:11410333701803615999&q=product&sa=X&ved=2ahUKEwjVgbfHjemXAxVl4zgGHZVxJpEQgLcPegoIAggACAAIExAE&biw=756&bih=798&dpr=2
[bc547]: https://www.google.com/search?q=BC547+NPN+Transistor+%28Pack+of+20%29
[resistor]: https://quartzcomponents.com/products/1k-ohm-1-4-watt-resistor
[tactile]: https://www.google.com/search?q=6x6x5mm+Tactile+Push+Button+Switch+%28Pack+of+10%29
[cable]: https://robu.in/product/usb-to-micro-usb-cable-wire-1m-for-nodemcu/
[adapter]: https://robu.in/product/orange-5v-1a-power-supply-with-5-5mm-dc-plug-adapter/
[breadboard]: https://robu.in/product/mb102-830-points-solderless-prototype-pcb-breadboard-high-quality/
[jumpers]: https://www.google.com/aclk?sa=L&ai=DChsSEwjXsJekkOmXAxUWk2YCHbVRFgIYACICCAEQBBoCc20&co=1&ase=2&gclid=CjwKCAiAhqHXBhAXEiwATbksUw2z1k4IuFFMIn9EAR8lU_7vXtQpmek-2IJjghbKfZmvPKUk9YItvhoCDiEQAvD_BwE&cid=CAAS3QHkaB_mDU7iqm77q725qPczSNhXen5IYHgLUE6g_-AmpABOcEI3BpczZ4CiptZYvvSGhAL_ug2UWo6g3ZJd3DxT36YVOOfyczfN6NOA3U9YiLNZHrjh74A_vYEJ5s9qiYsFKw3OQqlHsW_uOMrk1gmkdsF2fwSDfIUnrWMLieoifXh_jhTTN8v5mQ0IkatjhDlqsF3nJxnFVSMLpha49r5ADM4xls7xge6RPbIbhRn2Ny27OSHhYcryKt8AHdb219RvYIBweqn7ZiH5RTbdqlbJfgLpeVZg1mumxys9Iw&cce=2&category=acrcp_v1_32&sig=AOD64_3dp55Qw5ShxpZ2riyH4FjcWgvV1A&ctype=5&q=&nis=4&ved=2ahUKEwjWsI-kkOmXAxUowzgGHbW3L_wQ5bgDKAB6BAgMEAs&adurl=
[perfboard]: https://www.google.com/search?q=7x9+cm+Universal+PCB+Prototype+Board+%28Single+Sided%29
[filament]: https://www.google.com/aclk?sa=L&ai=DChsSEwj5saPekOmXAxXXimYCHbSaB-gYACICCAEQAxoCc20&co=1&ase=2&gclid=CjwKCAiAhqHXBhAXEiwATbksU9eGFoGc2iJ4Q-3A-MDwoa2Rw3I6etRBQBVT3S_B58F3FB1l6U6EQxoC6pcQAvD_BwE&cid=CAAS3QHkaKzLWq60ZtZgkVUjjQkymVDWjCepCZsVbdaMph-auFfHusjYNJLzErO6NKAUvUHJHSfX9mL3lM73UnlyTjq8PfIvuE7VL8SPhuQj2GFLfTdh-BdB_r1WgEP3-Z90YhOeQq3Bj533sF4mORdRJsKHXjoeTqDCsGM1cZs0DhC04F3BuIhnhuKObgsmZJ7WIzzbuEV8w6tkHoIhneAytMa1Msa8SsyeyXUBpgKX-MHgzhoh932fujCl91pUkoevayw_xYS7nwG0hXIpqkXJvI5ZW0JCMwM_4WxXapO7FQ&cce=2&category=acrcp_v1_32&sig=AOD64_0ZD7ANSCux-IvFwPihHKeRcx5lWw&ctype=5&q=&nis=4&ved=2ahUKEwj3i5zekOmXAxUHyqACHaEVNPkQ5bgDKAB6BAgMEAs&adurl=

## Circuit Diagram 
<img width="804" height="439" alt="image" src="https://github.com/user-attachments/assets/902f8118-e329-4791-8cec-ceaa249df4f9" />

## Pin Mapping

Board: ESP32 DevKit V1 (30-pin). Powered over USB.

| Component | Component Pin | ESP32 Pin |
|---|---|---|
| TFT display (ILI9341) | VCC | `3V3` |
| TFT display (ILI9341) | GND | `GND` |
| TFT display (ILI9341) | SCK | `GPIO18` |
| TFT display (ILI9341) | MOSI (SDA) | `GPIO23` |
| TFT display (ILI9341) | CS | `GPIO5` |
| TFT display (ILI9341) | DC | `GPIO27` |
| TFT display (ILI9341) | RST | `GPIO26` |
| TFT display (ILI9341) | LED (BLK) | `GPIO25` |
| TFT display (ILI9341) | MISO | not connected |
| BME280 | VIN | `3V3` |
| BME280 | GND | `GND` |
| BME280 | SDA | `GPIO21` |
| BME280 | SCL | `GPIO22` |
| BH1750 | VIN | `3V3` |
| BH1750 | GND | `GND` |
| BH1750 | SDA | `GPIO21` |
| BH1750 | SCL | `GPIO22` |
| PIR (HC-SR501) | VCC | `VIN` (5V) |
| PIR (HC-SR501) | GND | `GND` |
| PIR (HC-SR501) | OUT | `GPIO34` |
| Button A | Leg 1 | `GPIO32` |
| Button A | Leg 2 | `GND` |
| Button B | Leg 1 | `GPIO33` |
| Button B | Leg 2 | `GND` |
| 1k resistor | End 1 | `GPIO13` |
| 1k resistor | End 2 | BC547 base |
| BC547 | Collector | Buzzer negative |
| BC547 | Emitter | `GND` |
| Passive buzzer | Positive | `3V3` |

### I2C addresses

| Device | Address |
|---|---|
| BME280 | `0x76` or `0x77` |
| BH1750 | `0x23` (or `0x5C`) |

## Build steps

1. Print the base (`base.stl`) and lid (`lid.stl`) from `/CAD`.
2. Test each module on a breadboard before wiring anything permanently, the display first, then the BME280 and BH1750 (an I2C scan should show `0x76` or `0x77` and `0x23`), then the PIR sensor, then the buzzer through its transistor.
3. Wire everything following the pin map above.
4. Solder the buzzer driver (BC547 and 1 kΩ resistor) onto the prototype board.
5. Fit the parts into the case, display, ESP32 on the lid, PIR behind the dome hole, light sensor behind its window, buttons and buzzer under the top face.
6. Place the BME280 next to the vent slots, as far from the ESP32 as the case allows.
7. Install the ESP32 board package and the libraries (LovyanGFX, ArduinoJson 7, Adafruit BME280, Adafruit Unified Sensor, BH1750). Fill in the settings block at the top of `desk_clock.ino` (WiFi, location, alarm, countdown date), select `ESP32 Dev Module`, and flash the firmware.
8. Close the case with the screws.

Keep your WiFi password and GitHub token out of any commit. Edit a local copy of the settings block, or leave the placeholders in the repository.

## Calibration

1. **Display direction:** if the picture is upside down, change `tft.setRotation(1)` to `3` in `setup()`. If black and white are swapped, set `cfg.invert = true` in the display class. If your module uses an ST7789 chip, change `Panel_ILI9341` to `Panel_ST7789`.
2. **Time zone and location:** the clock is set to IST in `configTzTime("IST-5:30", ...)`. The weather uses `LATITUDE` and `LONGITUDE`, which default to Bengaluru. Change all three for another place.
3. **Room temperature:** the BME280 sits inside a case with a warm ESP32 and display, so it may read high. Let the clock run for an hour, compare it with a reference thermometer, and subtract the difference in `readRoom()`.
4. **Brightness:** `updateBrightness()` maps 2 to 150 lux onto the backlight range. If the screen is too bright in a dim room, lower the 150.0f limit. If it is too dim in daylight, raise it.
5. **PIR sensor:** the HC-SR501 has two trimmers for sensitivity and delay time. Turn them until the screen wakes when you sit down and sleeps a few seconds after you leave. The screen turns itself off after `SCREEN_OFF_MS` (10 minutes) without motion.
6. **Timers and dates:** set the Pomodoro lengths with `WORK_MS` and `BREAK_MS`, the alarm with `ALARM_HOUR` and `ALARM_MIN`, and the countdown with `TARGET_LABEL`, `TARGET_YEAR`, `TARGET_MONTH` and `TARGET_DAY`.

## Troubleshooting

- **Blank or white screen:** check the wiring on CS, DC, RST and the backlight pin first. A wrong display driver gives the same symptom.
- **Colors or orientation wrong:** see Calibration step 1.
- **Clock shows `--:--` or "Waiting for WiFi":** check the SSID and password. The ESP32 only joins 2.4 GHz networks.
- **Time is off by hours:** check the time zone string in `configTzTime`.
- **`Room --` on the clock screen:** the BME280 was not found. Check SDA on GPIO21 and SCL on GPIO22. The code tries both `0x76` and `0x77`.
- **`Weather --`:** the first fetch can take a few seconds. If it stays, check your internet connection and the coordinates.
- **GitHub screen says "Add token in code" or stays on "Fetching...":** check `GH_USER` and `GH_TOKEN`. Press button B on that screen to retry.
- **Buzzer silent or stuck on:** check the BC547 pin order against its datasheet and the 1 kΩ resistor on GPIO13. Use a passive buzzer. An active one plays a fixed tone.
- **Screen too dim or too bright:** see Calibration step 4.
- **Screen turns off while you are at the desk:** adjust the PIR trimmers.
- **Random resets:** try a different USB cable or power adapter.
- **Upload fails:** use a data cable, select the right port, and hold the BOOT button while the IDE says "Connecting...".
- **Build error about `JsonDocument`:** install ArduinoJson 7, because version 6 does not have it.

## CAD Model
1. Case
    <img width="1600" height="1039" alt="WhatsApp Image 2026-11-02 at 15 47 44 (1)" src="https://github.com/user-attachments/assets/c2e0abe9-d234-4d74-a6e1-5b60a9f49803" />
  4 inward extruded corners are for you to stick the lid with hot glue
2. Lid
   <img width="1600" height="1039" alt="WhatsApp Image 2026-11-02 at 15 47 44" src="https://github.com/user-attachments/assets/83ddd74a-6bff-4cbe-ae97-8f3ec36df3b0" />
   the ear like extrutions are for you to apply force and remove the lid from the base when it's stuck with glue
3. Complete
    <img width="1600" height="1039" alt="WhatsApp Image 2026-11-02 at 15 47 43" src="https://github.com/user-attachments/assets/48554d89-c90d-4ecf-917b-da1699eb60d8" />
