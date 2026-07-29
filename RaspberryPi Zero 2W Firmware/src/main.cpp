#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <csignal>

#include "arduino_compat.h"
#include "protocol.h"

static std::string g_port = "/dev/ttyACM0";
static int g_baud = LSWARM_LINK_BAUD;

static SerialPort link;
static bool g_linkUp = false;
static uint32_t g_lastRxMs = 0;
static uint32_t g_lastPingMs = 0;
static uint32_t g_lastBattMv = 0;
static int g_soc = -1;
static int g_fault = 0;

static const uint32_t LINK_TIMEOUT_MS = 4000;
static const uint32_t PING_PERIOD_MS = 3000;

static volatile sig_atomic_t g_run = 1;
static void onSig(int) { g_run = 0; }

static void logLine(const char *tag, const std::string &s) {
  printf("[%8u] %-4s %s\n", millis(), tag, s.c_str());
  fflush(stdout);
}

static bool getKV(const std::string &s, const char *key, long &val) {
  std::string pat = std::string(" ") + key + "=";
  std::string hay = " " + s;
  size_t p = hay.find(pat);
  if (p == std::string::npos) return false;
  val = strtol(hay.c_str() + p + pat.size(), nullptr, 10);
  return true;
}

static void onTelemetry(const std::string &line) {
  long v = 0, soc = 0, fault = 0;
  getKV(line, "v", v);
  getKV(line, "soc", soc);
  getKV(line, "fault", fault);
  g_lastBattMv = (uint32_t)v;
  g_soc = (int)soc;
  g_fault = (int)fault;
}

static void onLinkMessage(const std::string &line) {
  g_lastRxMs = millis();

  if (line.rfind(RSP_READY, 0) == 0) {
    logLine("ESP", ("co-processor ready: " + line).c_str());
    link.println(std::string(CMD_ID));
    link.println(std::string(CMD_TELEM) + " " + std::to_string(LSWARM_TELEM_MS));
    return;
  }
  if (line.rfind(RSP_TELEM, 0) == 0) {
    onTelemetry(line);
    printf("[%8u] BATT %u mV  soc=%d%%  fault=%d\n", millis(), g_lastBattMv, g_soc, g_fault);
    fflush(stdout);
    return;
  }
  if (line.rfind(RSP_EVENT, 0) == 0) {
    logLine("EVT", line);
    long f = 0;
    getKV(line, "fault", f);
    if (f) logLine("WARN", "battery-protection FAULT");
    return;
  }
  logLine("ESP", line);
}

void setup() {
  console::init();
  printf("LSwarm Pi compute  fw=%s  port=%s @ %d\n", LSWARM_FW_VERSION, g_port.c_str(), g_baud);
  printf("Type commands (e.g. 'LED blink', 'BEEP 2', 'STATUS?') and press Enter.\n");
  fflush(stdout);
}

void loop() {
  const uint32_t now = millis();

  if (!link.isOpen()) {
    if (link.ensureOpen(g_port.c_str(), g_baud)) {
      logLine("LINK", "opened " + g_port);
      g_linkUp = false;
      g_lastRxMs = now;
    } else {
      delay(500);
      return;
    }
  }

  std::string line;
  while (link.readLine(line)) onLinkMessage(line);
  if (!link.isOpen()) {
    logLine("LINK", "lost, will reconnect");
    g_linkUp = false;
    return;
  }

  std::string cmd;
  while (console::readLine(cmd)) {
    if (cmd == "quit" || cmd == "exit") { g_run = 0; return; }
    link.println(cmd);
  }

  if (now - g_lastPingMs >= PING_PERIOD_MS) { g_lastPingMs = now; link.println(std::string(CMD_PING)); }

  bool up = (now - g_lastRxMs) < LINK_TIMEOUT_MS;
  if (up != g_linkUp) {
    g_linkUp = up;
    logLine("LINK", up ? "co-processor online" : "co-processor STALE (no telemetry)");
  }

  delay(5);
}

int main(int argc, char **argv) {
  if (argc > 1) g_port = argv[1];
  if (argc > 2) g_baud = atoi(argv[2]);

  signal(SIGINT, onSig);
  signal(SIGTERM, onSig);

  setup();
  while (g_run) loop();

  link.end();
  printf("\nshutting down.\n");
  return 0;
}
