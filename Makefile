include vars.mk

.PHONY:	all clean cmake

# メインターゲット
all: $(BUILDDIR) $(BUILDDIR)/$(PROGRAM).hex

# 中間ファイルなどを全て消し、makeする前の状態に戻します
clean:
	$(RMDIR) $(call FixPath, $(BUILDDIR))

# cleanしてからmake
cmake: clean all

# 中間ファイルなどを置くディレクトリを作成する
$(BUILDDIR):
	$(MKDIR) $(call FixPath, $@)

# .c -> .o
$(BUILDDIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $(call FixPath, $<) -o $(call FixPath, $@)

# .o -> .elf
$(BUILDDIR)/$(PROGRAM).elf: $(OBJECTS)
	$(CC) $(CFLAGS) -o $(call FixPath, $@) $(call FixPath, $^)

# .elf -> .hex
$(BUILDDIR)/$(PROGRAM).hex: $(BUILDDIR)/$(PROGRAM).elf
	$(RM) $(call FixPath, $@)
	$(OBJCOPY) -j .text -j .data -O ihex $(call FixPath, $<) $(call FixPath, $@)
	$(SIZE) --format=avr --mcu=$(DEVICE) $(call FixPath, $<)

# If you have an EEPROM section, you must also create a hex file for the
# EEPROM and add it to the "flash" target.
flash:	all
	$(AVRDUDE) $(PROGRAMMER) -p $(DEVICE) -U flash:w:$(call FixPath, $(BUILDDIR)/$(PROGRAM).hex):i

fuse:
	$(AVRDUDE) $(PROGRAMMER) -p $(DEVICE) -U hfuse:w:$(HFUSE):m -U lfuse:w:$(LFUSE):m

# Xcode uses the Makefile targets "", "clean" and "install"
install: flash fuse

# if you use a bootloader, change the command below appropriately:
load: all
	$(HEX2BLHEX) $(call FixPath, $(BUILDDIR)/$(PROGRAM).hex)

# Targets for code debugging and analysis:
disasm:	$(BUILDDIR)/$(PROGRAM).elf
	$(OBJDUMP) -d $(call FixPath, $<)

-include $(DEPENDS)