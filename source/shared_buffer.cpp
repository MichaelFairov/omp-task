#include "shared_buffer.hpp"
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <sys/mman.h>
#include <unistd.h>

SharedBuffer::SharedBuffer(const std::string &name, size_t size)
    : shm_name(name), shm_size(size), shm_fd(-1), shm_ptr(nullptr) {
  shm_fd = shm_open(shm_name.c_str(), O_CREAT | O_RDWR, 0666);
  ftruncate(shm_fd, shm_size);
  shm_ptr = static_cast<uint8_t *>(
      mmap(nullptr, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));
  if (header()->write_offset.load() == 0)
    header()->write_offset.store(0);
}

SharedBuffer::~SharedBuffer() {
  munmap(shm_ptr, shm_size);
  close(shm_fd);
}

SharedBuffer::Header *SharedBuffer::header() {
  return reinterpret_cast<Header *>(shm_ptr);
}

uint8_t *SharedBuffer::data_start() { return shm_ptr + sizeof(Header); }

void SharedBuffer::write(const LogEntry &entry) {
  std::lock_guard lock(write_mutex);
  std::ostringstream ss;
  ss << entry.timestamp_ms << " " << static_cast<int>(entry.type) << " "
     << entry.pid << "\n";
  std::string record = ss.str();
  auto offset = header()->write_offset.load();
  if (offset + record.size() >= shm_size - sizeof(Header))
    offset = 0;
  std::memcpy(data_start() + offset, record.data(), record.size());
  header()->write_offset.store(offset + record.size());
}

std::vector<LogEntry> SharedBuffer::read_all() {
  std::vector<LogEntry> entries;

  size_t offset = 0;
  uint8_t *ptr = data_start();
  size_t max = header()->write_offset.load();

  while (offset + sizeof(long long) + sizeof(int) + sizeof(pid_t) <= max) {
    long long ts;
    int type;
    pid_t pid;

    std::memcpy(&ts, ptr + offset, sizeof(long long));
    offset += sizeof(long long);
    std::memcpy(&type, ptr + offset, sizeof(int));
    offset += sizeof(int);
    std::memcpy(&pid, ptr + offset, sizeof(pid_t));
    offset += sizeof(pid_t);

    LogEntry entry;
    entry.timestamp_ms = ts;
    entry.type = static_cast<LogType>(type);
    entry.pid = pid;
    entry.data = {};

    entries.push_back(entry);
  }
  return entries;
}
