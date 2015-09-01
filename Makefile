include vars.mk

# タスクターゲット
.PHONY:	all clean

# symbolic targets:
all: $(BUILDDIR) $(DEPENDS) $(BUILDDIR)/$(PROGRAM).hex

$(BUILDDIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $(call FixPath, $<) -o $(call FixPath, $@)

# .S.o:
# 	$(COMPILE) -x assembler-with-cpp -c $< -o $@
# "-x assembler-with-cpp" should not be necessary since this is the default
# file type for the .S (with capital S) extension. However, upper case
# characters are not always preserved on Windows. To ensure WinAVR
# compatibility define the file type manually.

# .c.s:
# 	$(COMPILE) -S $< -o $@

flash:	all
	$(AVRDUDE) -U flash:w:main.hex:i

fuse:
	$(AVRDUDE) $(FUSES)

# Xcode uses the Makefile targets "", "clean" and "install"
install: flash fuse

# if you use a bootloader, change the command below appropriately:
load: all
	bootloadHID main.hex

clean:
	$(RMDIR) $(call FixPath, $(BUILDDIR))

# file targets:
$(BUILDDIR)/$(PROGRAM).elf: $(OBJECTS)
	$(CC) $(CFLAGS) -o $(call FixPath, $@) $(call FixPath, $^)
	# $(COMPILE) -o $@ $(OBJECTS)

$(BUILDDIR)/$(PROGRAM).hex: $(BUILDDIR)/$(PROGRAM).elf
	$(RM) $(call FixPath, $@)
	$(OBJCOPY) -j .text -j .data -O ihex $(call FixPath, $<) $(call FixPath, $@)
	$(SIZE) --format=avr --mcu=$(DEVICE) $(call FixPath, $<)
# If you have an EEPROM section, you must also create a hex file for the
# EEPROM and add it to the "flash" target.

# Targets for code debugging and analysis:
disasm:	main.elf
	avr-objdump -d main.elf

cpp:
	$(COMPILE) -E main.c

$(BUILDDIR):
	$(MKDIR) $(call FixPath, $@)

$(BUILDDIR)/%.d: %.c
	$(CC) -MM $(CFLAGS) $(call FixPath, $<) > $(call FixPath, $@)
