# Name: Makefile
# Author: <insert your name here>
# Copyright: <insert your copyright message here>
# License: <insert your license reference here>

# This is a prototype Makefile. Modify it according to your needs.
# You should at least check the settings for
# DEVICE ....... The AVR device you compile for
# CLOCK ........ Target AVR clock rate in Hertz
# OBJECTS ...... The object files created from your source files. This list is
#                usually the same as the list of source files with suffix ".o".
# PROGRAMMER ... Options to avrdude which define the hardware you use for
#                uploading to the AVR and the interface where this hardware
#                is connected. We recommend that you leave it undefined and
#                add settings like this to your ~/.avrduderc file:
#                   default_programmer = "stk500v2"
#                   default_serial = "avrdoper"
# FUSES ........ Parameters for avrdude to flash the fuses appropriately.

DEVICE     = atmega8
CLOCK      = 8000000
PROGRAMMER = #-c stk500v2 -P avrdoper
# OBJECTS    = main.o
FUSES      = -U hfuse:w:0xd9:m -U lfuse:w:0x24:m

# ATMega8 fuse bits used above (fuse bits for other devices are different!):
# Example for 8 MHz internal oscillator
# Fuse high byte:
# 0xd9 = 1 1 0 1   1 0 0 1 <-- BOOTRST (boot reset vector at 0x0000)
#        ^ ^ ^ ^   ^ ^ ^------ BOOTSZ0
#        | | | |   | +-------- BOOTSZ1
#        | | | |   +---------- EESAVE (set to 0 to preserve EEPROM over chip erase)
#        | | | +-------------- CKOPT (clock option, depends on oscillator type)
#        | | +---------------- SPIEN (if set to 1, serial programming is disabled)
#        | +------------------ WDTON (if set to 0, watchdog is always on)
#        +-------------------- RSTDISBL (if set to 0, RESET pin is disabled)
# Fuse low byte:
# 0x24 = 0 0 1 0   0 1 0 0
#        ^ ^ \ /   \--+--/
#        | |  |       +------- CKSEL 3..0 (8M internal RC)
#        | |  +--------------- SUT 1..0 (slowly rising power)
#        | +------------------ BODEN (if 0, brown-out detector is enabled)
#        +-------------------- BODLEVEL (if 0: 4V, if 1: 2.7V)
#
# For computing fuse byte values for other devices and options see
# the fuse bit calculator at http://www.engbedded.com/fusecalc/


# Tune the lines below only if you know what you are doing:

# AVRDUDE = avrdude $(PROGRAMMER) -p $(DEVICE)
# COMPILE = avr-gcc -Wall -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE)
CC			= avr-gcc
OBJCOPY		= avr-objcopy
SIZE		= avr-size


# Commands
ifeq ($(OS), Windows_NT)
	RM		= del /Q
	RMDIR	= rd /s /q
	MKDIR	= mkdir
	ECHO	= echo
	CD		= cd
	FixPath	= $(subst /,\,$1)
else
	RM		= rm -f
	RMDIR	= rm -f -R
	MKDIR	= mkdir -p
	ECHO	= echo
	CD		= cd
	FixPath	= $1
endif

# 生成物などを入れるためのディレクトリ
BUILDDIR	= $(CURDIR)/build

# 最終的にできるプログラムの名前。ディレクトリ名と同じになるよ。
PROGRAM		= $(call FixPath, $(notdir $(CURDIR)))
# カレントディレクトリにあるC言語のソースファイル名。
SOURCES		= $(call FixPath, $(wildcard *.c))
# .c ファイルから生成される、.o ファイルの名前。
OBJECTS		= $(call FixPath, $(SOURCES:%.c=$(BUILDDIR)/%.o))
# ソースファイル事の依存関係を記したファイル。
DEPENDS		= $(call FixPath, $(SOURCES:%.c=$(BUILDDIR)/%.d))

CFLAGS		= -Wall -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE)
LIBS		= -L.
INCLUDE		= \

