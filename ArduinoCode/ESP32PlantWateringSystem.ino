// ==============================================================
// PLANT WATERING SYSTEM - PROGRAM CODE
// ORIGIN: https://github.com/Eccodyne/Plant-Watering-System
// ==============================================================

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <EEPROM.h>
#include <RTClib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <stdio.h>
#include <string.h>

// =====================================================
// TRANSLATION STRINGS - CHANGE THESE FOR OTHER LANGUAGES
// =====================================================

static constexpr char TXT_MON[] = "Monday";
static constexpr char TXT_TUE[] = "Tuesday";
static constexpr char TXT_WED[] = "Wednesday";
static constexpr char TXT_THU[] = "Thursday";
static constexpr char TXT_FRI[] = "Friday";
static constexpr char TXT_SAT[] = "Saturday";
static constexpr char TXT_SUN[] = "Sunday";
static constexpr char TXT_UNKNOWN_DAY[] = "?";

static constexpr char TXT_JAN[] = "January";
static constexpr char TXT_FEB[] = "February";
static constexpr char TXT_MAR[] = "March";
static constexpr char TXT_APR[] = "April";
static constexpr char TXT_MAY[] = "May";
static constexpr char TXT_JUN[] = "June";
static constexpr char TXT_JUL[] = "July";
static constexpr char TXT_AUG[] = "August";
static constexpr char TXT_SEP[] = "September";
static constexpr char TXT_OCT[] = "October";
static constexpr char TXT_NOV[] = "November";
static constexpr char TXT_DEC[] = "December";

static constexpr char TXT_SYS_TIME_FORMAT[] = "Time Format: ";
static constexpr char TXT_24H[] = "24h";
static constexpr char TXT_12H[] = "12h (am/pm)";

static constexpr char TXT_APP_TITLE[] = "- Plant Watering System -";
static constexpr char TXT_PUMP_1[] = "Pump 1";
static constexpr char TXT_PUMP_2[] = "Pump 2";
static constexpr char TXT_PRESS_KNOB[] = "Press knob for menu";
static constexpr char TXT_RTC_NOT_FOUND[] = "RTC NOT FOUND";
static constexpr char TXT_ENABLED[] = "Enabled";
static constexpr char TXT_DISABLED[] = "Disabled";
static constexpr char TXT_RUNNING[] = "Running";
static constexpr char TXT_IDLE[] = "Idle";
static constexpr char TXT_P1[] = "P1:";
static constexpr char TXT_P2[] = "  P2:";
static constexpr char TXT_ON[] = "ON ";
static constexpr char TXT_OFF[] = "OFF";
static constexpr char TXT_SLOT_ON[] = "ON ";
static constexpr char TXT_SLOT_OFF[] = "OFF ";
static constexpr char TXT_EDIT[] = "EDIT";
static constexpr char TXT_NAV[] = "NAV";

static constexpr char TXT_MAIN_MENU[] = "Main Menu";
static constexpr char TXT_MENU_PUMP_SETTINGS[] = "Pump Settings";
static constexpr char TXT_MENU_MANUAL[] = "Manual Watering";
static constexpr char TXT_MENU_TIME_SETUP[] = "Set Date/Time";
static constexpr char TXT_MENU_SYSTEM[] = "System";

static constexpr char TXT_PUMP_PREFIX[] = "Pump ";
static constexpr char TXT_ENABLED_COLON[] = "Enabled: ";
static constexpr char TXT_YES[] = "Yes";
static constexpr char TXT_NO[] = "No";
static constexpr char TXT_DURATION_COLON[] = "Duration: ";
static constexpr char TXT_SECONDS_SUFFIX[] = " s";
static constexpr char TXT_SCHEDULE_SLOTS[] = "Schedule Slots";
static constexpr char TXT_RUN_NOW[] = "Run Now";
static constexpr char TXT_STOP_PUMP[] = "Stop Pump";

static constexpr char TXT_SLOTS_SUFFIX[] = " Slots";
static constexpr char TXT_SLOT_PREFIX[] = "Slot ";
static constexpr char TXT_WEEKDAY_COLON[] = "Weekday: ";
static constexpr char TXT_HOUR_COLON[] = "Hour: ";
static constexpr char TXT_MINUTE_COLON[] = "Minute: ";

static constexpr char TXT_YEAR_COLON[] = "Year: ";
static constexpr char TXT_MONTH_COLON[] = "Month: ";
static constexpr char TXT_DAY_COLON[] = "Day: ";
static constexpr char TXT_APPLY_TIME[] = "Apply Time";

static constexpr char TXT_SYS_DEFAULTS[] = "Load Defaults";
static constexpr char TXT_SYS_AUTO_HOME[] = "Auto Home: ";
static constexpr char TXT_SYS_IDLE_TIMER[] = "Display Idle Timer: ";

static constexpr char TXT_MANUAL_RUN_P1[] = "Run Pump 1";
static constexpr char TXT_MANUAL_RUN_P2[] = "Run Pump 2";
static constexpr char TXT_MANUAL_RUN_ALL[] = "Run All";
static constexpr char TXT_MANUAL_STOP_ALL[] = "Stop All";

static constexpr char TXT_RUN_PREFIX[] = "Run: ";
static constexpr char TXT_RUN_SUFFIX[] = "s";

static constexpr const char* WEEKDAY_NAMES[7] = {
  TXT_MON, TXT_TUE, TXT_WED, TXT_THU, TXT_FRI, TXT_SAT, TXT_SUN
};

static constexpr const char* MONTH_NAMES[12] = {
  TXT_JAN, TXT_FEB, TXT_MAR, TXT_APR, TXT_MAY, TXT_JUN,
  TXT_JUL, TXT_AUG, TXT_SEP, TXT_OCT, TXT_NOV, TXT_DEC
};

// =====================================================
// PINS / HW CONFIG
// =====================================================

static constexpr uint8_t TFT_SDA_MOSI = 12;
static constexpr uint8_t TFT_SCLK     = 27;
static constexpr uint8_t TFT_CS       = 5;
static constexpr uint8_t TFT_DC       = 18;
static constexpr uint8_t TFT_RST      = 13;
static constexpr uint8_t TFT_BL       = 19;

static constexpr uint8_t ENC_A        = 35;
static constexpr uint8_t ENC_B        = 36;
static constexpr uint8_t ENC_BTN      = 26;
static constexpr uint8_t AUX_BTN      = 14;

static constexpr uint8_t RELAY1_PIN   = 17;
static constexpr uint8_t RELAY2_PIN   = 16;

static constexpr uint8_t I2C_SDA      = 25;
static constexpr uint8_t I2C_SCL      = 23;

static constexpr size_t EEPROM_SIZE   = 1024;

static constexpr int SCREEN_W         = 320;
static constexpr int SCREEN_H         = 240;

