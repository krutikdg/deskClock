#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <BH1750.h>
#include <time.h>

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

const char* WIFI_SSID = "your-wifi-name";
const char* WIFI_PASS = "your-wifi-password";
const char* GH_USER = "your-github-username";
const char* GH_TOKEN = "";

const float LATITUDE = 12.9716f;
const float LONGITUDE = 77.5946f;

const bool ALARM_ON = true;
const int ALARM_HOUR = 6;
const int ALARM_MIN = 30;

const char* TARGET_LABEL = "Exam";
const int TARGET_YEAR = 2026;
const int TARGET_MONTH = 12;
const int TARGET_DAY = 15;

const uint32_t WORK_MS = 25UL * 60UL * 1000UL;
const uint32_t BREAK_MS = 5UL * 60UL * 1000UL;
const uint32_t IDLE_MS = 120000UL;
const uint32_t RETURN_MS = 60000UL;
const uint32_t SCREEN_OFF_MS = 600000UL;
const uint32_t WEATHER_EVERY_MS = 900000UL;
const uint32_t GITHUB_EVERY_MS = 1800000UL;
const uint32_t RETRY_MS = 60000UL;

const int PIN_TFT_SCK = 18;
const int PIN_TFT_MOSI = 23;
const int PIN_TFT_CS = 5;
const int PIN_TFT_DC = 27;
const int PIN_TFT_RST = 26;
const int PIN_TFT_BL = 25;
const int PIN_SDA = 21;
const int PIN_SCL = 22;
const int PIN_PIR = 34;
const int PIN_BTN_A = 32;
const int PIN_BTN_B = 33;
const int PIN_BUZZER = 13;

const uint32_t COL_BG = 0x000000;
const uint32_t COL_FG = 0xFFFFFF;

class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_ILI9341 panel;
  lgfx::Bus_SPI bus;
  lgfx::Light_PWM light;

 public:
  LGFX() {
    {
      auto cfg = bus.config();
      cfg.spi_host = SPI3_HOST;
      cfg.spi_mode = 0;
      cfg.freq_write = 40000000;
      cfg.freq_read = 16000000;
      cfg.spi_3wire = false;
      cfg.use_lock = true;
      cfg.dma_channel = SPI_DMA_CH_AUTO;
      cfg.pin_sclk = PIN_TFT_SCK;
      cfg.pin_mosi = PIN_TFT_MOSI;
      cfg.pin_miso = -1;
      cfg.pin_dc = PIN_TFT_DC;
      bus.config(cfg);
      panel.setBus(&bus);
    }
    {
      auto cfg = panel.config();
      cfg.pin_cs = PIN_TFT_CS;
      cfg.pin_rst = PIN_TFT_RST;
      cfg.pin_busy = -1;
      cfg.panel_width = 240;
      cfg.panel_height = 320;
      cfg.offset_x = 0;
      cfg.offset_y = 0;
      cfg.offset_rotation = 0;
      cfg.dummy_read_pixel = 8;
      cfg.dummy_read_bits = 1;
      cfg.readable = false;
      cfg.invert = false;
      cfg.rgb_order = false;
      cfg.dlen_16bit = false;
      cfg.bus_shared = false;
      panel.config(cfg);
    }
    {
      auto cfg = light.config();
      cfg.pin_bl = PIN_TFT_BL;
      cfg.invert = false;
      cfg.freq = 12000;
      cfg.pwm_channel = 7;
      light.config(cfg);
      panel.setLight(&light);
    }
    setPanel(&panel);
  }
};

enum Screen : uint8_t { SCR_CLOCK, SCR_POMO, SCR_STOPWATCH, SCR_COUNTDOWN, SCR_GITHUB, SCR_COUNT };
enum PomoState : uint8_t { POMO_IDLE, POMO_RUN, POMO_PAUSE };
enum BtnEvent : uint8_t { EV_NONE, EV_SHORT, EV_LONG };
enum TextAlign : uint8_t { AL_LEFT, AL_CENTER, AL_RIGHT };

struct PushButton {
  int pin;
  bool stable;
  bool reading;
  uint32_t changedAt;
  uint32_t downAt;
  bool longSent;
};

