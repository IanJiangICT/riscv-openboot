TOP_DIR = $(CURDIR)
SRC_DIR = $(TOP_DIR)/src
BUILD_DIR = $(TOP_DIR)/build
INC_DIR =  $(TOP_DIR)/include

CONF_ZSBL_PRINT      = 1
CONF_ZSBL_PRINT_SIMPLE     = 1
CONF_ZSBL_ONCHIP     = 1
CONF_ZSBL_BIST       = 1

CONF_CLOCK_IN_ZSBL   = 0

CONF_STORAGE_PROBE   = 0
CONF_STORAGE_PROBE_QUICK   = 1

CONF_BC_AT_FLASH_PART0 = 1

CONF_FSBL_PRINT      = 0
CONF_FSBL_PRINT_SIMPLE     = 1
CONF_FSBL_BIST       = 1

CONF_MC_ACTS_AS_PC   = 0
CONF_PC_ACTS_AS_MC   = 1

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

ifeq ($(CONF_ZSBL_ONCHIP), 1)
CFLAGS += -D ZSBL_ONCHIP
endif

ifeq ($(CONF_CLOCK_IN_ZSBL), 1)
CFLAGS += -D CLOCK_IN_ZSBL
endif

ifeq ($(CONF_STORAGE_PROBE), 1)
CFLAGS += -D STORAGE_PROBE
ifeq ($(CONF_STORAGE_PROBE_QUICK), 1)
CFLAGS += -D STORAGE_PROBE_QUICK
endif
endif

ifeq ($(CONF_BC_AT_FLASH_PART0), 1)
CFLAGS += -D BC_AT_FLASH_PART0
endif

ifeq ($(CONF_MC_ACTS_AS_PC), 1)
CFLAGS += -D MC_ACTS_AS_PC
endif

ifeq ($(CONF_PC_ACTS_AS_MC), 1)
CFLAGS += -D PC_ACTS_AS_MC
endif

LDFLAGS = -nostdlib