static constexpr uint16_t C_BG        = 0x0841;
static constexpr uint16_t C_PANEL     = 0x10A2;
static constexpr uint16_t C_PANEL2    = 0x18E3;
static constexpr uint16_t C_ACCENT    = 0x07FF;
static constexpr uint16_t C_ACCENT2   = 0xFCE0;
static constexpr uint16_t C_TEXT      = 0xFFFF;
static constexpr uint16_t C_SUBTEXT   = 0xBDF7;
static constexpr uint16_t C_WARN      = 0xFD20;
static constexpr uint16_t C_OK        = 0x07E0;
static constexpr uint16_t C_OFF       = 0x632C;
static constexpr uint16_t C_HILITE    = 0x39E7;
static constexpr uint16_t C_RED       = 0xF800;
static constexpr uint16_t C_GREEN     = 0x07E0;

static constexpr unsigned long INPUT_POLL_MS       = 2;
static constexpr unsigned long RTC_CHECK_MS        = 500;
static constexpr unsigned long LIVE_UI_UPDATE_MS   = 200;
static constexpr unsigned long AUTO_HOME_TIMEOUT_MS= 30000;
static constexpr unsigned long BUTTON_DEBOUNCE_MS  = 25;
static constexpr unsigned long LONG_PRESS_MS       = 700;
static constexpr unsigned long ENCODER_STEP_MS     = 5;

static constexpr int BRIGHT_NORMAL = 255;
static constexpr int BRIGHT_DIM    = 8;

static constexpr uint8_t PUMP_COUNT = 2;
static constexpr uint8_t SLOT_COUNT = 7;

static constexpr int SERIALSPEED = 115200;


// =====================================================
// DISPLAY / RTC
// =====================================================

Adafruit_ST7789 tft = Adafruit_ST7789(&SPI, TFT_CS, TFT_DC, TFT_RST);
RTC_DS3231 rtc;

// =====================================================
// SETTINGS / STATE
// =====================================================

static constexpr uint32_t SETTINGS_MAGIC = 0x504C4E54;
static constexpr uint16_t SETTINGS_VERSION = 5;

enum Weekday : uint8_t {
  WD_MON = 0,
  WD_TUE,
  WD_WED,
  WD_THU,
  WD_FRI,
  WD_SAT,
  WD_SUN
};

struct ScheduleSlot {
  bool enabled;
  uint8_t weekday;
  uint8_t hour;
  uint8_t minute;
};

struct PumpConfig {
  bool enabled;
  uint16_t durationSec;
  ScheduleSlot slots[SLOT_COUNT];
};

struct AppSettings {
  uint32_t magic;
  uint16_t version;
  PumpConfig pumps[PUMP_COUNT];
  bool autoReturnHome;
  uint16_t dimTimeoutSec;
  bool use12hFormat;
};

struct PumpRuntime {
  bool running;
  unsigned long startedAtMs;
  unsigned long stopAtMs;
  bool triggeredThisMinute;
};

struct ButtonState {
  bool raw = false;
  bool stable = false;
  bool lastStable = false;
  unsigned long lastChange = 0;
  bool pressedEvent = false;
  bool releasedEvent = false;
  bool longPressEvent = false;
  bool longPressFired = false;
  unsigned long pressedAt = 0;
};

enum ScreenID {
  SCREEN_HOME,
  SCREEN_MAIN_MENU,
  SCREEN_PUMP_LIST,
  SCREEN_PUMP_EDIT,
  SCREEN_SLOT_LIST,
  SCREEN_SLOT_EDIT,
  SCREEN_MANUAL,
  SCREEN_TIME_SETUP,
  SCREEN_SYSTEM
};

enum PumpEditRow {
  PUMP_ROW_ENABLED,
  PUMP_ROW_DURATION,
  PUMP_ROW_SLOTS,
  PUMP_ROW_RUN,
  PUMP_EDIT_ROW_COUNT
};

enum SlotEditRow {
  SLOT_ROW_ENABLED,
  SLOT_ROW_WEEKDAY,
  SLOT_ROW_HOUR,
  SLOT_ROW_MINUTE,
  SLOT_EDIT_ROW_COUNT
};

enum TimeEditRow {
  TIME_ROW_YEAR,
  TIME_ROW_MONTH,
  TIME_ROW_DAY,
  TIME_ROW_HOUR,
  TIME_ROW_MINUTE,
  TIME_ROW_APPLY,
  TIME_EDIT_ROW_COUNT
};

enum SystemRow {
  SYSTEM_ROW_DIM_TIMEOUT,
  SYSTEM_ROW_DEFAULTS,
  SYSTEM_ROW_AUTO_HOME,
  SYSTEM_ROW_TIME_FORMAT,
  SYSTEM_ROW_COUNT
};

AppSettings settings;
AppSettings lastSavedSettings;
PumpRuntime pumpRt[PUMP_COUNT];
ButtonState encBtnState;
ButtonState auxBtnState;

bool rtcAvailable = false;
bool needFullRedraw = true;
bool homeScreenInitialized = false;
bool isScreenDimmed = false;
bool settingsDirty = false;

ScreenID currentScreen = SCREEN_HOME;

int encoderDelta = 0;
int lastEncA = 0;
int lastEncB = 0;
int menuIndex = 0;
int selectedPump = 0;
int selectedSlot = 0;
bool editMode = false;

unsigned long lastInputPoll = 0;
unsigned long lastRtcCheck = 0;
unsigned long lastUiUpdate = 0;
unsigned long lastInteractionMs = 0;
unsigned long lastEncMoveMs = 0;

char lastClockText[20] = "";
char lastDateText[40] = "";
bool lastPump1Enabled = false;
bool lastPump2Enabled = false;
bool lastPump1Running = false;
bool lastPump2Running = false;
int lastPump1Sec = -1;
int lastPump2Sec = -1;
char lastStatusBarText[24] = "";

int editYear = 2025;
int editMonth = 1;
int editDay = 1;
int editHour = 12;
int editMinute = 0;

// =====================================================
// SMALL HELPERS
// =====================================================

static inline void markSettingsDirty() {
  settingsDirty = true;
}

static inline unsigned long nowMs() {
  return millis();
}

void copyText(char* dst, size_t dstSize, const char* src) {
  if (dstSize == 0) return;
  snprintf(dst, dstSize, "%s", src ? src : "");
}

void formatTwoDigits(char* out, size_t outSize, int value) {
  snprintf(out, outSize, "%02d", value);
}

void formatHourOnly(char* out, size_t outSize, int h) {
  if (settings.use12hFormat) {
    const int h12 = (h % 12 == 0) ? 12 : (h % 12);
    snprintf(out, outSize, "%d%s", h12, h >= 12 ? " PM" : " AM");
  } else {
    snprintf(out, outSize, "%d", h);
  }
}

void formatTimeHM(char* out, size_t outSize, int h, int m) {
  if (settings.use12hFormat) {
    const int h12 = (h % 12 == 0) ? 12 : (h % 12);
    snprintf(out, outSize, "%d:%02d%s", h12, m, h >= 12 ? " PM" : " AM");
  } else {
    snprintf(out, outSize, "%02d:%02d", h, m);
  }
}

const char* monthNameFromRTC(uint8_t m) {
  return (m >= 1 && m <= 12) ? MONTH_NAMES[m - 1] : TXT_UNKNOWN_DAY;
}

