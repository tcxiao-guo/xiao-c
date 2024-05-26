/**
 * @file   Logger.h
 * @author xiao guo
 *
 *
 * @date   2024-5-26
 */
#pragma once

#include <xiao/exports.h>
#include <xiao/utils/Date.h>
#include <xiao/utils/LogStream.h>
#include <xiao/utils/NonCopyable.h>

#include <functional>
#include <memory>
#include <vector>

BEGIN_NAMESPACE(spdlog)
class logger;
END_NAMESPACE(spdlog)

#define XIAO_IF_(cond) for (int _r = 0; _r == 0 && (cond); _r = 1)

BEGIN_NAMESPACE(xiao)

class XIAO_EXPORT Logger : public NonCopyable {
 public:
  enum LogLevel {
    xTrace = 0,
    xDebug,
    xInfo,
    xWarn,
    xError,
    xFatal,
    xNumberofLogLevels
  };

  class SourceFile {
   public:
    template <int N>
    inline SourceFile(const char (&arr)[N]) : data_(arr), size_(N - 1) {
#ifndef _MSC_VER
      const char* slash = strrchr(data_, '/');
#else
      const char* slash = strrchr(data_, '\\');
#endif  // !_MSC_VER
      if (slash) {
        data_ = slash + 1;
        size_ -= static_cast<int>(data_ - arr);
      }
    }

    explicit SourceFile(const char* filename = nullptr) : data_(filename) {
      if (!filename) {
        size_ = 0;
        return;
      }
#ifndef _MSC_VER
      const char* slash = strrchr(filename, '/');
#else
      const char* slash = strrchr(filename, '\\');
#endif  // !_MSC_VER
      if (slash) {
        data_ = slash + 1;
      }
      size_ = static_cast<int>(strlen(data_));
    }

    const char* data_;
    int size_;
  };
  Logger(SourceFile file, int line);
  Logger(SourceFile file, int line, LogLevel level);
  Logger(SourceFile file, int line, bool isSysErr);
  Logger(SourceFile file, int line, LogLevel level, const char* func);

  Logger();
  Logger(LogLevel level);
  Logger(bool isSysErr);

  ~Logger();
  Logger& setIndex(int index) {
    index_ = index;
    return *this;
  }
  LogStream& stream();

  static void setOutputFunction(
      std::function<void(const char* msg, const uint64_t len)> outputFunc,
      std::function<void()> flushFunc, int index = -1) {
    if (index < 0) {
      outputFunc_() = outputFunc;
      flushFunc_() = flushFunc;
    } else {
      outputFunc_(index) = outputFunc;
      flushFunc_(index) = flushFunc;
    }
  }

  static void setLogLevel(LogLevel level) { logLevel_() = level; }

  static LogLevel logLevel() { return logLevel_(); }

  static bool displayLocalTime() { return displayLocalTime_(); }

  static void setDisplayLocalTime(bool showLocalTime) {
    displayLocalTime_() = showLocalTime;
  }

  static bool hasSpdLogSupport();

  static void enableSpdLog(int index,
                           std::shared_ptr<spdlog::logger> logger = {});

  inline static void enableSpdLog(std::shared_ptr<spdlog::logger> logger = {}) {
    enableSpdLog(-1, logger);
  }

  static void disableSpdLog(int index);

  static void disableSpdLog() { disableSpdLog(-1); }

  static std::shared_ptr<spdlog::logger> getSpdLogger(int index = -1);

  static std::shared_ptr<spdlog::logger> getDefaultSpdLogger(int index);

