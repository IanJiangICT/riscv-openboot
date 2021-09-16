TOP_DIR = $(CURDIR)
SRC_DIR = $(TOP_DIR)/src
BUILD_DIR = $(TOP_DIR)/build
INC_DIR =  $(TOP_DIR)/include

ifdef LLVM_PATH
CC = $(LLVM_PATH)/clang
LD = $(LLVM_PATH)/ld.lld
DUMP = $(LLVM_PATH)/llvm-objdump
CFLAGS += --target=riscv64
CFLAGS += -mno-relax
else
GNU_TOOL = riscv64-linux-gnu-
CC = $(GNU_TOOL)gcc
LD = $(GNU_TOOL)ld
DUMP = $(GNU_TOOL)objdump
CFLAGS += -nostartfiles
endif

CFLAGS += -mcmodel=medany -fvisibility=hidden -nostdlib
CFLAGS += -march=rv64g
CFLAGS += -mabi=lp64d
CFLAGS += -O0

CFLAGS += -c
CFLAGS += -static
CFLAGS += -Wall
CFLAGS += -I $(INC_DIR)
CFLAGS += -I $(SRC_DIR)

LDFLAGS = -nostdlib
