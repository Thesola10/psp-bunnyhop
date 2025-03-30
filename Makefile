BUILD_PRX = 1
TARGET = raylib
OBJS = controller.o level.o entity.o tileset.o sound.o main.o \
	   levels/lapinou.lvl.o
INCDIR =
CFLAGS = -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS =
LIBS =   -lraylib -lpng -lz -lglut -lGLU -lGL -lpspvfpu -lpspaudio -lpspaudiolib

EXTRA_TARGETS = EBOOT.PBP textures/bg-fs8.png textures/spriteset-fs8.png
PSP_EBOOT_TITLE = My Raylib Demo
PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

levels/%.lvl.c: levels/%.json levels/mkLevel.jsonnet
	jsonnet -S --tla-code-file lvl=$< -A name=$(patsubst levels/%.json,%,$<) levels/mkLevel.jsonnet > $@

textures/%-fs8.png: textures/%.png
	rm -f $@
	pngquant $^
