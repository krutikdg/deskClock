# deskClock
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

**Estimated total: $43.05** 

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