const char* weekdayNameFromRTClib(uint8_t dow) {
  switch (dow) {
    case 0: return TXT_SUN;
    case 1: return TXT_MON;
    case 2: return TXT_TUE;
    case 3: return TXT_WED;
    case 4: return TXT_THU;
    case 5: return TXT_FRI;
    case 6: return TXT_SAT;
    default: return TXT_UNKNOWN_DAY;
  }
}

uint8_t rtcDowToAppWeekday(uint8_t rtcDow) {
  switch (rtcDow) {
    case 1: return WD_MON;
    case 2: return WD_TUE;
    case 3: return WD_WED;
    case 4: return WD_THU;
    case 5: return WD_FRI;
    case 6: return WD_SAT;
    case 0: return WD_SUN;
    default: return WD_MON;
  }
}

int daysInMonth(int year, int month) {
  if (month == 2) {
    const bool leap = ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0));
    return leap ? 29 : 28;
  }
  if (month == 4 || month == 6 || month == 9 || month == 11) return 30;
  return 31;
}

void clampTimeEditorDate() {
  if (editMonth < 1) editMonth = 1;
  if (editMonth > 12) editMonth = 12;

  const int dim = daysInMonth(editYear, editMonth);
  if (editDay < 1) editDay = dim;
  if (editDay > dim) editDay = 1;
  if (editHour < 0) editHour = 23;
  if (editHour > 23) editHour = 0;
  if (editMinute < 0) editMinute = 59;
  if (editMinute > 59) editMinute = 0;
}

bool isValidSettings(const AppSettings& s) {
  if (s.magic != SETTINGS_MAGIC || s.version != SETTINGS_VERSION) return false;
  if (s.dimTimeoutSec < 5 || s.dimTimeoutSec > 300) return false;

  for (uint8_t p = 0; p < PUMP_COUNT; ++p) {
    if (s.pumps[p].durationSec < 1 || s.pumps[p].durationSec > 999) return false;
    for (uint8_t i = 0; i < SLOT_COUNT; ++i) {
      const ScheduleSlot& slot = s.pumps[p].slots[i];
      if (slot.weekday > 6 || slot.hour > 23 || slot.minute > 59) return false;
    }
  }
  return true;
}

// =====================================================
// EEPROM / SETTINGS
// =====================================================

void setDefaults() {
  settings.magic = SETTINGS_MAGIC;
  settings.version = SETTINGS_VERSION;
  settings.autoReturnHome = true;
  settings.dimTimeoutSec = 30;
  settings.use12hFormat = false;

  for (uint8_t p = 0; p < PUMP_COUNT; ++p) {
    settings.pumps[p].enabled = true;
    settings.pumps[p].durationSec = 5;
    for (uint8_t s = 0; s < SLOT_COUNT; ++s) {
      settings.pumps[p].slots[s].enabled = false;
      settings.pumps[p].slots[s].weekday = s;
      settings.pumps[p].slots[s].hour = 8;
      settings.pumps[p].slots[s].minute = 0;
    }
  }
  markSettingsDirty();
}

void saveSettingsIfNeeded() {
  if (!settingsDirty) return;
  if (memcmp(&settings, &lastSavedSettings, sizeof(AppSettings)) == 0) {
    settingsDirty = false;
    return;
  }

  EEPROM.put(0, settings);
  EEPROM.commit();
  lastSavedSettings = settings;
  settingsDirty = false;
}

bool loadSettings() {
  EEPROM.get(0, settings);

  if (!isValidSettings(settings)) {
    setDefaults();
    memset(&lastSavedSettings, 0, sizeof(AppSettings));
    saveSettingsIfNeeded();
    return false;
  }

  lastSavedSettings = settings;
  settingsDirty = false;
  return true;
}

// =====================================================
// PUMP CONTROL
// =====================================================

void setPumpOutput(int pump, bool on) {
  const int pin = (pump == 0) ? RELAY1_PIN : RELAY2_PIN;
  digitalWrite(pin, on ? HIGH : LOW);
}

void startPump(int pump) {
  if (pump < 0 || pump >= PUMP_COUNT) return;
  if (!settings.pumps[pump].enabled) return;

  const unsigned long ms = nowMs();
  pumpRt[pump].running = true;
  pumpRt[pump].startedAtMs = ms;
  pumpRt[pump].stopAtMs = ms + (static_cast<unsigned long>(settings.pumps[pump].durationSec) * 1000UL);
  setPumpOutput(pump, true);
  needFullRedraw = true;
}

void stopPump(int pump) {
  if (pump < 0 || pump >= PUMP_COUNT) return;

  pumpRt[pump].running = false;
  setPumpOutput(pump, false);
  needFullRedraw = true;
}

void updatePumps() {
  const unsigned long ms = nowMs();
  for (uint8_t p = 0; p < PUMP_COUNT; ++p) {
    if (pumpRt[p].running && ms >= pumpRt[p].stopAtMs) {
      stopPump(p);
    }
  }
}

int pumpRemainingSec(uint8_t pump, unsigned long ms) {
  if (!pumpRt[pump].running) return 0;
  if (ms >= pumpRt[pump].stopAtMs) return 0;
  return static_cast<int>((pumpRt[pump].stopAtMs - ms) / 1000UL);
}

// =====================================================
// INPUT HANDLING
// =====================================================

void updateButton(ButtonState& btn, bool rawLevel) {
  const unsigned long ms = nowMs();
  btn.raw = rawLevel;

  if (btn.raw != btn.lastStable && (ms - btn.lastChange > BUTTON_DEBOUNCE_MS)) {
    btn.lastChange = ms;
    btn.lastStable = btn.raw;
    btn.stable = btn.raw;

    if (btn.stable) {
      btn.pressedEvent = true;
      btn.pressedAt = ms;
      btn.longPressFired = false;
    } else {
      btn.releasedEvent = true;
    }
  }

  if (btn.stable && !btn.longPressFired && (ms - btn.pressedAt > LONG_PRESS_MS)) {
    btn.longPressEvent = true;
    btn.longPressFired = true;
  }
}

void clearButtonEvents(ButtonState& btn) {
  btn.pressedEvent = false;
  btn.releasedEvent = false;
  btn.longPressEvent = false;
}

void updateEncoder() {
  const unsigned long ms = nowMs();
  const int a = digitalRead(ENC_A);
  const int b = digitalRead(ENC_B);

  if (a != lastEncA && a == LOW) {
    if (ms - lastEncMoveMs > ENCODER_STEP_MS) {
      if (a == b) ++encoderDelta;
      else --encoderDelta;
      lastEncMoveMs = ms;
    }
  }

  lastEncA = a;
  lastEncB = b;
}

void pollInputs() {
  const unsigned long ms = nowMs();
  if (ms - lastInputPoll < INPUT_POLL_MS) return;
  lastInputPoll = ms;

  updateEncoder();
  updateButton(encBtnState, digitalRead(ENC_BTN) == LOW);
  updateButton(auxBtnState, digitalRead(AUX_BTN) == LOW);
}

void noteInteraction() {
  lastInteractionMs = nowMs();
  if (isScreenDimmed) {
    analogWrite(TFT_BL, BRIGHT_NORMAL);
    isScreenDimmed = false;
  }
}

// =====================================================
// RTC / SCHEDULING
// =====================================================

