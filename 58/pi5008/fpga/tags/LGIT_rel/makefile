ifndef ANDESIGHT_ROOT
ANDESIGHT_ROOT=/cygdrive/C/Andestech/AndeSight211STD
endif

APP_TARGET = outfile
LD_FILE_NAME = nds32.ld
SAG_SRCS = pixelplus.sag

CROSS=nds32le-elf-
#CROSS=mips-linux-gnu-

CC=$(CROSS)gcc
#LD=$(CROSS)ld
OD=$(CROSS)objdump
OC=$(CROSS)objcopy
NM=$(CROSS)nm

RM=rm -f

INC_DIR = -I./include
INC_DIR += -I./src

CFLAGS = -Os
CFLAGS += -mcmodel=medium
#CFLAGS += -g3
CFLAGS += -Wall
CFLAGS += -mcpu=d1088-spu
CFLAGS += -c
CFLAGS += -fmessage-length=0
CFLAGS += -fsingle-precision-constant


LDFLAGS  = -nostartfiles
LDFLAGS += -static
LDFLAGS += -mcmodel=medium
LDFLAGS += -mext-dsp
LDFLAGS += -mext-zol
LDFLAGS += -T"$(LD_FILE_NAME)"
LDFLAGS += -mvh

CSRC += $(wildcard ./src/*.c)

SSRC += $(wildcard ./src/*.S)


SRCOBJS := $(SSRC:.S=.o) $(CSRC:.c=.o)

#asic: BUILD
#BUILD: $(SRCOBJS)

all: BUILD

BUILD: $(SRCOBJS)
	$(ANDESIGHT_ROOT)/utils/nds_ldsag -t $(ANDESIGHT_ROOT)/utils/nds32_template.txt $(SAG_SRCS) -o $(LD_FILE_NAME)
	$(CROSS)gcc $(LDFLAGS) $(SRCOBJS) -o $(APP_TARGET).adx
	$(CROSS)objdump -x -d -C $(APP_TARGET).adx > $(APP_TARGET).dasm
	$(CROSS)objcopy -S -O binary $(APP_TARGET).adx $(APP_TARGET).bin
	
.c.o:
	$(CC) $(INC_DIR) $(CFLAGS) $< -o $@
	
.S.o:
	$(CC) $(INC_DIR) $(CFLAGS) $< -o $@

	
clean: SRCOBJS:=$(SSRC:.S=.o) $(CSRC:.c=.o)
clean:
	$(RM) $(APP_TARGET).adx
	$(RM) $(APP_TARGET).dasm
	$(RM) $(APP_TARGET).bin
	$(RM) $(SRCOBJS)
