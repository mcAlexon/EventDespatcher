# ЛР2: Ядро системы (Event Dispatcher)

**Цель работы:**  
Реализовать маршрутизацию событий через `EventDispatcher` с возможностью регистрации обработчиков.  
Добавить 2–3 примера обработчиков (`LogHandler`, `EchoHandler`).  
Написать и запустить юнит-тесты с помощью Google Test.  
Настроить сборку проекта через CMake.

## Структура проекта
.
├── .devcontainer/              # Настройка Docker-контейнера (Dev Containers)
│   ├── Dockerfile
│   └── devcontainer.json
├── include/
│   └── event_dispatcher/
│       └── EventDispatcher.h
├── src/
│   ├── EventDispatcher.cpp
│   ├── LogHandler.cpp
│   ├── EchoHandler.cpp
│   └── main.cpp                # Демонстрация работы
├── tests/
│   └── EventDispatcherTest.cpp # Юнит-тесты
├── CMakeLists.txt
├── README.md
└── build/                      # Папка сборки (игнорируется в .gitignore)
text## Требования

- Docker Desktop (установлен)
- Visual Studio Code
- Расширение **Dev Containers** (Microsoft)

## 1. Настройка окружения (Docker + Dev Containers)

### Установка необходимых расширений VS Code (один раз)
Открой VS Code → `Ctrl+Shift+X` и установи:
- **Dev Containers** (автор: Microsoft) — главное расширение
- **C/C++** (Microsoft) — подсветка и IntelliSense
- **C/C++ Extension Pack** (Microsoft) — дополнительные инструменты
- **CMake Tools** (Microsoft) — работа с CMake
- **CMake Language Support** (twxs) — подсветка CMakeLists.txt

### Как запустить проект в Docker-контейнере
1. Открой папку проекта в VS Code.
2. Нажми `Ctrl+Shift+P` → введи **Dev Containers: Reopen in Container**.
3. Выбери конфигурацию `C++ Event System (ЛР2 и дальше)`.
4. VS Code автоматически соберёт Docker-образ (первый раз 3–7 минут) и перезапустится **внутри контейнера**.

После этого внизу слева будет надпись **Dev Container: C++ Event System…** — всё готово.

**Важно:** Все команды `cmake`, `g++`, `ctest` теперь выполняются **внутри контейнера**, а не на твоей машине.

## 2. Сборка проекта

В терминале VS Code (уже внутри контейнера) выполни:

```bash
# Переходим в папку сборки
cd build

# Генерируем файлы сборки (если папка build пустая)
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Собираем проект
cmake --build .

3. Запуск тестов
Bashcd build
ctest -V          # подробный вывод
# или напрямую
./tests

4. Запуск демонстрационной программы
Bashcd build
./app