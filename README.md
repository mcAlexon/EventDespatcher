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

ЛАБОРАТОРНАЯ 3/////////////////////////////////////////////////

# FS-EventHub — Event-Driven Сервис на C++

**Дисциплина:** Технологии системного программирования  
**Лабораторная работа:** ЛР2 + ЛР3

**Цель проекта:**  
Разработать событийно-ориентированный сервис, который обрабатывает события (внутренние и внешние), маршрутизирует их через `EventDispatcher` и обменивается ими через брокер сообщений Redis.

---

## Выполненные лабораторные работы

- **ЛР2** — Реализован `EventDispatcher`, регистрация обработчиков, автотесты (Google Test)
- **ЛР3** — Интеграция с Redis (publish/subscribe), `RedisClient`, преобразование событий

---

## Структура проекта
.
├── .devcontainer/              # Docker окружение
│   ├── Dockerfile
│   └── devcontainer.json
├── include/
│   ├── event_dispatcher/
│   └── redis/
├── src/
│   ├── EventDispatcher.cpp
│   ├── LogHandler.cpp
│   ├── EchoHandler.cpp
│   ├── RedisPublisherHandler.cpp
│   ├── RedisClient.cpp
│   └── main.cpp
├── tests/
├── build/                      # Папка сборки
├── CMakeLists.txt
└── README.md
text---

## 1. Настройка окружения (Подробный гайд)

### Требования
- Docker Desktop (установлен и запущен)
- Visual Studio Code
- Расширения VS Code:
  - **Dev Containers** (Microsoft) — обязательно
  - **C/C++ Extension Pack** (Microsoft)
  - **CMake Tools** (Microsoft)
  - **CMake Language Support** (twxs)

### Шаг 1: Открытие проекта в Dev Container
1. Открой папку проекта в VS Code.
2. Нажми `Ctrl + Shift + P`.
3. Выбери команду **Dev Containers: Reopen in Container**.
4. Подожди, пока VS Code соберёт контейнер и перезапустится внутри него.

После успешного запуска внизу слева должно быть написано:  
**Dev Container: C++ Event System...**

### Шаг 2: Запуск Redis
В терминале **внутри Dev Container** выполни:

```bash
# Запуск Redis в фоне
docker run -d --name redis-local -p 6379:6379 redis:7-alpine

# Проверка подключения
redis-cli -h host.docker.internal ping
Должно ответить: PONG

2. Сборка и запуск проекта
Сборка
Bashcd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . --clean-first
Запуск тестов (ЛР2)
Bashcd build
ctest -V
# или напрямую
./tests
Запуск основного приложения (ЛР2 + ЛР3)
Bashcd build
./app

3. Как работает система
При запуске ./app происходит следующее:

EventDispatcher инициализируется.
Подключается RedisClient.
Запускается подписчик на канал fs_events.
Регистрируются обработчики:
LogHandler
EchoHandler
RedisPublisherHandler

При вызове dispatcher.dispatch() событие проходит через все обработчики и публикуется в Redis.


Полезные команды
Bash# Пересобрать проект с очисткой
cd build && cmake --build . --clean-first

# Посмотреть сообщения в Redis (в отдельном терминале)
redis-cli -h host.docker.internal SUBSCRIBE fs_events

# Остановить Redis
docker stop redis-local