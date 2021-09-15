TOP_DIR = $(CURDIR)
SRC_DIR = $(TOP_DIR)/src
BUILD_DIR = $(TOP_DIR)/build
INC_DIR =  $(TOP_DIR)/include

CFLAGS = -mcmodel=medany -fvisibility=hidden -nostdlib -nostartfiles -march=rv64g -mabi=lp64d
CFLAGS += -static
CFLAGS += -c
CFLAGS += -Wall
CFLAGS += -I $(INC_DIR)

LDFLAGS = -nostdlib -nostartfiles

CROSS_TOOL = riscv64-linux-gnu-
CC = $(CROSS_TOOL)gcc
DUMP = $(CROSS_TOOL)objdump