struct AlertState {
  uint16_t remaining;
  bool on;
  uint32_t nextAt;
  uint16_t freq;
  uint16_t onMs;
  uint16_t offMs;
};

LGFX tft;
LGFX_Sprite canvas(&tft);
Adafruit_BME280 bme;
BH1750 lightMeter(0x23);

bool bmeOk = false;
bool lightOk = false;
float roomTemp = NAN;
float roomHum = NAN;
float roomPres = NAN;
float brightness = 150;

volatile float wxTemp = 0;
volatile int wxCode = -1;
volatile bool wxValid = false;
volatile int ghStreak = 0;
volatile int ghToday = 0;
volatile bool ghValid = false;
volatile bool ghForce = false;

Screen screen = SCR_CLOCK;
bool use24h = false;
bool screenOff = false;
uint32_t lastInput = 0;
uint32_t lastMotion = 0;

PomoState pomoState = POMO_IDLE;
bool pomoWork = true;
uint32_t pomoRemaining = WORK_MS;
uint32_t pomoEndAt = 0;
int pomoDone = 0;

bool swRunning = false;
uint32_t swStart = 0;
uint32_t swElapsed = 0;

int alarmDay = -1;
bool alarmRinging = false;

AlertState alertState = {0, false, 0, 0, 0, 0};
PushButton btnA = {PIN_BTN_A, false, false, 0, 0, false};
PushButton btnB = {PIN_BTN_B, false, false, 0, 0, false};

void wake(uint32_t now) {
  lastMotion = now;
  screenOff = false;
}

void buzzerInit() {
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcAttach(PIN_BUZZER, 2000, 8);
#else
  ledcSetup(0, 2000, 8);
  ledcAttachPin(PIN_BUZZER, 0);
#endif
}

void buzzerTone(uint32_t freq) {
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcWriteTone(PIN_BUZZER, freq);
#else
  ledcWriteTone(0, freq);
#endif
}

void startAlert(uint16_t count, uint16_t freq, uint16_t onMs, uint16_t offMs) {
  alertState = {count, false, millis(), freq, onMs, offMs};
}

void stopAlert() {
  alertState.remaining = 0;
  alertState.on = false;
  buzzerTone(0);
}

void updateAlert(uint32_t now) {
  if (alertState.remaining == 0) return;
  if ((int32_t)(now - alertState.nextAt) < 0) return;
  if (!alertState.on) {
    buzzerTone(alertState.freq);
    alertState.on = true;
    alertState.nextAt = now + alertState.onMs;
  } else {
    buzzerTone(0);
    alertState.on = false;
    alertState.remaining--;
    alertState.nextAt = now + alertState.offMs;
  }
}

BtnEvent pollButton(PushButton& b, uint32_t now) {
  bool down = digitalRead(b.pin) == LOW;
  if (down != b.reading) {
    b.reading = down;
    b.changedAt = now;
  }
  BtnEvent ev = EV_NONE;
  if (now - b.changedAt >= 30 && down != b.stable) {
    b.stable = down;
    if (down) {
      b.downAt = now;
      b.longSent = false;
    } else if (!b.longSent) {
      ev = EV_SHORT;
    }
  }
  if (b.stable && !b.longSent && now - b.downAt >= 800) {
    b.longSent = true;
    ev = EV_LONG;
  }
  return ev;
}

const char* weatherText(int code) {
  if (code < 0) return "--";
  if (code <= 1) return "Clear";
  if (code <= 3) return "Cloudy";
  if (code == 45 || code == 48) return "Fog";
  if (code >= 51 && code <= 57) return "Drizzle";
  if (code >= 61 && code <= 67) return "Rain";
  if ((code >= 71 && code <= 77) || code == 85 || code == 86) return "Snow";
  if (code >= 80 && code <= 82) return "Showers";
  if (code >= 95) return "Storm";
  return "--";
}

