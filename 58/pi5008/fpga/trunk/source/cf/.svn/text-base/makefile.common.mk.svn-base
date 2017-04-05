ADX_EXT         = .adx
DASM_EXT        = .dasm
BIN_EXT         = .bin

OBJS           += $(patsubst %.c, %.o, $(addprefix $(OBJ_DIR)/, $(CSRCS)))
DEPS           += $(patsubst %.c, %.d, $(addprefix $(DEP_DIR)/, $(CSRCS)))

build::$(EXECUTABLE)$(ADX_EXT)
	$(OD) -x -d -C $(EXECUTABLE)$(ADX_EXT) > $(EXECUTABLE)$(DASM_EXT)
	$(OC) -S -O binary $(EXECUTABLE)$(ADX_EXT) $(EXECUTABLE)$(BIN_EXT)
	mkdir -p $(BIN_DIR)
	cp $(EXECUTABLE)$(ADX_EXT) $(BIN_DIR)
	cp $(EXECUTABLE)$(DASM_EXT) $(BIN_DIR)
	cp $(EXECUTABLE)$(BIN_EXT) $(BIN_DIR)

$(EXECUTABLE)$(ADX_EXT):$(LIBS) $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(EXECUTABLE)$(ADX_EXT)

$(LIBS):
	$(ANDESIGHT_ROOT)/utils/nds_ldsag -t $(ANDESIGHT_ROOT)/utils/nds32_template.txt $(SAG_SRCS) -o $(LD_FILE)
	$(MAKE) -C $(PI5008_DIR) build

clean distclean::
	$(RM) $(EXECUTABLE)$(ADX_EXT)
	$(RM) $(EXECUTABLE)$(DASM_EXT)
	$(RM) $(EXECUTABLE)$(BIN_EXT)
	$(RM) $(BIN_DIR)/$(EXECUTABLE)$(ADX_EXT)
	$(RM) $(BIN_DIR)/$(EXECUTABLE)$(DASM_EXT)
	$(RM) $(BIN_DIR)/$(EXECUTABLE)$(BIN_EXT)
	$(RM) $(OBJ_DIR)/*
	$(RM) $(DEP_DIR)/*
	$(RM) $(OBJ_DIR)
	$(RM) $(DEP_DIR)
	$(RM) $(LD_FILE)

distclean::
	$(MAKE) -C $(PI5008_DIR) $@

ifeq ($(MAKECMDGOALS),)
include $(DEPS)
endif

.PHONY:: build clean distclean
