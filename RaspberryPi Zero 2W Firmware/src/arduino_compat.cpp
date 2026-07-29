#include "arduino_compat.h"

#include <ctime>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>

static uint64_t mono_us() {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (uint64_t)ts.tv_sec * 1000000ULL + ts.tv_nsec / 1000ULL;
}
static const uint64_t g_start_us = mono_us();

uint32_t millis() { return (uint32_t)((mono_us() - g_start_us) / 1000ULL); }
uint64_t micros() { return mono_us() - g_start_us; }
void delay(uint32_t ms) { usleep((useconds_t)ms * 1000); }

bool LineReader::feed(char c, std::string &out) {
  if (c == '\n' || c == '\r') {
    if (buf_.empty()) return false;
    out.swap(buf_);
    buf_.clear();
    return true;
  }
  if (buf_.size() < 512) buf_.push_back(c);
  else buf_.clear();
  return false;
}

static speed_t baud_const(int baud) {
  switch (baud) {
    case 9600: return B9600;
    case 19200: return B19200;
    case 38400: return B38400;
    case 57600: return B57600;
    case 230400: return B230400;
    case 115200:
    default: return B115200;
  }
}

bool SerialPort::begin(const char *device, int baud) {
  end();
  fd_ = ::open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
  if (fd_ < 0) return false;

  struct termios tio;
  if (tcgetattr(fd_, &tio) != 0) { end(); return false; }
  cfmakeraw(&tio);
  speed_t sp = baud_const(baud);
  cfsetispeed(&tio, sp);
  cfsetospeed(&tio, sp);
  tio.c_cflag |= (CLOCAL | CREAD);
  tio.c_cflag &= ~CRTSCTS;
  tio.c_cc[VMIN] = 0;
  tio.c_cc[VTIME] = 0;
  if (tcsetattr(fd_, TCSANOW, &tio) != 0) { end(); return false; }
  tcflush(fd_, TCIOFLUSH);
  return true;
}

void SerialPort::end() {
  if (fd_ >= 0) { ::close(fd_); fd_ = -1; }
}

bool SerialPort::readLine(std::string &line) {
  if (fd_ < 0) return false;
  char c;
  ssize_t n;
  while ((n = ::read(fd_, &c, 1)) == 1) {
    if (reader_.feed(c, line)) return true;
  }
  if (n == 0 || (n < 0 && errno != EAGAIN && errno != EWOULDBLOCK)) {
    end();
  }
  return false;
}

void SerialPort::print(const std::string &s) {
  if (fd_ < 0) return;
  size_t off = 0;
  while (off < s.size()) {
    ssize_t n = ::write(fd_, s.data() + off, s.size() - off);
    if (n > 0) { off += (size_t)n; continue; }
    if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) { usleep(1000); continue; }
    break;
  }
}

void SerialPort::println(const std::string &s) { print(s); print("\n"); }

bool SerialPort::ensureOpen(const char *device, int baud) {
  if (fd_ >= 0) return true;
  return begin(device, baud);
}

namespace {
  LineReader g_stdin_reader;
  bool g_stdin_ready = false;
}

void console::init() {
  int fl = fcntl(STDIN_FILENO, F_GETFL, 0);
  if (fl != -1) fcntl(STDIN_FILENO, F_SETFL, fl | O_NONBLOCK);
  g_stdin_ready = true;
}

bool console::readLine(std::string &line) {
  if (!g_stdin_ready) return false;
  char c;
  ssize_t n;
  while ((n = ::read(STDIN_FILENO, &c, 1)) == 1) {
    if (g_stdin_reader.feed(c, line)) return true;
  }
  return false;
}