bool slotMatchesNow(const ScheduleSlot& slot, const DateTime& now) {
  if (!slot.enabled) return false;
  const uint8_t currentWd = rtcDowToAppWeekday(now.dayOfTheWeek());
  return (slot.weekday == currentWd && slot.hour == now.hour() && slot.minute == now.minute());
}

void checkSchedules() {
  const unsigned long ms = nowMs();
  if (!rtcAvailable) return;
  if (ms - lastRtcCheck < RTC_CHECK_MS) return;
  lastRtcCheck = ms;

  const DateTime now = rtc.now();

  for (uint8_t p = 0; p < PUMP_COUNT; ++p) {
    bool anyMatch = false;
    for (uint8_t s = 0; s < SLOT_COUNT; ++s) {
      if (settings.pumps[p].enabled && slotMatchesNow(settings.pumps[p].slots[s], now)) {
        anyMatch = true;
      }
    }

    if (anyMatch) {
      if (!pumpRt[p].triggeredThisMinute) {
        startPump(p);
        pumpRt[p].triggeredThisMinute = true;
      }
    } else {
      pumpRt[p].triggeredThisMinute = false;
    }
  }
}

// =====================================================
// DRAW HELPERS
// =====================================================

void fillScreenBg() {
  tft.fillScreen(C_BG);
}

void drawCard(int x, int y, int w, int h, uint16_t color, bool selected = false) {
  tft.fillRoundRect(x, y, w, h, 8, color);
  tft.drawRoundRect(x, y, w, h, 8, selected ? C_ACCENT : C_HILITE);
}

void drawCenteredText(const char* txt, int y, int size, uint16_t color) {
  int16_t x1, y1;
  uint16_t w, h;
  tft.setTextSize(size);
  tft.getTextBounds(const_cast<char*>(txt), 0, y, &x1, &y1, &w, &h);
  const int x = (SCREEN_W - static_cast<int>(w)) / 2;
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.print(txt);
}

void drawTopBarStatic(const char* title) {
  tft.fillRect(0, 0, SCREEN_W, 28, C_PANEL2);
  tft.drawFastHLine(0, 28, SCREEN_W, C_HILITE);

  if (strcmp(title, TXT_APP_TITLE) == 0) {
    drawCenteredText(title, 7, 2, C_GREEN);
  } else {
    tft.setTextColor(C_ACCENT2);
    tft.setTextSize(2);
    tft.setCursor(8, 7);
    tft.print(title);
  }
}

void drawStatusBarFrame() {
  tft.fillRect(0, SCREEN_H - 18, SCREEN_W, 18, C_PANEL2);
  tft.drawFastHLine(0, SCREEN_H - 18, SCREEN_W, C_HILITE);
}

void updateStatusBarText() {
  char txt[24];
  snprintf(txt, sizeof(txt), "%s%s%s%s  %s",
           TXT_P1, pumpRt[0].running ? TXT_ON : TXT_OFF,
           TXT_P2, pumpRt[1].running ? TXT_ON : TXT_OFF,
           editMode ? TXT_EDIT : TXT_NAV);

  if (strcmp(txt, lastStatusBarText) == 0) return;

  tft.fillRect(0, SCREEN_H - 17, SCREEN_W, 17, C_PANEL2);
  tft.setTextSize(1);
  tft.setCursor(6, SCREEN_H - 13);

  tft.setTextColor(C_SUBTEXT);
  tft.print(TXT_P1);
  tft.setTextColor(pumpRt[0].running ? C_OK : C_OFF);
  tft.print(pumpRt[0].running ? TXT_ON : TXT_OFF);

  tft.setTextColor(C_SUBTEXT);
  tft.print(TXT_P2);
  tft.setTextColor(pumpRt[1].running ? C_OK : C_OFF);
  tft.print(pumpRt[1].running ? TXT_ON : TXT_OFF);

  tft.setTextColor(C_SUBTEXT);
  tft.print("  ");
  tft.print(editMode ? TXT_EDIT : TXT_NAV);

  copyText(lastStatusBarText, sizeof(lastStatusBarText), txt);
}

void invalidateHomeCache() {
  lastClockText[0] = '\0';
  lastDateText[0] = '\0';
  lastPump1Enabled = !settings.pumps[0].enabled;
  lastPump2Enabled = !settings.pumps[1].enabled;
  lastPump1Running = !pumpRt[0].running;
  lastPump2Running = !pumpRt[1].running;
  lastPump1Sec = -1;
  lastPump2Sec = -1;
  lastStatusBarText[0] = '\0';
}

void drawHomeStatic() {
  fillScreenBg();
  drawTopBarStatic(TXT_APP_TITLE);

  const int margin = 10;
  const int gap = 10;
  const int fullW = SCREEN_W - margin * 2;
  const int halfW = (fullW - gap) / 2;

  drawCard(margin, 40, fullW, 58, C_PANEL2, false);
  drawCard(margin, 108, halfW, 70, C_PANEL, false);
  drawCard(margin + halfW + gap, 108, halfW, 70, C_PANEL, false);
  drawCard(margin, 188, fullW, 28, C_PANEL2, true);

  tft.setTextSize(2);
  tft.setTextColor(C_TEXT);
  tft.setCursor(margin + 10, 118);
  tft.print(TXT_PUMP_1);
  tft.setCursor(margin + halfW + gap + 10, 118);
  tft.print(TXT_PUMP_2);

  drawCenteredText(TXT_PRESS_KNOB, 194, 2, C_ACCENT);
  drawStatusBarFrame();
  homeScreenInitialized = true;
  invalidateHomeCache();
}