 protected:
  static void defaultOutputFunction(const char* msg, const uint64_t len) {
    fwrite(msg, 1, static_cast<size_t>(len), stdout);
  }
  static void defaultFlushFunction() { fflush(stdout); }
  void formatTime();
  static bool& displayLocalTime_() {
    static bool showLocalTime = false;
    return showLocalTime;
  }
  static LogLevel& logLevel_() {
#ifdef RELEASE
    static LogLevel logLevel = LogLevel::xInfo;
#else
    static LogLevel logLevel = LogLevel::xDebug;
#endif
    return logLevel;
  }
  static std::function<void(const char* msg, const uint64_t len)>&
  outputFunc_() {
    static std::function<void(const char* msg, const uint64_t len)> outputFunc =
        Logger::defaultOutputFunction;
    return outputFunc;
  }
  static std::function<void()>& flushFunc_() {
    static std::function<void()> flushFunc = Logger::defaultFlushFunction;
    return flushFunc;
  }
  static std::function<void(const char* msg, const uint64_t len)>& outputFunc_(
      size_t index) {
    static std::vector<std::function<void(const char* msg, const uint64_t len)>>
        outputFuncs;
    if (index < outputFuncs.size()) {
      return outputFuncs[index];
    }
    while (index >= outputFuncs.size()) {
      outputFuncs.emplace_back(outputFunc_());
    }
    return outputFuncs[index];
  }
  static std::function<void()>& flushFunc_(size_t index) {
    static std::vector<std::function<void()>> flushFuncs;
    if (index < flushFuncs.size()) {
      return flushFuncs[index];
    }
    while (index >= flushFuncs.size()) {
      flushFuncs.emplace_back(flushFunc_());
    }
    return flushFuncs[index];
  }

  friend class RawLogger;
  LogStream logStream_;
  Date date_{Date::now()};
  SourceFile sourceFile_;
  int fileLine_;
  LogLevel level_;
  int index_{-1};
  size_t spdLogMessageOffset_{ 0 };
};

class XIAO_EXPORT RawLogger : public NonCopyable {
 public:
  ~RawLogger();
  RawLogger& setIndex(int index) {
    index_ = index;
    return *this;
  }

  LogStream& stream() { return logStream_; }

 private:
  LogStream logStream_;
  int index_{-1};
};

#ifdef NDEBUG
#define LOG_TRACE \
  XIAO_IF_(0)     \
  xiao::Logger(__FILE__, __LINE__, xiao::Logger::xTrace, __func__).stream()
#else
#define LOG_TRACE                                            \
  XIAO_IF_(xiao::Logger::logLevel() <= xiao::Logger::xTrace) \
  xiao::Logger(__FILE__, __LINE__, xiao::Logger::xTrace, __func__).stream()
#define LOG_TRACE_TO(index)                                        \
  XIAO_IF_(xiao::Logger::logLevel() <= xiao::Logger::xTrace)       \
  xiao::Logger(__FILE__, __LINE__, xiao::Logger::xTrace, __func__) \
      .setIndex(index)                                             \
      .stream()
#endif

#define LOG_DEBUG                                            \
  XIAO_IF_(xiao::Logger::logLevel() <= xiao::Logger::xDebug) \
  xiao::Logger(__FILE__, __LINE__, xiao::Logger::xDebug, __func__).stream()
#define LOG_DEBUG_TO(index)                                           \
  XIAO_IF_(xiao::Logger::logLevel() <= xiao::Logger::xDebug)          \
  xiao::Logger(__FILE__, __LINE__, xiao::Logger::xDebug, __func__) \
      .setIndex(index)                                                \
      .stream()
#define LOG_INFO                                            \
  XIAO_IF_(xiao::Logger::logLevel() <= xiao::Logger::xInfo) \
  xiao::Logger(__FILE__, __LINE__).stream()
#define LOG_INFO_TO(index)                                  \
  XIAO_IF_(xiao::Logger::logLevel() <= xiao::Logger::xInfo) \
  xiao::Logger(__FILE__, __LINE__).setIndex(index).stream()
#define LOG_WARN xiao::Logger(__FILE__, __LINE__, xiao::Logger::xWarn).stream()
#define LOG_WARN_TO(index) \
  xiao::Logger(__FILE__, __LINE__, xiao::Logger::xWarn).setIndex(index).stream()
#define LOG_ERROR \
  xiao::Logger(__FILE__, __LINE__, xiao::Logger::xError).stream()
#define LOG_ERROR_TO(index)                              \
  xiao::Logger(__FILE__, __LINE__, xiao::Logger::xError) \
      .setIndex(index)                                   \
      .stream()
#define LOG_FATAL \
  xiao::Logger(__FILE__, __LINE__, xiao::Logger::xFatal).stream()
#define LOG_FATAL_TO(index)                              \
  xiao::Logger(__FILE__, __LINE__, xiao::Logger::xFatal) \
      .setIndex(index)                                   \
      .stream()