bool fetchWeather() {
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;
  String url = "https://api.open-meteo.com/v1/forecast?latitude=" + String(LATITUDE, 4) +
               "&longitude=" + String(LONGITUDE, 4) +
               "&current=temperature_2m,weather_code&timezone=Asia%2FKolkata";
  if (!http.begin(client, url)) return false;
  http.useHTTP10(true);
  http.setTimeout(8000);
  bool ok = false;
  if (http.GET() == 200) {
    JsonDocument doc;
    if (!deserializeJson(doc, http.getStream())) {
      JsonVariant temp = doc["current"]["temperature_2m"];
      JsonVariant code = doc["current"]["weather_code"];
      if (!temp.isNull()) {
        wxTemp = temp.as<float>();
        wxCode = code | -1;
        ok = true;
      }
    }
  }
  http.end();
  return ok;
}

bool fetchGithub() {
  if (strlen(GH_TOKEN) == 0 || strlen(GH_USER) == 0) return false;
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;
  if (!http.begin(client, "https://api.github.com/graphql")) return false;
  http.useHTTP10(true);
  http.setTimeout(15000);
  http.addHeader("Authorization", String("Bearer ") + GH_TOKEN);
  http.addHeader("User-Agent", "esp32-desk-clock");
  http.addHeader("Content-Type", "application/json");
  String body = String("{\"query\":\"query{user(login:\\\"") + GH_USER +
                "\\\"){contributionsCollection{contributionCalendar{weeks{contributionDays{contributionCount}}}}}}\"}";
  bool ok = false;
  if (http.POST(body) == 200) {
    JsonDocument filter;
    filter["data"]["user"]["contributionsCollection"]["contributionCalendar"]["weeks"][0]["contributionDays"][0]["contributionCount"] = true;
    JsonDocument doc;
    if (!deserializeJson(doc, http.getStream(), DeserializationOption::Filter(filter))) {
      JsonArray weeks = doc["data"]["user"]["contributionsCollection"]["contributionCalendar"]["weeks"];
      int streak = 0;
      int beforeLast = 0;
      int lastCount = 0;
      bool any = false;
      for (JsonObject week : weeks) {
        for (JsonObject day : week["contributionDays"].as<JsonArray>()) {
          int c = day["contributionCount"] | 0;
          beforeLast = streak;
          if (c > 0) streak++;
          else streak = 0;
          lastCount = c;
          any = true;
        }
      }
      if (any) {
        ghStreak = lastCount > 0 ? streak : beforeLast;
        ghToday = lastCount;
        ok = true;
      }
    }
  }
  http.end();
  return ok;
}