void updateHomeDynamic() {
  if (!homeScreenInitialized) return;

  const int margin = 10;
  const int gap = 10;
  const int fullW = SCREEN_W - margin * 2;
  const int halfW = (fullW - gap) / 2;
  const unsigned long ms = nowMs();

  if (rtcAvailable) {
    const DateTime now = rtc.now();
    char clockText[20];
    char dateText[40];
    formatTimeHM(clockText, sizeof(clockText), now.hour(), now.minute());
    snprintf(dateText, sizeof(dateText), "%s, %d %s %d",
             weekdayNameFromRTClib(now.dayOfTheWeek()),
             now.day(), monthNameFromRTC(now.month()), now.year());

    if (strcmp(clockText, lastClockText) != 0) {
      tft.fillRect(margin + 6, 48, fullW - 12, 24, C_PANEL2);
      drawCenteredText(clockText, 50, 3, C_WARN);
      copyText(lastClockText, sizeof(lastClockText), clockText);
    }

    if (strcmp(dateText, lastDateText) != 0) {
      tft.fillRect(margin + 6, 76, fullW - 12, 12, C_PANEL2);
      drawCenteredText(dateText, 78, 1, C_SUBTEXT);
      copyText(lastDateText, sizeof(lastDateText), dateText);
    }
  } else {
    tft.fillRect(margin + 6, 56, fullW - 12, 16, C_PANEL2);
    drawCenteredText(TXT_RTC_NOT_FOUND, 60, 2, C_WARN);
  }

  if (settings.pumps[0].enabled != lastPump1Enabled) {
    tft.fillRect(margin + 10, 145, halfW - 20, 10, C_PANEL);
    tft.setTextSize(1);
    tft.setTextColor(settings.pumps[0].enabled ? C_OK : C_WARN);
    tft.setCursor(margin + 10, 145);
    tft.print(settings.pumps[0].enabled ? TXT_ENABLED : TXT_DISABLED);
    lastPump1Enabled = settings.pumps[0].enabled;
  }

  const int p1Sec = pumpRemainingSec(0, ms);
  if (pumpRt[0].running != lastPump1Running || (pumpRt[0].running && p1Sec != lastPump1Sec)) {
    tft.fillRect(margin + 10, 158, halfW - 20, 10, C_PANEL);
    tft.setTextSize(1);
    tft.setTextColor(pumpRt[0].running ? C_ACCENT2 : C_SUBTEXT);
    tft.setCursor(margin + 10, 158);
    if (pumpRt[0].running) {
      tft.print(TXT_RUN_PREFIX);
      tft.print(p1Sec);
      tft.print(TXT_RUN_SUFFIX);
    } else {
      tft.print(TXT_IDLE);
    }
    lastPump1Running = pumpRt[0].running;
    lastPump1Sec = p1Sec;
  }

  const int p2x = margin + halfW + gap;
  if (settings.pumps[1].enabled != lastPump2Enabled) {
    tft.fillRect(p2x + 10, 145, halfW - 20, 10, C_PANEL);
    tft.setTextSize(1);
    tft.setTextColor(settings.pumps[1].enabled ? C_OK : C_WARN);
    tft.setCursor(p2x + 10, 145);
    tft.print(settings.pumps[1].enabled ? TXT_ENABLED : TXT_DISABLED);
    lastPump2Enabled = settings.pumps[1].enabled;
  }

  const int p2Sec = pumpRemainingSec(1, ms);
  if (pumpRt[1].running != lastPump2Running || (pumpRt[1].running && p2Sec != lastPump2Sec)) {
    tft.fillRect(p2x + 10, 158, halfW - 20, 10, C_PANEL);
    tft.setTextSize(1);
    tft.setTextColor(pumpRt[1].running ? C_ACCENT2 : C_SUBTEXT);
    tft.setCursor(p2x + 10, 158);
    if (pumpRt[1].running) {
      tft.print(TXT_RUN_PREFIX);
      tft.print(p2Sec);
      tft.print(TXT_RUN_SUFFIX);
    } else {
      tft.print(TXT_IDLE);
    }
    lastPump2Running = pumpRt[1].running;
    lastPump2Sec = p2Sec;
  }

  updateStatusBarText();
}

void drawHomeScreen() {
  drawHomeStatic();
  updateHomeDynamic();
}

void drawMainMenu() {
  static constexpr const char* items[] = {
    TXT_MENU_PUMP_SETTINGS, TXT_MENU_MANUAL, TXT_MENU_TIME_SETUP, TXT_MENU_SYSTEM
  };

  fillScreenBg();
  drawTopBarStatic(TXT_MAIN_MENU);

  const int startY = 40;
  for (int i = 0; i < 4; ++i) {
    const bool sel = (menuIndex == i);
    drawCard(10, startY + i * 36, SCREEN_W - 20, 30, sel ? C_PANEL2 : C_PANEL, sel);
    tft.setTextSize(2);
    tft.setTextColor(sel ? C_ACCENT : C_TEXT);
    tft.setCursor(18, startY + 7 + i * 36);
    tft.print(items[i]);
  }

  drawStatusBarFrame();
  lastStatusBarText[0] = '\0';
  updateStatusBarText();
}

void drawPumpList() {
  fillScreenBg();
  drawTopBarStatic(TXT_MENU_PUMP_SETTINGS);

  for (int i = 0; i < PUMP_COUNT; ++i) {
    const bool sel = (menuIndex == i);
    drawCard(10, 50 + i * 60, SCREEN_W - 20, 50, sel ? C_PANEL2 : C_PANEL, sel);

    tft.setTextSize(2);
    tft.setTextColor(sel ? C_ACCENT : C_TEXT);
    tft.setCursor(18, 58 + i * 60);
    tft.print(TXT_PUMP_PREFIX);
    tft.print(i + 1);

    tft.setTextSize(1);
    tft.setTextColor(C_SUBTEXT);
    tft.setCursor(18, 80 + i * 60);
    tft.print(settings.pumps[i].enabled ? TXT_ON : TXT_OFF);
  }

  drawStatusBarFrame();
  lastStatusBarText[0] = '\0';
  updateStatusBarText();
}

void drawPumpEdit() {
  fillScreenBg();

  char title[16];
  snprintf(title, sizeof(title), "%s%d", TXT_PUMP_PREFIX, selectedPump + 1);
  drawTopBarStatic(title);

  PumpConfig& p = settings.pumps[selectedPump];
  char rows[PUMP_EDIT_ROW_COUNT][40];
  snprintf(rows[PUMP_ROW_ENABLED], sizeof(rows[PUMP_ROW_ENABLED]), "%s%s", TXT_ENABLED_COLON, p.enabled ? TXT_YES : TXT_NO);
  snprintf(rows[PUMP_ROW_DURATION], sizeof(rows[PUMP_ROW_DURATION]), "%s%u%s", TXT_DURATION_COLON, p.durationSec, TXT_SECONDS_SUFFIX);
  snprintf(rows[PUMP_ROW_SLOTS], sizeof(rows[PUMP_ROW_SLOTS]), "%s", TXT_SCHEDULE_SLOTS);
  snprintf(rows[PUMP_ROW_RUN], sizeof(rows[PUMP_ROW_RUN]), "%s", pumpRt[selectedPump].running ? TXT_STOP_PUMP : TXT_RUN_NOW);

  const int startY = 40;
  for (int i = 0; i < PUMP_EDIT_ROW_COUNT; ++i) {
    const bool sel = (menuIndex == i);
    drawCard(10, startY + i * 40, SCREEN_W - 20, 30, sel ? C_PANEL2 : C_PANEL, sel);
    tft.setTextSize(2);
    tft.setTextColor(sel ? (editMode ? C_ACCENT2 : C_ACCENT) : C_TEXT);
    tft.setCursor(18, startY + 7 + i * 40);
    tft.print(rows[i]);
  }

  drawStatusBarFrame();
  lastStatusBarText[0] = '\0';
  updateStatusBarText();
}

