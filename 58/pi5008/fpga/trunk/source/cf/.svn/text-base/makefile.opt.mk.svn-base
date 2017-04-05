#ANDESIGHT_ROOT  = /cygdrive/C/Andestech/AndeSight211STD
ANDESIGHT_ROOT  = /cygdrive/C/Andestech/BSPv410

CROSS           = nds32le-elf-
CC              = $(CROSS)gcc
OD              = $(CROSS)objdump
OC              = $(CROSS)objcopy
AR              = ar
RANLIB          = ranlib
RM              = rm -rf

LIB_DIR         = $(TOP_DIR)/lib
INC_DIR         = $(TOP_DIR)/include
BIN_DIR         = $(TOP_DIR)/workspace/bin
OBJ_DIR         = obj
DEP_DIR         = dep

CFLAGS          = -mcmodel=medium
#CFLAGS        += -g3
CFLAGS         += -Wall
CFLAGS         += -mcpu=d1088-spu
CFLAGS         += -c
CFLAGS         += -fmessage-length=0
CFLAGS         += -fsingle-precision-constant
#CFLAGS         += -Wfatal-errors
CFLAGS         += -Os
CFLAGS         += -D__FPGA

LDFLAGS         = -nostartfiles
LDFLAGS        += -static
LDFLAGS        += -mcmodel=medium
LDFLAGS        += -mext-dsp
LDFLAGS        += -mext-zol
LDFLAGS        += -mvh

OBJ_EXT         = .o
DEP_EXT         = .d

$(OBJ_DIR)/%$(OBJ_EXT):%.c
	$(MUTE)\
	mkdir -p $(dir $@); \
	$(CC) $(CFLAGS) -c $< -o $@
	@echo 'compiled ....$@'

$(OBJ_DIR)/%$(OBJ_EXT):%.S
	$(MUTE)\
	mkdir -p $(dir $@); \
	$(CC) $(CFLAGS) -c $< -o $@
	@echo 'compiled ....$@'

$(DEP_DIR)/%$(DEP_EXT):%.c
	$(MUTE)\
	mkdir -p $(dir $@); \
	$(CC) $(CFLAGS) -MM -MG \
	-MT $@ -MT $(patsubst %.c,$(OBJ_DIR)/%.o,$<) -MF $@ $<
	@echo 'dependency check ... $@'