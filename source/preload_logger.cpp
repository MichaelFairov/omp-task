#define _GNU_SOURCE
#include "log_entry.hpp"
#include "shared_buffer.hpp"
#include <atomic>
#include <cstring>
#include <dlfcn.h>
#include <fcntl.h>
#include <iostream>
#include <mutex>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

namespace {
SharedBuffer buffer("/userspace_log", 1 << 20);
long long now_ms() {
  timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  return ts.tv_sec * 1000LL + ts.tv_nsec / 1000000;
}
template <typename T> T resolve(const char *name) {
  return reinterpret_cast<T>(dlsym(RTLD_NEXT, name));
}
} // namespace

extern "C" {
int open(const char *pathname, int flags, ...) {
  auto real = resolve<int (*)(const char *, int, ...)>("open");
  int fd = real(pathname, flags);
  buffer.write(
      {now_ms(), LogType::OPEN, getpid(), OpenInfo{pathname, flags, fd}});
  return fd;
}

ssize_t read(int fd, void *buf, size_t count) {
  auto real = resolve<ssize_t (*)(int, void *, size_t)>("read");
  ssize_t r = real(fd, buf, count);
  buffer.write(
      {now_ms(), LogType::READ, getpid(), ReadInfo{fd, buf, count, r}});
  return r;
}

ssize_t write(int fd, const void *buf, size_t count) {
  auto real = resolve<ssize_t (*)(int, const void *, size_t)>("write");
  ssize_t r = real(fd, buf, count);
  buffer.write(
      {now_ms(), LogType::WRITE, getpid(), WriteInfo{fd, buf, count, r}});
  return r;
}

int close(int fd) {
  auto real = resolve<int (*)(int)>("close");
  int rc = real(fd);
  buffer.write({now_ms(), LogType::CLOSE, getpid(), CloseInfo{fd, rc}});
  return rc;
}

off_t lseek(int fd, off_t offset, int whence) {
  auto real = resolve<off_t (*)(int, off_t, int)>("lseek");
  auto r = real(fd, offset, whence);
  buffer.write(
      {now_ms(), LogType::LSEEK, getpid(), LseekInfo{fd, offset, whence, r}});
  return r;
}

void *malloc(size_t size) {
  auto real = resolve<void *(*)(size_t)>("malloc");
  void *ptr = real(size);
  buffer.write({now_ms(), LogType::MALLOC, getpid(), MallocInfo{size, ptr}});
  return ptr;
}

void free(void *ptr) {
  auto real = resolve<void (*)(void *)>("free");
  real(ptr);
  buffer.write({now_ms(), LogType::FREE, getpid(), FreeInfo{ptr}});
}

void *realloc(void *ptr, size_t size) {
  auto real = resolve<void *(*)(void *, size_t)>("realloc");
  void *np = real(ptr, size);
  buffer.write(
      {now_ms(), LogType::REALLOC, getpid(), ReallocInfo{ptr, size, np}});
  return np;
}
}
