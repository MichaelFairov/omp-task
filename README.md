Инструмент для динамического трассирования вызовов стандартной библиотеки `libc` с помощью механизма `LD_PRELOAD`. Он перехватывает операции с файлами и памятью, записывая информацию в shared memory. Отдельный демон считывает данные и сохраняет их в логи.

---

## 📁 Структура проекта
```
.
├── CMakeLists.txt
├── README.md
└── source/
    ├── preload_logger.cpp     
    ├── daemon_logger.cpp      
    ├── shared_buffer.cpp/hpp  
    ├── log_entry.cpp/hpp     
```

---

## 🔧 Инструкция по сборке

```bash
mkdir build && cd build
cmake ..
make
```

Результат сборки:
- `libpreload_logger.so`  — библиотека для перехвата
- `logger_daemon`         — исполняемый демон

---

## 🚀 Пример использования

### Запуск демона
```bash
./logger_daemon -f log.txt -i 1000 -m file
```

### Запуск приложения с перехватом
```bash
LD_PRELOAD=./libpreload_logger.so ls
```

### Пример строки в логе:
```
1720000000 PID=12345 TYPE=0
```

---

## Сборка RPM-пакета

### 1. Установить зависимости:
```bash
sudo dnf install rpm-build rpmdevtools cmake gcc-c++ make
# или Ubuntu:
sudo apt install rpm cmake g++ make
```

### 2. Сборка и упаковка:
```bash
mkdir -p build && cd build
cmake .. -DCPACK_GENERATOR=RPM
cmake --build .
cpack -G RPM
```

### 3. Результат:
```
userspace-logger-1.0.0-1.x86_64.rpm
```

### 4. Установка:
```bash
sudo rpm -i userspace-logger-1.0.0-1.x86_64.rpm
```

---