#define LOG_SYSERR xiao::Logger(__FILE__, __LINE__, true).stream()
#define LOG_SYSERR_TO(index) \
  xiao::Logger(__FILE__, __LINE__, true).setIndex(index).stream()


// LOG_COMPACT_... begin block
#define LOG_COMPACT_DEBUG                                             \
  XIAO_IF_(xiao::Logger::logLevel() <= xiao::Logger::xDebug) \
  xiao::Logger(xiao::Logger::xDebug).stream()
#define LOG_COMPACT_DEBUG_TO(index)                                   \
  XIAO_IF_(xiao::Logger::logLevel() <= xiao::Logger::xDebug) \
  xiao::Logger(xiao::Logger::xDebug).setIndex(index).stream()
#define LOG_COMPACT_INFO                                             \
  XIAO_IF_(xiao::Logger::logLevel() <= xiao::Logger::xInfo) \
  xiao::Logger().stream()
#define LOG_COMPACT_INFO_TO(index)                                   \
  XIAO_IF_(xiao::Logger::logLevel() <= xiao::Logger::xInfo) \
  xiao::Logger().setIndex(index).stream()
#define LOG_COMPACT_WARN xiao::Logger(xiao::Logger::xWarn).stream()
#define LOG_COMPACT_WARN_TO(index) \
  xiao::Logger(xiao::Logger::xWarn).setIndex(index).stream()
#define LOG_COMPACT_ERROR xiao::Logger(xiao::Logger::xError).stream()
#define LOG_COMPACT_ERROR_TO(index) \
  xiao::Logger(xiao::Logger::xError).setIndex(index).stream()
#define LOG_COMPACT_FATAL xiao::Logger(xiao::Logger::xFatal).stream()
#define LOG_COMPACT_FATAL_TO(index) \
  xiao::Logger(xiao::Logger::xFatal).setIndex(index).stream()
#define LOG_COMPACT_SYSERR xiao::Logger(true).stream()
#define LOG_COMPACT_SYSERR_TO(index) \
  xiao::Logger(true).setIndex(index).stream()
// LOG_COMPACT_... end block

#define LOG_RAW xiao::RawLogger().stream()
#define LOG_RAW_TO(index) xiao::RawLogger().setIndex(index).stream()

#define LOG_TRACE_IF(cond)                                                \
  XIAO_IF_((xiao::Logger::logLevel() <= xiao::Logger::xTrace) && \
              (cond))                                                     \
  xiao::Logger(__FILE__, __LINE__, xiao::Logger::xTrace, __func__)  \
      .stream()
#define LOG_DEBUG_IF(cond)                                                \
  XIAO_IF_((xiao::Logger::logLevel() <= xiao::Logger::xDebug) && \
              (cond))                                                     \
  xiao::Logger(__FILE__, __LINE__, xiao::Logger::xDebug, __func__)  \
      .stream()
#define LOG_INFO_IF(cond)                                                \
  XIAO_IF_((xiao::Logger::logLevel() <= xiao::Logger::xInfo) && \
              (cond))                                                    \
  xiao::Logger(__FILE__, __LINE__).stream()
#define LOG_WARN_IF(cond) \
  XIAO_IF_(cond)       \
  xiao::Logger(__FILE__, __LINE__, xiao::Logger::xWarn).stream()
#define LOG_ERROR_IF(cond) \
  XIAO_IF_(cond)        \
  xiao::Logger(__FILE__, __LINE__, xiao::Logger::xError).stream()
#define LOG_FATAL_IF(cond) \
  XIAO_IF_(cond)        \
  xiao::Logger(__FILE__, __LINE__, xiao::Logger::xFatal).stream()


#ifdef NDEBUG
#define DLOG_TRACE                                                       \
  XIAO_IF_(0)                                                         \
  xiao::Logger(__FILE__, __LINE__, xiao::Logger::xTrace, __func__) \
      .stream()
#define DLOG_DEBUG                                                       \
  XIAO_IF_(0)                                                         \
  xiao::Logger(__FILE__, __LINE__, xiao::Logger::xDebug, __func__) \
      .stream()
#define DLOG_INFO \
  XIAO_IF_(0)  \
  xiao::Logger(__FILE__, __LINE__).stream()
