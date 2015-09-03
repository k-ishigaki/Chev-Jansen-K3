include vars.mk

.PHONY:	all clean cmake

# メインターゲット
all: $(DIRS) $(BINDIR)/$(PROGRAM).hex

# 中間ファイルなどを全て消し、makeする前の状態に戻します
clean:
	$(RMDIR) $(call FixPath, $(BINDIR))

# cleanしてからmake
cmake: clean all

# 中間ファイルなどを置くディレクトリを作成する
$(BINDIR)/%: $(SRCDIR)/%
	$(MKDIR) $(call FixPath, $@)

# .c -> .o
$(BINDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $(call FixPath, $<) -o $(call FixPath, $@)

# .o -> .elf
$(BINDIR)/$(PROGRAM).elf: $(OBJECTS)
	$(CC) $(CFLAGS) -o $(call FixPath, $@) $(call FixPath, $^)

# .elf -> .hex
%.hex: %.elf
	$(RM) $(call FixPath, $@)
	$(OBJCOPY) -j .text -j .data -O ihex $(call FixPath, $<) $(call FixPath, $@)
	$(SIZE) --format=avr --mcu=$(DEVICE) $(call FixPath, $<)

# If you have an EEPROM section, you must also create a hex file for the
# EEPROM and add it to the "flash" target.
flash:	all
	$(HIDSPX) $(call FixPath, $(BUILDDIR)/$(PROGRAM).hex)
#	$(AVRDUDE) $(PROGRAMMER) -p $(DEVICE) -U flash:w:$(call FixPath, $(BUILDDIR)/$(PROGRAM).hex):i

fuse:
	$(HIDSPX) -fH$(HFUSE) -fL$(LFUSE) -fX$(XFUSE)
#	$(AVRDUDE) $(PROGRAMMER) -p $(DEVICE) -U hfuse:w:$(HFUSE):m -U lfuse:w:$(LFUSE):m

# Xcode uses the Makefile targets "", "clean" and "install"
install: flash fuse

# if you use a bootloader, change the command below appropriately:
load: all
	$(HEX2BLHEX) $(call FixPath, $(BUILDDIR)/$(PROGRAM).hex)

# Targets for code debugging and analysis:
disasm:	$(BINDIR)/$(PROGRAM).elf
	$(OBJDUMP) -d $(call FixPath, $<)

-include $(DEPENDS)