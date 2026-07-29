#include <Arduino.h>
#include "pins.h"
#include "protocol.h"

#if ESP_ARDUINO_VERSION_MAJOR >= 3
static void buzzerBegin() { ledcAttach(PIN_BUZZER, 2700, 10); ledcWrite(PIN_BUZZER, 0); }
static void buzzerTone(uint32_t f) { if (f) ledcWriteTone(PIN_BUZZER, f); else ledcWrite(PIN_BUZZER, 0); }
#else
static const uint8_t BUZ_CH = 0;
static void buzzerBegin() { ledcSetup(BUZ_CH, 2700, 10); ledcAttachPin(PIN_BUZZER, BUZ_CH); ledcWrite(BUZ_CH, 0); }
static void buzzerTone(uint32_t f) { if (f) ledcWriteTone(BUZ_CH, f); else ledcWrite(BUZ_CH, 0); }
#endif

static uint32_t buzzEndMs = 0;
static uint8_t beepsLeft = 0;
static bool beepOn = false;
static uint32_t beepNextMs = 0;

static void buzzStart(uint32_t freq, uint32_t ms) {
  buzzerTone(freq);
  buzzEndMs = (freq && ms) ? millis() + ms : 0;
  if (!buzzEndMs) buzzerTone(0);
}

static void beepStart(uint8_t count) {
  beepsLeft = count;
  beepOn = false;
  beepNextMs = millis();
}

enum LedMode { LED_OFF, LED_ON, LED_BLINK, LED_BEAT, LED_SOS };
static LedMode ledMode = LED_BEAT;

static inline void ledWrite(bool on) { digitalWrite(PIN_STATUS_LED, on ? LOW : HIGH); }

static const uint16_t SOS_PATTERN[] = {
  200, 200, 200, 200, 200, 600,
  600, 200, 600, 200, 600, 600,
  200, 200, 200, 200, 200, 1400
};

static uint8_t sosIdx = 0;
static bool sosOn = false;
static uint32_t sosNext = 0;

static void ledTask() {
  const uint32_t now = millis();
  switch (ledMode) {
    case LED_OFF: ledWrite(false); break;
    case LED_ON: ledWrite(true); break;
    case LED_BLINK: {
      ledWrite((now / 250) & 1);
      break;
    }
    case LED_BEAT: {
      uint32_t t = now % 1200;
      ledWrite(t < 90 || (t >= 200 && t < 290));
      break;
    }
    case LED_SOS: {
      if (now >= sosNext) {
        sosOn = !sosOn;
        ledWrite(sosOn);
        sosNext = now + SOS_PATTERN[sosIdx];
        sosIdx = (sosIdx + 1) % (sizeof(SOS_PATTERN) / sizeof(SOS_PATTERN[0]));
      }
      break;
    }
  }
}

static void setLed(const String &m) {
  if (m == "off") ledMode = LED_OFF;
  else if (m == "on") ledMode = LED_ON;
  else if (m == "blink") ledMode = LED_BLINK;
  else if (m == "beat") ledMode = LED_BEAT;
  else if (m == "sos") { ledMode = LED_SOS; sosIdx = 0; sosOn = false; sosNext = 0; }
}

static uint32_t readBatt_mV() {
  uint32_t acc = 0;
  for (int i = 0; i < 8; i++) acc += analogReadMilliVolts(PIN_VMON_ADC);
  uint32_t vadc = acc / 8;
  return (uint32_t)((uint64_t)vadc * LSWARM_VDIV_NUM / LSWARM_VDIV_DEN);
}

static uint8_t soc_pct(uint32_t mV) {
  if (mV <= LSWARM_BATT_EMPTY_MV) return 0;
  if (mV >= LSWARM_BATT_FULL_MV) return 100;
  return (uint8_t)((mV - LSWARM_BATT_EMPTY_MV) * 100UL / (LSWARM_BATT_FULL_MV - LSWARM_BATT_EMPTY_MV));
}

static bool faultActive() { return digitalRead(PIN_DW_IRQ) == LOW; }

