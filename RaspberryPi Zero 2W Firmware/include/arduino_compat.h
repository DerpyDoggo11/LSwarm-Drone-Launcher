#pragma once

#include <cstdint>
#include <cstddef>
#include <string>

uint32_t millis();
uint64_t micros();
void delay(uint32_t ms);

class LineReader {
public:
  bool feed(char c, std::string &out);
private:
  std::string buf_;
};

class SerialPort {
public:
  bool begin(const char *device, int baud);
  void end();
  bool isOpen() const { return fd_ >= 0; }
  int fd() const { return fd_; }

  bool readLine(std::string &line);

  void print(const std::string &s);
  void println(const std::string &s);

  bool ensureOpen(const char *device, int baud);

private:
  int fd_ = -1;
  LineReader reader_;
};

namespace console {
  void init();
  bool readLine(std::string &line);
}
