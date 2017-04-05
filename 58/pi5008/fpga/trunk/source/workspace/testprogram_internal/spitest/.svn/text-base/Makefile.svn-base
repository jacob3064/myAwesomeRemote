export MUTE      = @

####### [adx filename & c file list] #######
EXECUTABLE      = spitest

CSRCS           = spitest.c \
############################################

TOP_DIR         = ../../..
CF_DIR          = $(TOP_DIR)/cf
PI5008_DIR      = $(TOP_DIR)/pi5008

include $(CF_DIR)/makefile.opt.mk

INCLUDE_DIRS    = -I.   	   \
	              -I$(INC_DIR)

SAG_SRCS        = $(PI5008_DIR)/pixelplus.sag
LD_FILE         = $(PI5008_DIR)/nds32.ld

LIBS           += $(LIB_DIR)/libpi5008.a

CFLAGS         += $(INCLUDE_DIRS)
CFLAGS         += -Og

LDFLAGS        += -T"$(LD_FILE)"
LDFLAGS        += $(LIBS)

include $(CF_DIR)/makefile.common.mk
