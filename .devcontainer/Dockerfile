FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

# Установка всех зависимостей
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
    nlohmann-json3-dev \
    redis-tools \
    && rm -rf /var/lib/apt/lists/*

# Сборка Google Test
RUN cd /usr/src/googletest && \
    cmake -B build -S . -DCMAKE_BUILD_TYPE=Release && \
    cmake --build build && \
    cmake --install build --prefix /usr/local

# Создание пользователя
RUN useradd -m -s /bin/bash devuser
USER devuser
WORKDIR /workspace