static void sendTelemetry() {
  uint32_t mV = readBatt_mV();
  Serial.printf("%s v=%u soc=%u fault=%u up=%lu\n", RSP_TELEM, mV, soc_pct(mV), faultActive() ? 1 : 0, (unsigned long)millis());
}

static void handleLine(String line) {
  line.trim();
  if (line.length() == 0) return;

  int sp = line.indexOf(' ');
  String cmd = (sp < 0) ? line : line.substring(0, sp);
  String arg = (sp < 0) ? "" : line.substring(sp + 1);
  arg.trim();

  if (cmd == CMD_PING) {
    Serial.println(RSP_PONG);
  } else if (cmd == CMD_ID) {
    Serial.printf("%s %s fw=%s\n", RSP_ID, LSWARM_DEV_NAME, LSWARM_FW_VERSION);
  } else if (cmd == CMD_STATUS) {
    sendTelemetry();
  } else if (cmd == CMD_LED) {
    setLed(arg);
    Serial.printf("%s %s\n", RSP_OK, line.c_str());
  } else if (cmd == CMD_BUZZ) {
    int sp2 = arg.indexOf(' ');
    uint32_t f = arg.toInt();
    uint32_t ms = (sp2 < 0) ? 0 : (uint32_t)arg.substring(sp2 + 1).toInt();
    buzzStart(f, ms);
    Serial.printf("%s %s\n", RSP_OK, line.c_str());
  } else if (cmd == CMD_BEEP) {
    beepStart((uint8_t)constrain(arg.toInt(), 1, 20));
    Serial.printf("%s %s\n", RSP_OK, line.c_str());
  } else if (cmd == CMD_TELEM) {
    Serial.printf("%s %s\n", RSP_OK, line.c_str());
  } else {
    Serial.printf("%s unknown:%s\n", RSP_ERR, cmd.c_str());
    return;
  }
}

static uint32_t telemPeriod = LSWARM_TELEM_MS;

static void maybeHandleTelemPeriod(const String &line) {
  if (line.startsWith(CMD_TELEM)) {
    int sp = line.indexOf(' ');
    if (sp > 0) telemPeriod = (uint32_t)line.substring(sp + 1).toInt();
  }
}

static char lineBuf[LSWARM_LINE_MAX];
static uint8_t lineLen = 0;

static void serialTask() {
  while (Serial.available()) {
    char c = (char)Serial.read();
    if (c == '\n' || c == '\r') {
      if (lineLen) {
        lineBuf[lineLen] = '\0';
        String line(lineBuf);
        maybeHandleTelemPeriod(line);
        handleLine(line);
        lineLen = 0;
      }
    } else if (lineLen < LSWARM_LINE_MAX - 1) {
      lineBuf[lineLen++] = c;
    } else {
      lineLen = 0;
    }
  }
}

void setup() {
  pinMode(PIN_STATUS_LED, OUTPUT);
  ledWrite(false);
  pinMode(PIN_DW_IRQ, INPUT_PULLUP);
  analogReadResolution(12);
  buzzerBegin();

  Serial.begin(LSWARM_LINK_BAUD);
  delay(50);
  Serial.printf("%s %s\n", RSP_READY, LSWARM_FW_VERSION);
}

void loop() {
  const uint32_t now = millis();

  serialTask();
  ledTask();

  if (buzzEndMs && now >= buzzEndMs) { buzzerTone(0); buzzEndMs = 0; }
  if (beepsLeft && now >= beepNextMs) {
    beepOn = !beepOn;
    buzzerTone(beepOn ? 2700 : 0);
    beepNextMs = now + 80;
    if (!beepOn) beepsLeft--;
  }

  static uint32_t lastTelem = 0;
  if (telemPeriod && now - lastTelem >= telemPeriod) { lastTelem = now; sendTelemetry(); }

  static int lastFault = -1;
  int f = faultActive() ? 1 : 0;
  if (f != lastFault) {
    lastFault = f;
    Serial.printf("%s fault=%d\n", RSP_EVENT, f);
    if (f) beepStart(3);
  }
}
