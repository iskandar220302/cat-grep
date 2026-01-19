FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    build-essential \
    valgrind \
    diffutils \
    coreutils \
    grep \
    && rm -rf /var/lib/apt/lists/*

# Корень проекта
WORKDIR /project

# Копируем весь репозиторий
COPY . .

# ---------- s21_cat ----------
WORKDIR /project/src/cat
RUN make

# ---------- s21_grep ----------
WORKDIR /project/src/grep
RUN make

CMD ["bash"]
