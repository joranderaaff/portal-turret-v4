#include "logging/Logger.h"

#include "web/TurretWebServer.h"

namespace {

constexpr size_t LOG_TEXT_BYTES = 192;

} // namespace

Logger::Logger(TurretWebServer &serverIn) : server(serverIn) {}

void Logger::Send(LogLevel level, const char *format, va_list args) {
  char text[LOG_TEXT_BYTES];
  vsnprintf(text, sizeof(text), format, args);
  server.SendLog(level, text);
}

void Logger::Info(const char *format, ...) {
  va_list args;
  va_start(args, format);
  Send(LogLevel::Info, format, args);
  va_end(args);
}

void Logger::Warn(const char *format, ...) {
  va_list args;
  va_start(args, format);
  Send(LogLevel::Warning, format, args);
  va_end(args);
}

void Logger::Error(const char *format, ...) {
  va_list args;
  va_start(args, format);
  Send(LogLevel::Error, format, args);
  va_end(args);
}
