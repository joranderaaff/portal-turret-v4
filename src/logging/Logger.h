#pragma once

#include <Arduino.h>
#include <stdarg.h>

class TurretWebServer;

// Values are sent over the websocket, keep in sync with LEVEL_* in data/www/index.js.
enum class LogLevel : uint8_t {
  Info = 0,
  Warning = 1,
  Error = 2,
};

// printf-style messages for the log panel of the web UI. Cosmetic only, not for debugging.
class Logger {
public:
  explicit Logger(TurretWebServer &server);
  void Info(const char *format, ...) __attribute__((format(printf, 2, 3)));
  void Warn(const char *format, ...) __attribute__((format(printf, 2, 3)));
  void Error(const char *format, ...) __attribute__((format(printf, 2, 3)));

private:
  void Send(LogLevel level, const char *format, va_list args);

  TurretWebServer &server;
};
