BUILD_PRX = 1
TARGET = raylib
OBJS = main.o controller.o
INCDIR =
CFLAGS = -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS =
LIBS =   -lraylib -lpng -lz -lglut -lGLU -lGL -lpspvfpu

EXTRA_TARGETS = EBOOT.PBP textures/bg-fs8.png
PSP_EBOOT_TITLE = My Raylib Demo
PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

textures/%-fs8.png: %.png
	rm -f $@
	pngquant $^
