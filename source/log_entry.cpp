#include "log_entry.hpp"
#pragma once
#include "log_entry.hpp"
#include <atomic>
#include <cstddef>
#include <mutex>
#include <string>
#include <vector>

class SharedBuffer {
public:
  SharedBuffer(const std::string &name, size_t size);
  ~SharedBuffer();

  void write(const LogEntry &entry);
  std::vector<LogEntry> read_all();

private:
  std::string shm_name;
  size_t shm_size;
  int shm_fd;
  uint8_t *shm_ptr;
  std::mutex write_mutex;

  struct Header {
    std::atomic<size_t> write_offset;
  };
  Header *header();
  uint8_t *data_start();
};
