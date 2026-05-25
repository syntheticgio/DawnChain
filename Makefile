# Compiler
CXX = g++

# Compiler Flags
CXXFLAGS = -std=c++11 -Wall -Wextra

# OpenSSL: use pkg-config when available, fall back to Homebrew on macOS
OPENSSL_CFLAGS := $(shell pkg-config --cflags openssl 2>/dev/null)
OPENSSL_LIBS   := $(shell pkg-config --libs openssl 2>/dev/null)

ifeq ($(strip $(OPENSSL_LIBS)),)
  # pkg-config not available or OpenSSL not found — try common Homebrew path
  OPENSSL_PREFIX ?= /opt/homebrew/opt/openssl
  OPENSSL_CFLAGS  = -I$(OPENSSL_PREFIX)/include
  OPENSSL_LIBS    = -L$(OPENSSL_PREFIX)/lib -lssl -lcrypto
endif

# Include directories
INCLUDES = -Isrc/ $(OPENSSL_CFLAGS)

# Libraries
LIBS = $(OPENSSL_LIBS)

# Source Files
SRC_DIR = src
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)

# Object Files
OBJ_DIR = obj
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))

# Executable
EXEC = blockchain_app

# Targets
all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

# Create the obj directory if it doesn't exist
$(OBJECTS): | $(OBJ_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR) $(EXEC)

.PHONY: all clean