void drawSlotList() {
  fillScreenBg();

  char title[24];
  snprintf(title, sizeof(title), "%s%d%s", TXT_PUMP_PREFIX, selectedPump + 1, TXT_SLOTS_SUFFIX);
  drawTopBarStatic(title);

  const int visibleItems = 3;
  int windowStart = menuIndex;
  if (windowStart > SLOT_COUNT - visibleItems) {
    windowStart = SLOT_COUNT - visibleItems;
  }

  for (int i = windowStart; i < windowStart + visibleItems; ++i) {
    if (i >= SLOT_COUNT) break;

    const int displayPos = i - windowStart;
    ScheduleSlot& s = settings.pumps[selectedPump].slots[i];
    const bool sel = (menuIndex == i);

    drawCard(10, 40 + displayPos * 50, SCREEN_W - 20, 40, sel ? C_PANEL2 : C_PANEL, sel);

    tft.setTextSize(2);
    tft.setTextColor(sel ? C_ACCENT : C_TEXT);
    tft.setCursor(18, 49 + displayPos * 50);
    tft.print(TXT_SLOT_PREFIX);
    tft.print(i + 1);

    char timeText[20];
    char rowText[40];
    formatTimeHM(timeText, sizeof(timeText), s.hour, s.minute);
    snprintf(rowText, sizeof(rowText), "%s%s  %s",
             s.enabled ? TXT_SLOT_ON : TXT_SLOT_OFF,
             WEEKDAY_NAMES[s.weekday], timeText);

    tft.setTextSize(1);
    tft.setTextColor(C_GREEN);
    tft.setCursor(18, 68 + displayPos * 50);
    tft.print(rowText);
  }

  drawStatusBarFrame();
  lastStatusBarText[0] = '\0';
  updateStatusBarText();
}

void drawSlotEdit() {
  fillScreenBg();

  char title[16];
  snprintf(title, sizeof(title), "%s%d", TXT_SLOT_PREFIX, selectedSlot + 1);
  drawTopBarStatic(title);

  ScheduleSlot& s = settings.pumps[selectedPump].slots[selectedSlot];
  char hourText[16];
  char minuteText[8];
  char rows[SLOT_EDIT_ROW_COUNT][40];

  formatHourOnly(hourText, sizeof(hourText), s.hour);
  formatTwoDigits(minuteText, sizeof(minuteText), s.minute);

  snprintf(rows[SLOT_ROW_ENABLED], sizeof(rows[SLOT_ROW_ENABLED]), "%s%s", TXT_ENABLED_COLON, s.enabled ? TXT_YES : TXT_NO);
  snprintf(rows[SLOT_ROW_WEEKDAY], sizeof(rows[SLOT_ROW_WEEKDAY]), "%s%s", TXT_WEEKDAY_COLON, WEEKDAY_NAMES[s.weekday]);
  snprintf(rows[SLOT_ROW_HOUR], sizeof(rows[SLOT_ROW_HOUR]), "%s%s", TXT_HOUR_COLON, hourText);
  snprintf(rows[SLOT_ROW_MINUTE], sizeof(rows[SLOT_ROW_MINUTE]), "%s%s", TXT_MINUTE_COLON, minuteText);

  for (int i = 0; i < SLOT_EDIT_ROW_COUNT; ++i) {
    const bool sel = (menuIndex == i);
    drawCard(10, 45 + i * 40, SCREEN_W - 20, 30, sel ? C_PANEL2 : C_PANEL, sel);
    tft.setTextSize(2);
    tft.setTextColor(sel ? (editMode ? C_ACCENT2 : C_ACCENT) : C_TEXT);
    tft.setCursor(18, 52 + i * 40);
    tft.print(rows[i]);
  }

  drawStatusBarFrame();
  lastStatusBarText[0] = '\0';
  updateStatusBarText();
}

void drawTimeSetup() {
  fillScreenBg();
  drawTopBarStatic(TXT_MENU_TIME_SETUP);

  char hourText[16];
  char minuteText[8];
  char rows[TIME_EDIT_ROW_COUNT][40];

  formatHourOnly(hourText, sizeof(hourText), editHour);
  formatTwoDigits(minuteText, sizeof(minuteText), editMinute);

  snprintf(rows[TIME_ROW_YEAR], sizeof(rows[TIME_ROW_YEAR]), "%s%d", TXT_YEAR_COLON, editYear);
  snprintf(rows[TIME_ROW_MONTH], sizeof(rows[TIME_ROW_MONTH]), "%s%d", TXT_MONTH_COLON, editMonth);
  snprintf(rows[TIME_ROW_DAY], sizeof(rows[TIME_ROW_DAY]), "%s%d", TXT_DAY_COLON, editDay);
  snprintf(rows[TIME_ROW_HOUR], sizeof(rows[TIME_ROW_HOUR]), "%s%s", TXT_HOUR_COLON, hourText);
  snprintf(rows[TIME_ROW_MINUTE], sizeof(rows[TIME_ROW_MINUTE]), "%s%s", TXT_MINUTE_COLON, minuteText);
  snprintf(rows[TIME_ROW_APPLY], sizeof(rows[TIME_ROW_APPLY]), "%s", TXT_APPLY_TIME);

  for (int i = 0; i < TIME_EDIT_ROW_COUNT; ++i) {
    const bool sel = (menuIndex == i);
    drawCard(10, 35 + i * 31, SCREEN_W - 20, 25, sel ? C_PANEL2 : C_PANEL, sel);
    tft.setTextSize(2);
    tft.setTextColor(sel ? (editMode ? C_ACCENT2 : C_ACCENT) : C_TEXT);
    tft.setCursor(18, 39 + i * 31);
    tft.print(rows[i]);
  }

  drawStatusBarFrame();
  lastStatusBarText[0] = '\0';
  updateStatusBarText();
}

void drawManualScreen() {
  static constexpr const char* items[] = {
    TXT_MANUAL_RUN_P1, TXT_MANUAL_RUN_P2, TXT_MANUAL_RUN_ALL, TXT_MANUAL_STOP_ALL
  };

  fillScreenBg();
  drawTopBarStatic(TXT_MENU_MANUAL);

  for (int i = 0; i < 4; ++i) {
    const bool sel = (menuIndex == i);
    drawCard(10, 50 + i * 42, SCREEN_W - 20, 30, sel ? C_PANEL2 : C_PANEL, sel);
    tft.setTextSize(2);
    tft.setTextColor(sel ? C_ACCENT : C_TEXT);
    tft.setCursor(18, 57 + i * 42);
    tft.print(items[i]);
  }

  drawStatusBarFrame();
  lastStatusBarText[0] = '\0';
  updateStatusBarText();
}

void drawSystemScreen() {
  fillScreenBg();
  drawTopBarStatic(TXT_MENU_SYSTEM);

  char rows[SYSTEM_ROW_COUNT][44];
  snprintf(rows[SYSTEM_ROW_DIM_TIMEOUT], sizeof(rows[SYSTEM_ROW_DIM_TIMEOUT]), "%s%us", TXT_SYS_IDLE_TIMER, settings.dimTimeoutSec);
  snprintf(rows[SYSTEM_ROW_DEFAULTS], sizeof(rows[SYSTEM_ROW_DEFAULTS]), "%s", TXT_SYS_DEFAULTS);
  snprintf(rows[SYSTEM_ROW_AUTO_HOME], sizeof(rows[SYSTEM_ROW_AUTO_HOME]), "%s%s", TXT_SYS_AUTO_HOME, settings.autoReturnHome ? TXT_YES : TXT_NO);
  snprintf(rows[SYSTEM_ROW_TIME_FORMAT], sizeof(rows[SYSTEM_ROW_TIME_FORMAT]), "%s%s", TXT_SYS_TIME_FORMAT, settings.use12hFormat ? TXT_12H : TXT_24H);

  for (int i = 0; i < SYSTEM_ROW_COUNT; ++i) {
    const bool sel = (menuIndex == i);
    drawCard(10, 45 + i * 40, SCREEN_W - 20, 30, sel ? C_PANEL2 : C_PANEL, sel);
    tft.setTextSize(2);
    tft.setTextColor(sel ? (editMode ? C_ACCENT2 : C_ACCENT) : C_TEXT);
    tft.setCursor(18, 52 + i * 40);
    tft.print(rows[i]);
  }

  drawStatusBarFrame();
  lastStatusBarText[0] = '\0';
  updateStatusBarText();
}

