FROM ubuntu:24.04

# Чтобы apt не спрашивал подтверждения
ENV DEBIAN_FRONTEND=noninteractive

# Устанавливаем ВСЁ, что нужно для всех твоих ЛР (и ЛР2 тоже)
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    ninja-build \
    git \
    curl \
    gdb \
    valgrind \
    libgtest-dev \
    libgmock-dev \
    libhiredis-dev \
    libmicrohttpd-dev \
    libspdlog-dev \
    && rm -rf /var/lib/apt/lists/*

# === Google Test: собираем библиотеки (это обязательно, пакет только даёт исходники) ===
RUN cd /usr/src/googletest && \
    cmake -B build -S . -DCMAKE_BUILD_TYPE=Release && \
    cmake --build build && \
    cmake --install build --prefix /usr/local

# Устанавливаем ВСЁ, что нужно
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    ninja-build \
    git \
    curl \
    gdb \
    valgrind \
    libgtest-dev \
    libgmock-dev \
    libhiredis-dev \
    libmicrohttpd-dev \
    libspdlog-dev \
    redis-tools \
    && rm -rf /var/lib/apt/lists/*

# Создаём обычного пользователя (чтобы не работать от root — это хорошая практика)
RUN useradd -m -s /bin/bash devuser && chown -R devuser /home/devuser
USER devuser
WORKDIR /workspace