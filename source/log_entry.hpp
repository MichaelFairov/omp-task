#pragma once
#include <cstddef>
#include <string>
#include <unistd.h>
#include <variant>

enum class LogType { OPEN, READ, WRITE, CLOSE, LSEEK, MALLOC, FREE, REALLOC };

struct OpenInfo {
  std::string path;
  int flags;
  int fd;
};
struct ReadInfo {
  int fd;
  void *buffer;
  size_t count;
  ssize_t bytesRead;
};
struct WriteInfo {
  int fd;
  const void *buffer;
  size_t count;
  ssize_t bytesWritten;
};
struct CloseInfo {
  int fd;
  int returnCode;
};
struct LseekInfo {
  int fd;
  off_t offset;
  int whence;
  off_t result;
};
struct MallocInfo {
  size_t size;
  void *result;
};
struct FreeInfo {
  void *ptr;
};
struct ReallocInfo {
  void *oldPtr;
  size_t newSize;
  void *newPtr;
};

using LogData = std::variant<OpenInfo, ReadInfo, WriteInfo, CloseInfo,
                             LseekInfo, MallocInfo, FreeInfo, ReallocInfo>;

struct LogEntry {
  long long timestamp_ms;
  LogType type;
  pid_t pid;
  LogData data;
};
