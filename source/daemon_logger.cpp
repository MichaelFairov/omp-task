#include "log_entry.hpp"
#include "shared_buffer.hpp"
#include <chrono>
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <sstream>
#include <thread>

std::string stringify(const LogEntry &entry) {
  std::ostringstream oss;
  oss << entry.timestamp_ms << " PID=" << entry.pid << " ";
  oss << "TYPE=" << static_cast<int>(entry.type);
  return oss.str();
}

void run_daemon(const std::string &shm_name, const std::string &log_file,
                int interval_ms) {
  SharedBuffer buffer(shm_name, 1 << 20);
  std::ofstream out(log_file, std::ios::app);
  while (true) {
    auto logs = buffer.read_all();
    for (const auto &entry : logs)
      out << stringify(entry) << "\n";
    out.flush();
    std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
  }
}

int main(int argc, char *argv[]) {
  std::string log_file;
  int interval = 1000;
  std::string mode;
  int opt;
  while ((opt = getopt(argc, argv, "f:i:m:")) != -1) {
    if (opt == 'f')
      log_file = optarg;
    else if (opt == 'i')
      interval = std::stoi(optarg);
    else if (opt == 'm')
      mode = optarg;
  }
  run_daemon("/userspace_log", log_file, interval);
  return 0;
}
