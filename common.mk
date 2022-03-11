TOP_DIR = $(CURDIR)
SRC_DIR = $(TOP_DIR)/src
BUILD_DIR = $(TOP_DIR)/build
INC_DIR =  $(TOP_DIR)/include

CONF_PRINT_ENABLE    = 0
CONF_PRINTF_ON       = 0
CONF_ZSBL_ONCHIP     = 1
CONF_STORAGE_PROBE   = 1
CONF_STORAGE_PROBE_QUICK   = 1

CONF_MC_ACTS_AS_PC   = 0
CONF_PC_ACTS_AS_MC   = 0

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

ifeq ($(CONF_PRINT_ENABLE), 1)
CFLAGS += -D PRINT_ENABLE
endif

ifeq ($(CONF_PRINTF_ON), 1)
CFLAGS += -D PRINTF_ON
endif

ifeq ($(CONF_ZSBL_ONCHIP), 1)
CFLAGS += -D ZSBL_ONCHIP
endif

ifeq ($(CONF_STORAGE_PROBE), 1)
CFLAGS += -D STORAGE_PROBE
ifeq ($(CONF_STORAGE_PROBE_QUICK), 1)
CFLAGS += -D STORAGE_PROBE_QUICK
endif
endif

ifeq ($(CONF_MC_ACTS_AS_PC), 1)
CFLAGS += -D MC_ACTS_AS_PC
endif

ifeq ($(CONF_PC_ACTS_AS_MC), 1)
CFLAGS += -D PC_ACTS_AS_MC
endif

LDFLAGS = -nostdlib
