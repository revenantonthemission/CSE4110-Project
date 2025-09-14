# Makefile

# 컴파일러 설정
CXX = g++
CXXFLAGS = -std=c++17 -Wall

# 운영체제 감지
UNAME_S := $(shell uname -s)

# MySQL 설정
# macOS (Homebrew 기준)
ifeq ($(UNAME_S),Darwin)
    MYSQL_DIR = /opt/homebrew
    MYSQL_INCLUDE = -I$(MYSQL_DIR)/include/mysql
    MYSQL_LIB = -L$(MYSQL_DIR)/lib
endif

# Linux 기준
ifeq ($(UNAME_S),Linux)
    MYSQL_INCLUDE = -I/usr/include/mysql
    MYSQL_LIB = -L/usr/lib/x86_64-linux-gnu
endif

# Windows (MSYS2/MinGW 기준)
ifeq ($(OS),Windows_NT)
    MYSQL_DIR = C:/Program Files/MySQL/MySQL Server 8.0
    MYSQL_INCLUDE = -I"$(MYSQL_DIR)/include"
    MYSQL_LIB = -L"$(MYSQL_DIR)/lib"
endif

# ODBC 설정
ifeq ($(UNAME_S),Darwin)
    ODBC_INCLUDE = -I/opt/homebrew/include
    ODBC_LIB = -L/opt/homebrew/lib
endif

ifeq ($(UNAME_S),Linux)
    ODBC_INCLUDE = -I/usr/include
    ODBC_LIB = -L/usr/lib/x86_64-linux-gnu
endif

# 헤더 파일 경로
INCLUDES = $(MYSQL_INCLUDE) $(ODBC_INCLUDE)

# 라이브러리
LIBS = $(MYSQL_LIB) -lmysqlclient -lodbc

# 소스 파일
SRCS = main.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = app

# 기본 타겟
all: $(TARGET)

# 링킹
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LIBS)

# 컴파일
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# 정리
clean:
	rm -f $(OBJS) $(TARGET)

# 의존성 표시
.PHONY: all clean

# 도움말
help:
	@echo "사용 가능한 make 옵션:"
	@echo "  make          - 프로그램 빌드"
	@echo "  make clean    - 빌드 파일 정리"
	@echo "  make help     - 이 도움말 표시"
	@echo ""
	@echo "커스텀 MySQL 경로 설정:"
	@echo "  make MYSQL_DIR=/path/to/mysql"