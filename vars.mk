# MCU関連設定
DEVICE		= atmega328p
CLOCK		= 20000000
PROGRAMMER	= #-c stk500v2 -P avrdoper
HFUSE		= 0xd9
LFUSE		= 0xe7
XFUSE		= 0x07

# コンパイラ関連設定
CC			= avr-gcc
OBJCOPY		= avr-objcopy
OBJDUMP		= avr-objdump
SIZE		= avr-size
# コンパイラオプションなど
CFLAGS		= -Wall -Os -MMD -DF_CPU=$(CLOCK) -mmcu=$(DEVICE) -std=c99
LIBS		= -L.
INCLUDE		= 
# ブートローダを使用するとき、.hexファイルをブートローダに対応させるためのコマンド(まぁ使わんやろ)
HEX2BLHEX	= bootloadHID
# HIDaspxを使用するためのコマンド
HIDSPX		= hidspx

# ソースコードが入っているディレクトリ
SRCDIR		= $(CURDIR)/src
# 中間ファイルや生成物を入れるディレクトリ
BINDIR		= $(CURDIR)/bin

# コマンド関連設定(OS間差異吸収)
ifeq ($(OS), Windows_NT)
	# コマンド
	RM		= del /Q
	RMDIR	= rd /s /q
	MKDIR	= mkdir
	ECHO	= echo
	CD		= cd
	# パス変換('/'->'\')
	FixPath	= $(subst /,\,$1)
	# srcディレクトリ以下の全ファイル
	SOURCES	:= $(shell dir $(call FixPath,$(SRCDIR)) /s /b /a-d)
	SOURCES := $(subst \,/,$(SOURCES))
else
	# コマンド
	RM		= rm -f
	RMDIR	= rm -f -R
	MKDIR	= mkdir -p
	ECHO	= echo
	CD		= cd
	# パス変換(Windows以外では変換しない)
	FixPath	= $1
	# srcディレクトリ以下の全ファイル
	SOURCES	:= $(shell find $(SRCDIR) -type f -name '*')
endif

# 中間ファイルなどを入れるためのディレクトリ(ソースコード入りディレクトリの、src->binしたもの)
DIRS		:= $(patsubst $(SRCDIR)%,$(BINDIR)%,$(sort $(dir $(SOURCES))))

# 最終的にできるプログラムの名前．ディレクトリ名と同じになるよ．
PROGRAM		= $(notdir $(CURDIR))
# カレントディレクトリ以下にあるC言語のソースファイル名．
SOURCES		:= $(filter %.c,$(SOURCES))
# .c ファイルから生成される、.o ファイルの名前．
OBJECTS		= $(SOURCES:$(SRCDIR)/%.c=$(BINDIR)/%.o)
# ソースファイル事の依存関係を記したファイル．
DEPENDS		= $(SOURCES:$(SRCDIR)/%.c=$(BINDIR)/%.d)

# PROGRAMMER ... Options to avrdude which define the hardware you use for
#                uploading to the AVR and the interface where this hardware
#                is connected. We recommend that you leave it undefined and
#                add settings like this to your ~/.avrduderc file:
#                   default_programmer = "stk500v2"
#                   default_serial = "avrdoper"

# 2015/09/01現在のJansenのFuse値
# Device: ATmega328P
# Mode: ISP/Parallel
# High:11-11001 (0xd9)
#      |||||||+-- BOOTRST (1:Normal, 0:BootLoader)
#      |||||++-- BOOTSZ[1:0] (11:256W, 10:512, 01:1024, 00:2048)
#      ||||+-- EESAVE (消去でEEPROMを 1:消去, 0:保持)
#      |||+-- WDTON (1:WDT通常動作, 0:WDT常時ON)
#      ||+-- SPIEN (1:ISP禁止, 0:ISP許可) ※Parallel時のみ
#      |+-- DWEN (On-Chipデバッグ 1:無効, 0:有効)
#      +-- RSTDISBL (RESETピン 1:有効, 0:無効(PC6))
# Low: 11100111 (0xe7)
#      ||||++++-- CKSEL[3:0] システムクロック選択
#      ||++-- SUT[1:0] 起動時間
#      |+-- CKOUT (0:PB0にシステムクロックを出力)
#      +-- CKDIV8 クロック分周初期値 (1:1/1, 0:1/8)
# Ext: -----111 (0x07)
#           +++-- BODLEVEL[2:0] (111:無, 110:1.8V, 101:2.7V, 100:4.3V)
# the fuse bit calculator at http://www.engbedded.com/fusecalc/
