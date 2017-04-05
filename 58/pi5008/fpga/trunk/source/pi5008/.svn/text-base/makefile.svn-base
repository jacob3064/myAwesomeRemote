TOP_DIR         = ..
CF_DIR          = $(TOP_DIR)/cf

include $(CF_DIR)/makefile.opt.mk

INCLUDE_DIRS    = -I.   	   \
	              -I$(INC_DIR)

CFLAGS         += $(INCLUDE_DIRS)

LIB             = $(LIB_DIR)/libpi5008.a

CSRC            = $(wildcard *.c)
SSRC            = $(wildcard *.S)

OBJS            = $(patsubst %.S, %.o, $(addprefix $(OBJ_DIR)/, $(SSRC)))
OBJS           += $(patsubst %.c, %.o, $(addprefix $(OBJ_DIR)/, $(CSRC)))
DEPS            = $(patsubst %.c, %.d, $(addprefix $(DEP_DIR)/, $(CSRC)))

build::$(LIB)

$(LIB):$(OBJS)
	$(AR) -ur $@ $^
	$(RANLIB) $@

clean distclean:
	$(RM) $(OBJ_DIR)/*
	$(RM) $(DEP_DIR)/*
	$(RM) $(OBJ_DIR)
	$(RM) $(DEP_DIR)
	$(RM) $(LIB)

ifeq ($(MAKECMDGOALS),build)
include $(DEPS)
endif

.PHONY:: build clean distclean