void networkTask(void*) {
  uint32_t lastWx = 0;
  uint32_t lastGh = 0;
  bool wxFirst = true;
  bool ghFirst = true;
  for (;;) {
    if (WiFi.status() == WL_CONNECTED) {
      uint32_t now = millis();
      if (wxFirst || now - lastWx >= WEATHER_EVERY_MS) {
        bool ok = fetchWeather();
        if (ok) wxValid = true;
        lastWx = ok ? millis() : millis() - WEATHER_EVERY_MS + RETRY_MS;
        wxFirst = false;
      }
      now = millis();
      if (ghFirst || ghForce || now - lastGh >= GITHUB_EVERY_MS) {
        ghForce = false;
        bool ok = fetchGithub();
        if (ok) ghValid = true;
        lastGh = ok ? millis() : millis() - GITHUB_EVERY_MS + RETRY_MS;
        ghFirst = false;
      }
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void maintainWifi(uint32_t now) {
  static uint32_t last = 0;
  if (now - last < 30000) return;
  last = now;
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.disconnect();
    WiFi.begin(WIFI_SSID, WIFI_PASS);
  }
}

const lgfx::IFont* F_BIG = &lgfx::fonts::Font7;
const lgfx::IFont* F_MID = &lgfx::fonts::Font4;
const lgfx::IFont* F_SMALL = &lgfx::fonts::Font2;

const char* DAYS[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
const char* MONTHS[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
const int8_t DRIFT_X[] = {-50, 30, -20, 50, -40, 10};
const int8_t DRIFT_Y[] = {-10, 10, 20, -15, 5, -20};

void put(const char* s, int x, int y, const lgfx::IFont* f, float size, TextAlign a) {
  canvas.setFont(f);
  canvas.setTextSize(size);
  canvas.setTextColor(COL_FG, COL_BG);
  if (a == AL_LEFT) canvas.drawString(s, x, y);
  else if (a == AL_CENTER) canvas.drawCenterString(s, x, y);
  else canvas.drawRightString(s, x, y);
}

void bootMessage(const char* msg) {
  canvas.fillScreen(COL_BG);
  put(msg, 160, 110, F_MID, 1.0f, AL_CENTER);
  canvas.pushSprite(0, 0);
}

void formatTime(const tm& t, bool ok, char* out, size_t n, const char** suffix) {
  *suffix = "";
  if (!ok) {
    snprintf(out, n, "--:--");
    return;
  }
  if (use24h) {
    snprintf(out, n, "%02d:%02d", t.tm_hour, t.tm_min);
    return;
  }
  int h = t.tm_hour % 12;
  if (h == 0) h = 12;
  snprintf(out, n, "%d:%02d", h, t.tm_min);
  *suffix = t.tm_hour >= 12 ? "PM" : "AM";
}

int daysUntil(const tm& now) {
  tm a = now;
  a.tm_hour = 12;
  a.tm_min = 0;
  a.tm_sec = 0;
  a.tm_isdst = 0;
  tm b = {};
  b.tm_year = TARGET_YEAR - 1900;
  b.tm_mon = TARGET_MONTH - 1;
  b.tm_mday = TARGET_DAY;
  b.tm_hour = 12;
  b.tm_isdst = 0;
  return (int)lround(difftime(mktime(&b), mktime(&a)) / 86400.0);
}

void drawDots() {
  for (int i = 0; i < SCR_COUNT; i++) {
    int x = 160 - (SCR_COUNT - 1) * 10 + i * 20;
    if (i == screen) canvas.fillCircle(x, 228, 4, COL_FG);
    else canvas.drawCircle(x, 228, 4, COL_FG);
  }
}

void drawFace(int cx, int cy, uint32_t now) {
  bool blink = (now % 4000) < 250;
  canvas.drawRoundRect(cx - 30, cy - 24, 60, 48, 12, COL_FG);
  if (blink) {
    canvas.drawFastHLine(cx - 18, cy - 6, 12, COL_FG);
    canvas.drawFastHLine(cx + 6, cy - 6, 12, COL_FG);
  } else {
    canvas.fillCircle(cx - 12, cy - 6, 4, COL_FG);
    canvas.fillCircle(cx + 12, cy - 6, 4, COL_FG);
  }
  if ((now / 8000UL) % 2 == 0) {
    canvas.drawLine(cx - 10, cy + 8, cx - 4, cy + 13, COL_FG);
    canvas.drawFastHLine(cx - 4, cy + 13, 8, COL_FG);
    canvas.drawLine(cx + 4, cy + 13, cx + 10, cy + 8, COL_FG);
  } else {
    canvas.drawFastHLine(cx - 8, cy + 11, 16, COL_FG);
  }
}

void drawIdle(const tm& t, bool ok, uint32_t now) {
  uint32_t slot = (now / 20000UL) % 6;
  int cx = 160 + DRIFT_X[slot];
  int cy = 110 + DRIFT_Y[slot];
  char buf[12];
  const char* suffix;
  formatTime(t, ok, buf, sizeof(buf), &suffix);
  put(buf, cx, cy - 75, F_BIG, 1.0f, AL_CENTER);
  drawFace(cx, cy + 40, now);
}

void drawClock(const tm& t, bool ok) {
  char timeStr[12];
  char buf[40];
  const char* suffix;
  formatTime(t, ok, timeStr, sizeof(timeStr), &suffix);
  canvas.setFont(F_BIG);
  canvas.setTextSize(1.5f);
  int w = canvas.textWidth(timeStr);
  int extra = suffix[0] ? 52 : 0;
  int x0 = (320 - w - extra) / 2;
  put(timeStr, x0, 22, F_BIG, 1.5f, AL_LEFT);
  if (suffix[0]) put(suffix, x0 + w + 8, 64, F_MID, 1.0f, AL_LEFT);

  if (ok) {
    snprintf(buf, sizeof(buf), "%s, %d %s", DAYS[t.tm_wday], t.tm_mday, MONTHS[t.tm_mon]);
    put(buf, 160, 112, F_MID, 1.0f, AL_CENTER);
    canvas.drawFastHLine(0, 155, 320, COL_FG);
    canvas.fillRect(0, 154, (t.tm_sec * 320) / 60, 3, COL_FG);
  } else {
    put("Waiting for WiFi", 160, 112, F_MID, 1.0f, AL_CENTER);
  }

  if (wxValid) {
    float temp = wxTemp;
    snprintf(buf, sizeof(buf), "%.0fC %s", temp, weatherText(wxCode));
  } else {
    snprintf(buf, sizeof(buf), "Weather --");
  }
  put(buf, 10, 170, F_SMALL, 1.0f, AL_LEFT);

  if (isnan(roomTemp)) snprintf(buf, sizeof(buf), "Room --");
  else snprintf(buf, sizeof(buf), "Room %.1fC %.0f%%", roomTemp, roomHum);
  put(buf, 310, 170, F_SMALL, 1.0f, AL_RIGHT);

  if (isnan(roomPres)) snprintf(buf, sizeof(buf), "-- hPa");
  else snprintf(buf, sizeof(buf), "%.0f hPa", roomPres);
  put(buf, 10, 192, F_SMALL, 1.0f, AL_LEFT);

  if (ALARM_ON) snprintf(buf, sizeof(buf), "Alarm %02d:%02d", ALARM_HOUR, ALARM_MIN);
  else snprintf(buf, sizeof(buf), "Alarm off");
  put(buf, 310, 192, F_SMALL, 1.0f, AL_RIGHT);
}

void drawPomodoro(uint32_t now) {
  uint32_t rem = pomoRemaining;
  if (pomoState == POMO_RUN) {
    int32_t left = (int32_t)(pomoEndAt - now);
    rem = left > 0 ? (uint32_t)left : 0;
  }
  uint32_t secs = (rem + 999) / 1000;
  uint32_t total = pomoWork ? WORK_MS : BREAK_MS;
  char buf[24];
  put(pomoWork ? "FOCUS" : "BREAK", 160, 12, F_MID, 1.0f, AL_CENTER);
  snprintf(buf, sizeof(buf), "%02u:%02u", (unsigned)(secs / 60), (unsigned)(secs % 60));
  put(buf, 160, 52, F_BIG, 1.6f, AL_CENTER);
  uint32_t spent = total - (rem > total ? total : rem);
  int fill = (int)(280UL * spent / total);
  canvas.drawRect(20, 150, 280, 12, COL_FG);
  canvas.fillRect(20, 150, fill, 12, COL_FG);
  const char* state = pomoState == POMO_RUN ? "running" : (pomoState == POMO_PAUSE ? "paused" : "ready");
  put(state, 160, 172, F_MID, 1.0f, AL_CENTER);
  snprintf(buf, sizeof(buf), "Sessions done: %d", pomoDone);
  put(buf, 160, 198, F_SMALL, 1.0f, AL_CENTER);
}

void drawStopwatch(uint32_t now) {
  uint32_t ms = swElapsed + (swRunning ? now - swStart : 0);
  char buf[24];
  put("STOPWATCH", 160, 12, F_MID, 1.0f, AL_CENTER);
  snprintf(buf, sizeof(buf), "%02u:%02u", (unsigned)((ms / 60000UL) % 100), (unsigned)((ms / 1000UL) % 60));
  put(buf, 160, 52, F_BIG, 1.6f, AL_CENTER);
  snprintf(buf, sizeof(buf), ".%u", (unsigned)((ms / 100UL) % 10));
  put(buf, 160, 135, F_MID, 1.5f, AL_CENTER);
  put(swRunning ? "running" : "stopped", 160, 185, F_MID, 1.0f, AL_CENTER);
}

void drawCountdown(const tm& t, bool ok) {
  char buf[16];
  put(TARGET_LABEL, 160, 14, F_MID, 1.0f, AL_CENTER);
  if (!ok) {
    put("--", 160, 70, F_BIG, 2.0f, AL_CENTER);
    return;
  }
  int d = daysUntil(t);
  if (d < 0) {
    put("Passed", 160, 90, F_MID, 1.5f, AL_CENTER);
  } else if (d == 0) {
    put("TODAY", 160, 90, F_MID, 1.5f, AL_CENTER);
  } else {
    snprintf(buf, sizeof(buf), "%d", d);
    put(buf, 160, 60, F_BIG, 2.0f, AL_CENTER);
    put(d == 1 ? "day left" : "days left", 160, 172, F_MID, 1.0f, AL_CENTER);
  }
}

void drawGithub() {
  char buf[32];
  put("GITHUB STREAK", 160, 14, F_MID, 1.0f, AL_CENTER);
  if (strlen(GH_TOKEN) == 0 || strlen(GH_USER) == 0) {
    put("Add token in code", 160, 100, F_MID, 1.0f, AL_CENTER);
    return;
  }
  if (!ghValid) {
    put("Fetching...", 160, 100, F_MID, 1.0f, AL_CENTER);
    return;
  }
  snprintf(buf, sizeof(buf), "%d", (int)ghStreak);
  put(buf, 160, 56, F_BIG, 2.0f, AL_CENTER);
  put(ghStreak == 1 ? "day" : "days", 160, 160, F_MID, 1.0f, AL_CENTER);
  snprintf(buf, sizeof(buf), "Today: %d", (int)ghToday);
  put(buf, 160, 195, F_SMALL, 1.0f, AL_CENTER);
}

void render(uint32_t now) {
  static uint32_t last = 0;
  uint32_t interval = (screen == SCR_STOPWATCH && swRunning) ? 100 : 250;
  if (now - last < interval) return;
  last = now;
  if (screenOff) return;
  tm t;
  bool ok = getLocalTime(&t, 0);
  canvas.fillScreen(COL_BG);
  bool idle = screen == SCR_CLOCK && now - lastInput > IDLE_MS;
  if (idle) {
    drawIdle(t, ok, now);
  } else {
    switch (screen) {
      case SCR_CLOCK: drawClock(t, ok); break;
      case SCR_POMO: drawPomodoro(now); break;
      case SCR_STOPWATCH: drawStopwatch(now); break;
      case SCR_COUNTDOWN: drawCountdown(t, ok); break;
      case SCR_GITHUB: drawGithub(); break;
      default: break;
    }
    drawDots();
  }
  canvas.pushSprite(0, 0);
}

void pomoToggle(uint32_t now) {
  if (pomoState == POMO_RUN) {
    int32_t left = (int32_t)(pomoEndAt - now);
    pomoRemaining = left > 0 ? (uint32_t)left : 0;
    pomoState = POMO_PAUSE;
  } else {
    pomoEndAt = now + pomoRemaining;
    pomoState = POMO_RUN;
  }
}

void pomoReset() {
  pomoState = POMO_IDLE;
  pomoWork = true;
  pomoRemaining = WORK_MS;
  pomoDone = 0;
}

void updatePomo(uint32_t now) {
  if (pomoState != POMO_RUN) return;
  if ((int32_t)(now - pomoEndAt) < 0) return;
  if (pomoWork) pomoDone++;
  pomoWork = !pomoWork;
  pomoRemaining = pomoWork ? WORK_MS : BREAK_MS;
  pomoEndAt = now + pomoRemaining;
  screen = SCR_POMO;
  lastInput = now;
  wake(now);
  startAlert(3, 2000, 250, 150);
}

void stopwatchToggle(uint32_t now) {
  if (swRunning) {
    swElapsed += now - swStart;
    swRunning = false;
  } else {
    swStart = now;
    swRunning = true;
  }
}

void stopwatchReset() {
  swRunning = false;
  swElapsed = 0;
}

void actionShort(uint32_t now) {
  if (screen == SCR_POMO) pomoToggle(now);
  else if (screen == SCR_STOPWATCH) stopwatchToggle(now);
  else if (screen == SCR_GITHUB) ghForce = true;
}

void actionLong() {
  if (screen == SCR_POMO) pomoReset();
  else if (screen == SCR_STOPWATCH) stopwatchReset();
}

void handleInput(uint32_t now) {
  BtnEvent a = pollButton(btnA, now);
  BtnEvent b = pollButton(btnB, now);
  if (a == EV_NONE && b == EV_NONE) return;
  bool wasOff = screenOff;
  lastInput = now;
  wake(now);
  if (alarmRinging) {
    stopAlert();
    alarmRinging = false;
    return;
  }
  if (wasOff) return;
  if (a == EV_SHORT) screen = (Screen)((screen + 1) % SCR_COUNT);
  else if (a == EV_LONG) use24h = !use24h;
  if (b == EV_SHORT) actionShort(now);
  else if (b == EV_LONG) actionLong();
  if (alertState.remaining == 0) startAlert(1, 3000, 25, 0);
}

void updateAlarm(uint32_t now) {
  static uint32_t last = 0;
  if (alarmRinging && alertState.remaining == 0) alarmRinging = false;
  if (!ALARM_ON) return;
  if (now - last < 500) return;
  last = now;
  tm t;
  if (!getLocalTime(&t, 0)) return;
  if (t.tm_hour == ALARM_HOUR && t.tm_min == ALARM_MIN && t.tm_yday != alarmDay) {
    alarmDay = t.tm_yday;
    alarmRinging = true;
    wake(now);
    startAlert(60, 2400, 300, 300);
  }
}

void updatePir(uint32_t now) {
  if (digitalRead(PIN_PIR) == HIGH) {
    lastMotion = now;
    screenOff = false;
  } else if (!screenOff && now - lastMotion > SCREEN_OFF_MS) {
    screenOff = true;
  }
}

void updateBrightness(uint32_t now) {
  static uint32_t last = 0;
  if (now - last < 400) return;
  last = now;
  if (screenOff) {
    tft.setBrightness(0);
    return;
  }
  float target = 160;
  if (lightOk) {
    float lux = lightMeter.readLightLevel();
    if (lux >= 0) {
      float k = fminf(fmaxf(lux, 2.0f), 150.0f);
      target = 12 + (k - 2.0f) / 148.0f * 243.0f;
    }
  }
  brightness += (target - brightness) * 0.25f;
  tft.setBrightness((uint8_t)brightness);
}

void readRoom() {
  if (!bmeOk) return;
  roomTemp = bme.readTemperature();
  roomHum = bme.readHumidity();
  roomPres = bme.readPressure() / 100.0f;
}

void updateRoom(uint32_t now) {
  static uint32_t last = 0;
  if (now - last < 2000) return;
  last = now;
  readRoom();
}

void updateScreenFlow(uint32_t now) {
  if (screen == SCR_CLOCK) return;
  if (pomoState == POMO_RUN || swRunning) return;
  if (now - lastInput > RETURN_MS) screen = SCR_CLOCK;
}

void setup() {
  pinMode(PIN_BTN_A, INPUT_PULLUP);
  pinMode(PIN_BTN_B, INPUT_PULLUP);
  pinMode(PIN_PIR, INPUT);
  buzzerInit();

  tft.init();
  tft.setRotation(1);
  tft.setBrightness(160);
  tft.fillScreen(COL_BG);
  canvas.setColorDepth(8);
  canvas.createSprite(320, 240);

  Wire.begin(PIN_SDA, PIN_SCL);
  bmeOk = bme.begin(0x76) || bme.begin(0x77);
  lightOk = lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);
  readRoom();

  bootMessage("Connecting to WiFi");
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  uint32_t started = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - started < 15000) delay(250);

  configTzTime("IST-5:30", "pool.ntp.org", "time.nist.gov");
  if (WiFi.status() == WL_CONNECTED) {
    bootMessage("Syncing time");
    tm t;
    getLocalTime(&t, 8000);
  } else {
    bootMessage("No WiFi, will keep trying");
    delay(1500);
  }

  xTaskCreatePinnedToCore(networkTask, "net", 16384, nullptr, 1, nullptr, 0);
  uint32_t now = millis();
  lastInput = now;
  lastMotion = now;
}

void loop() {
  uint32_t now = millis();
  handleInput(now);
  updatePir(now);
  updateBrightness(now);
  updateRoom(now);
  updatePomo(now);
  updateAlarm(now);
  updateAlert(now);
  updateScreenFlow(now);
  maintainWifi(now);
  render(now);
  delay(5);
}