#define DLOG_WARN \
  XIAO_IF_(0)  \
  xiao::Logger(__FILE__, __LINE__, xiao::Logger::xWarn).stream()
#define DLOG_ERROR \
  XIAO_IF_(0)   \
  xiao::Logger(__FILE__, __LINE__, xiao::Logger::xError).stream()
#define DLOG_FATAL \
  XIAO_IF_(0)   \
  xiao::Logger(__FILE__, __LINE__, xiao::Logger::kFatal).stream()

#define DLOG_TRACE_IF(cond)                                              \
  XIAO_IF_(0)                                                         \
  xiao::Logger(__FILE__, __LINE__, xiao::Logger::xTrace, __func__) \
      .stream()
#define DLOG_DEBUG_IF(cond)                                              \
  XIAO_IF_(0)                                                         \
  xiao::Logger(__FILE__, __LINE__, xiao::Logger::xDebug, __func__) \
      .stream()
#define DLOG_INFO_IF(cond) \
  XIAO_IF_(0)           \
  xiao::Logger(__FILE__, __LINE__).stream()
#define DLOG_WARN_IF(cond) \
  XIAO_IF_(0)           \
  xiao::Logger(__FILE__, __LINE__, xiao::Logger::xWarn).stream()
#define DLOG_ERROR_IF(cond) \
  XIAO_IF_(0)            \
  xiao::Logger(__FILE__, __LINE__, xiao::Logger::xError).stream()
#define DLOG_FATAL_IF(cond) \
  XIAO_IF_(0)            \
  xiao::Logger(__FILE__, __LINE__, xiao::Logger::kFatal).stream()
#else
#define DLOG_TRACE                                                       \
  XIAO_IF_(xiao::Logger::logLevel() <= xiao::Logger::xTrace)    \
  xiao::Logger(__FILE__, __LINE__, xiao::Logger::xTrace, __func__) \
      .stream()
#define DLOG_DEBUG                                                       \
  XIAO_IF_(xiao::Logger::logLevel() <= xiao::Logger::xDebug)    \
  xiao::Logger(__FILE__, __LINE__, xiao::Logger::xDebug, __func__) \
      .stream()
#define DLOG_INFO                                                    \
  XIAO_IF_(xiao::Logger::logLevel() <= xiao::Logger::xInfo) \
  xiao::Logger(__FILE__, __LINE__).stream()
#define DLOG_WARN \
  xiao::Logger(__FILE__, __LINE__, xiao::Logger::xWarn).stream()
#define DLOG_ERROR \
  xiao::Logger(__FILE__, __LINE__, xiao::Logger::xError).stream()
#define DLOG_FATAL \
  xiao::Logger(__FILE__, __LINE__, xiao::Logger::kFatal).stream()

#define DLOG_TRACE_IF(cond)                                               \
  XIAO_IF_((xiao::Logger::logLevel() <= xiao::Logger::xTrace) && \
              (cond))                                                     \
  xiao::Logger(__FILE__, __LINE__, xiao::Logger::xTrace, __func__)  \
      .stream()
#define DLOG_DEBUG_IF(cond)                                               \
  XIAO_IF_((xiao::Logger::logLevel() <= xiao::Logger::xDebug) && \
              (cond))                                                     \
  xiao::Logger(__FILE__, __LINE__, xiao::Logger::xDebug, __func__)  \
      .stream()
#define DLOG_INFO_IF(cond)                                               \
  XIAO_IF_((xiao::Logger::logLevel() <= xiao::Logger::xInfo) && \
              (cond))                                                    \
  xiao::Logger(__FILE__, __LINE__).stream()
#define DLOG_WARN_IF(cond) \
  XIAO_IF_(cond)        \
  xiao::Logger(__FILE__, __LINE__, xiao::Logger::xWarn).stream()
#define DLOG_ERROR_IF(cond) \
  XIAO_IF_(cond)         \
  xiao::Logger(__FILE__, __LINE__, xiao::Logger::xError).stream()
#define DLOG_FATAL_IF(cond) \
  XIAO_IF_(cond)         \
  xiao::Logger(__FILE__, __LINE__, xiao::Logger::kFatal).stream()
#endif

END_NAMESPACE(xiao)
