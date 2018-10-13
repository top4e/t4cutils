#CC=gcc
ECHO=@echo
MKDIR=@mkdir
RM=@rm
CP=@cp

ifeq ($(CONFIG_TARGET_NATIVE),y)
CC=@gcc
endif

#LDFLAGS += -s

install: $(LIB_SHARED) $(EXE) $(PLUGIN)
	@$(foreach file,$(LIB_SHARED),$(CP) -rf $(file) $(INSTALL_LIB_DIR);)
	@$(foreach file,$(EXE),$(CP) -rf $(file) $(INSTALL_BIN_DIR);)
	@$(foreach file,$(DATA),$(CP) -rf $(file) $(INSTALL_DATA_DIR);)
	@$(foreach file,$(PLUGIN),$(CP) -rf $(file) $(INSTALL_PLUGIN_DIR);)

-include $(obj-y:.o=.d)

$(LIB_SHARED): $(obj-y)
	$(ECHO) "[LD SHARED    ]***" $(LIB_SHARED)
	$(ECHO) "----------------------------------------------------------"
	$(CC) $(obj-y) $(LDFLAGS) $(LIBS) -o $@

$(PLUGIN): $(obj-y)
	$(ECHO) "[LD PLUGIN    ]***" $(PLUGIN)
	$(ECHO) "----------------------------------------------------------"
	$(CC) $(obj-y) $(LDFLAGS) $(LIBS) -o $@

$(EXE): $(obj-y)
	$(ECHO) "[LD EXECUTABLE]***" $(EXE)
	$(ECHO) "----------------------------------------------------------"
	$(CC) $(obj-y) $(LDFLAGS) $(EXE_LIBS) -o $@

# pull in dependency info for *existing* .o files
%.o: %.c # combined w/ next line will compile recently changed .c files
	$(ECHO) "[CC  " $(LIB_SHARED)$(PLUGIN)$(EXE) "  ] " $(notdir $<)
	$(CC) $(CFLAGS) -o $@ -c $< $(INCLUDES)
	$(CC) -MM $(CFLAGS) $*.c > $*.d
	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp

clean:
	$(ECHO) Cleaning $(EXE) $(HOST_EXE) $(LIB_SHARED) $(PLUGIN)
	$(RM) -f $(EXE) $(HOST_EXE) $(LIB_SHARED) $(PLUGIN)
	@find . -name "*.o" -exec rm {} \;
	@find . -name "*.d" -exec rm {} \;