void drawCurrentScreen() {
  switch (currentScreen) {
    case SCREEN_HOME:       drawHomeScreen(); break;
    case SCREEN_MAIN_MENU:  drawMainMenu(); break;
    case SCREEN_PUMP_LIST:  drawPumpList(); break;
    case SCREEN_PUMP_EDIT:  drawPumpEdit(); break;
    case SCREEN_SLOT_LIST:  drawSlotList(); break;
    case SCREEN_SLOT_EDIT:  drawSlotEdit(); break;
    case SCREEN_MANUAL:     drawManualScreen(); break;
    case SCREEN_TIME_SETUP: drawTimeSetup(); break;
    case SCREEN_SYSTEM:     drawSystemScreen(); break;
  }
  needFullRedraw = false;
}

// =====================================================
// NAVIGATION
// =====================================================

void beginTimeEditing() {
  if (rtcAvailable) {
    const DateTime now = rtc.now();
    editYear = now.year();
    editMonth = now.month();
    editDay = now.day();
    editHour = now.hour();
    editMinute = now.minute();
  }
}

int maxItemsForScreen(ScreenID screen) {
  switch (screen) {
    case SCREEN_MAIN_MENU:  return 4;
    case SCREEN_PUMP_LIST:  return PUMP_COUNT;
    case SCREEN_PUMP_EDIT:  return PUMP_EDIT_ROW_COUNT;
    case SCREEN_SLOT_LIST:  return SLOT_COUNT;
    case SCREEN_SLOT_EDIT:  return SLOT_EDIT_ROW_COUNT;
    case SCREEN_MANUAL:     return 4;
    case SCREEN_TIME_SETUP: return TIME_EDIT_ROW_COUNT;
    case SCREEN_SYSTEM:     return SYSTEM_ROW_COUNT;
    default:                return 0;
  }
}

void moveSelection(int delta) {
  if (!editMode) {
    const int maxItems = maxItemsForScreen(currentScreen);
    if (maxItems > 0) {
      menuIndex += delta;
      if (menuIndex < 0) menuIndex = maxItems - 1;
      if (menuIndex >= maxItems) menuIndex = 0;
      needFullRedraw = true;
    }
    return;
  }

  if (currentScreen == SCREEN_SLOT_EDIT) {
    ScheduleSlot& s = settings.pumps[selectedPump].slots[selectedSlot];
    switch (menuIndex) {
      case SLOT_ROW_ENABLED:
        if (delta != 0) s.enabled = !s.enabled;
        break;
      case SLOT_ROW_WEEKDAY: {
        int w = static_cast<int>(s.weekday) + delta;
        if (w < 0) w = 6;
        if (w > 6) w = 0;
        s.weekday = w;
        break;
      }
      case SLOT_ROW_HOUR: {
        int h = static_cast<int>(s.hour) + delta;
        if (h < 0) h = 23;
        if (h > 23) h = 0;
        s.hour = h;
        break;
      }
      case SLOT_ROW_MINUTE: {
        int m = static_cast<int>(s.minute) + delta;
        if (m < 0) m = 59;
        if (m > 59) m = 0;
        s.minute = m;
        break;
      }
    }
    markSettingsDirty();
    needFullRedraw = true;
  }

  if (currentScreen == SCREEN_PUMP_EDIT) {
    PumpConfig& p = settings.pumps[selectedPump];
    switch (menuIndex) {
      case PUMP_ROW_ENABLED:
        if (delta != 0) p.enabled = !p.enabled;
        break;
      case PUMP_ROW_DURATION: {
        int v = static_cast<int>(p.durationSec) + delta;
        if (v < 1) v = 1;
        if (v > 999) v = 999;
        p.durationSec = v;
        break;
      }
    }
    markSettingsDirty();
    needFullRedraw = true;
  }

  if (currentScreen == SCREEN_TIME_SETUP) {
    switch (menuIndex) {
      case TIME_ROW_YEAR:
        editYear += delta;
        if (editYear < 2024) editYear = 2024;
        if (editYear > 2099) editYear = 2099;
        break;
      case TIME_ROW_MONTH:
        editMonth += delta;
        if (editMonth < 1) editMonth = 12;
        if (editMonth > 12) editMonth = 1;
        break;
      case TIME_ROW_DAY:
        editDay += delta;
        break;
      case TIME_ROW_HOUR:
        editHour += delta;
        break;
      case TIME_ROW_MINUTE:
        editMinute += delta;
        break;
    }
    clampTimeEditorDate();
    needFullRedraw = true;
  }

  if (currentScreen == SCREEN_SYSTEM) {
    if (menuIndex == SYSTEM_ROW_DIM_TIMEOUT) {
      int v = static_cast<int>(settings.dimTimeoutSec) + (delta * 5);
      if (v < 5) v = 5;
      if (v > 300) v = 300;
      settings.dimTimeoutSec = v;
      markSettingsDirty();
      needFullRedraw = true;
    }
    if (menuIndex == SYSTEM_ROW_AUTO_HOME && delta != 0) {
      settings.autoReturnHome = !settings.autoReturnHome;
      markSettingsDirty();
      needFullRedraw = true;
    }
    if (menuIndex == SYSTEM_ROW_TIME_FORMAT && delta != 0) {
      settings.use12hFormat = !settings.use12hFormat;
      markSettingsDirty();
      invalidateHomeCache();
      needFullRedraw = true;
    }
  }
}

void goBack() {
  editMode = false;

  switch (currentScreen) {
    case SCREEN_HOME:
      break;
    case SCREEN_MAIN_MENU:
      currentScreen = SCREEN_HOME;
      menuIndex = 0;
      break;
    case SCREEN_PUMP_LIST:
      currentScreen = SCREEN_MAIN_MENU;
      menuIndex = 0;
      break;
    case SCREEN_PUMP_EDIT:
      currentScreen = SCREEN_PUMP_LIST;
      menuIndex = selectedPump;
      break;
    case SCREEN_SLOT_LIST:
      currentScreen = SCREEN_PUMP_EDIT;
      menuIndex = PUMP_ROW_SLOTS;
      break;
    case SCREEN_SLOT_EDIT:
      currentScreen = SCREEN_SLOT_LIST;
      menuIndex = selectedSlot;
      break;
    case SCREEN_MANUAL:
      currentScreen = SCREEN_MAIN_MENU;
      menuIndex = 1;
      break;
    case SCREEN_TIME_SETUP:
      currentScreen = SCREEN_MAIN_MENU;
      menuIndex = 2;
      break;
    case SCREEN_SYSTEM:
      currentScreen = SCREEN_MAIN_MENU;
      menuIndex = 3;
      break;
  }

  saveSettingsIfNeeded();
  needFullRedraw = true;
}

