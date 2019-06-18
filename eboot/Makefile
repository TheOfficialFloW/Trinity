release: all
	./PrxEncrypter $(TARGET).prx
	pack-pbp PBOOT.PBP PARAMS.SFO ICON0.PNG NULL NULL NULL NULL DATA.PSP NULL

TARGET = trinity
OBJS = main.o rain.o mips.o arm.o krop.o rpc.o rop.o imports.o

CFLAGS = -O2 -Os -G0 -Wall -fshort-wchar -fno-pic -mno-check-zero-division
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBS = -lpspwlan

USE_PSPSDK_LIBC = 1
USE_PSPSDK_LIBS = 1

BUILD_PRX = 1

EXTRA_CLEAN = DATA.PSP PBOOT.PBP
PSP_EBOOT_TITLE = Trinity

PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