void selectCurrent() {
  switch (currentScreen) {
    case SCREEN_HOME:
      currentScreen = SCREEN_MAIN_MENU;
      menuIndex = 0;
      break;

    case SCREEN_MAIN_MENU:
      if (menuIndex == 0) currentScreen = SCREEN_PUMP_LIST;
      if (menuIndex == 1) currentScreen = SCREEN_MANUAL;
      if (menuIndex == 2) {
        beginTimeEditing();
        currentScreen = SCREEN_TIME_SETUP;
      }
      if (menuIndex == 3) currentScreen = SCREEN_SYSTEM;
      menuIndex = 0;
      break;

    case SCREEN_PUMP_LIST:
      selectedPump = menuIndex;
      currentScreen = SCREEN_PUMP_EDIT;
      menuIndex = 0;
      break;

    case SCREEN_PUMP_EDIT:
      if (menuIndex == PUMP_ROW_SLOTS) {
        currentScreen = SCREEN_SLOT_LIST;
        menuIndex = 0;
      } else if (menuIndex == PUMP_ROW_RUN) {
        if (pumpRt[selectedPump].running) stopPump(selectedPump);
        else startPump(selectedPump);
      } else {
        editMode = !editMode;
      }
      break;

    case SCREEN_SLOT_LIST:
      selectedSlot = menuIndex;
      currentScreen = SCREEN_SLOT_EDIT;
      menuIndex = 0;
      break;

    case SCREEN_SLOT_EDIT:
      editMode = !editMode;
      break;

    case SCREEN_MANUAL:
      if (menuIndex == 0) {
        if (pumpRt[0].running) stopPump(0); else startPump(0);
      }
      if (menuIndex == 1) {
        if (pumpRt[1].running) stopPump(1); else startPump(1);
      }
      if (menuIndex == 2) {
        startPump(0);
        startPump(1);
      }
      if (menuIndex == 3) {
        stopPump(0);
        stopPump(1);
      }
      break;

    case SCREEN_TIME_SETUP:
      if (menuIndex == TIME_ROW_APPLY) {
        if (rtcAvailable) {
          clampTimeEditorDate();
          rtc.adjust(DateTime(editYear, editMonth, editDay, editHour, editMinute, 0));
          invalidateHomeCache();
        }
      } else {
        editMode = !editMode;
      }
      break;

    case SCREEN_SYSTEM:
      if (menuIndex == SYSTEM_ROW_DIM_TIMEOUT) {
        editMode = !editMode;
      } else if (menuIndex == SYSTEM_ROW_DEFAULTS) {
        setDefaults();
        saveSettingsIfNeeded();
        invalidateHomeCache();
      } else if (menuIndex == SYSTEM_ROW_AUTO_HOME) {
        settings.autoReturnHome = !settings.autoReturnHome;
        markSettingsDirty();
      } else if (menuIndex == SYSTEM_ROW_TIME_FORMAT) {
        settings.use12hFormat = !settings.use12hFormat;
        markSettingsDirty();
        invalidateHomeCache();
      }
      break;
  }

  needFullRedraw = true;
}

// =====================================================
// LIVE UI / AUTO HOME / DIM
// =====================================================

void updateLiveUi() {
  const unsigned long ms = nowMs();
  if (ms - lastUiUpdate < LIVE_UI_UPDATE_MS) return;
  lastUiUpdate = ms;

  if (currentScreen == SCREEN_HOME) updateHomeDynamic();
  else updateStatusBarText();
}

void checkAutoHome() {
  if (!settings.autoReturnHome) return;
  if (currentScreen == SCREEN_HOME) return;

  if (nowMs() - lastInteractionMs > AUTO_HOME_TIMEOUT_MS) {
    currentScreen = SCREEN_HOME;
    menuIndex = 0;
    editMode = false;
    saveSettingsIfNeeded();
    needFullRedraw = true;
  }
}

void checkScreenDim() {
  if (settings.dimTimeoutSec > 0) {
    if (!isScreenDimmed && (nowMs() - lastInteractionMs > (static_cast<unsigned long>(settings.dimTimeoutSec) * 1000UL))) {
      analogWrite(TFT_BL, BRIGHT_DIM);
      isScreenDimmed = true;
    }
  }
}

// =====================================================
// HARDWARE INIT
// =====================================================

void initInputs() {
  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);
  pinMode(ENC_BTN, INPUT_PULLUP);
  pinMode(AUX_BTN, INPUT_PULLUP);

  lastEncA = digitalRead(ENC_A);
  lastEncB = digitalRead(ENC_B);
}

void initOutputs() {
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  digitalWrite(RELAY1_PIN, LOW);
  digitalWrite(RELAY2_PIN, LOW);

  pinMode(TFT_BL, OUTPUT);
  analogWrite(TFT_BL, BRIGHT_NORMAL);
}

void initDisplay() {
  SPI.begin(TFT_SCLK, -1, TFT_SDA_MOSI, TFT_CS);
  tft.init(240, 320);
  tft.setRotation(3);
  tft.invertDisplay(false);
  tft.fillScreen(C_BG);
}

void initRtc() {
  Wire.begin(I2C_SDA, I2C_SCL);
  if (!rtc.begin(&Wire)) {
    rtcAvailable = false;
  } else {
    rtcAvailable = true;
    if (rtc.lostPower()) {
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
  }
}

// =====================================================
// SETUP / LOOP
// =====================================================

void setup() {
  Serial.begin(SERIALSPEED);

  EEPROM.begin(EEPROM_SIZE);
  loadSettings();

  initInputs();
  initOutputs();
  initDisplay();
  initRtc();

  for (uint8_t i = 0; i < PUMP_COUNT; ++i) {
    pumpRt[i].running = false;
    pumpRt[i].startedAtMs = 0;
    pumpRt[i].stopAtMs = 0;
    pumpRt[i].triggeredThisMinute = false;
  }

  beginTimeEditing();
  lastInteractionMs = nowMs();
  needFullRedraw = true;
}

void loop() {
  pollInputs();
  updatePumps();
  checkSchedules();
  checkAutoHome();
  checkScreenDim();

  if (encoderDelta != 0) {
    moveSelection(encoderDelta);
    encoderDelta = 0;
    noteInteraction();
  }

  if (encBtnState.pressedEvent) {
    selectCurrent();
    clearButtonEvents(encBtnState);
    noteInteraction();
  }

  if (auxBtnState.pressedEvent) {
    goBack();
    clearButtonEvents(auxBtnState);
    noteInteraction();
  }

  if (encBtnState.longPressEvent) {
    saveSettingsIfNeeded();
    clearButtonEvents(encBtnState);
    noteInteraction();
  }

  if (auxBtnState.longPressEvent) {
    currentScreen = SCREEN_HOME;
    menuIndex = 0;
    editMode = false;
    needFullRedraw = true;
    clearButtonEvents(auxBtnState);
    noteInteraction();
  }

  if (needFullRedraw) drawCurrentScreen();
  else updateLiveUi();
}
