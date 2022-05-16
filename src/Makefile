OBJS =	framedisplay.o \
	mbaa_framedisplay.o \
	mbaa_character.o \
	mbaa_framedata.o \
	mbaa_arx.o \
	mbaa_cg2.o \
	mbaa_tileimage.o \
	mbaa_iso.o \
	mbaacc_framedisplay.o \
	mbaacc_character.o \
	mbaacc_framedata.o \
	mbaacc_cg.o \
	mbaacc_pack.o \
	mbtl_framedisplay.o \
	mbtl_character.o \
	mbtl_framedata.o \
	mbtl_cg.o \
	mbtl_pack.o \
	touhou_framedisplay.o \
	touhou_framedata.o \
	touhou_packfiles.o \
	touhou_images.o \
	abk_framedisplay.o \
	abk_framedata.o \
	abk_packfiles.o \
	abk_images.o \
	ougon_data.o \
	ougon_framedata.o \
	ougon_framedisplay.o \
	texture.o \
	clone.o \
	render.o

LINUX_OBJS = $(OBJS) simple_gui.o
WINDOWS_OBJS = $(OBJS) windows_gui.o
ALL_OBJS = $(OBJS) simple_gui.o windows_gui.o

PROJECT = framedisplay.exe

ifeq ($(OS),Windows_NT)
	OPENGL_HEADERS = /usr/mingw/i686-w64-mingw32/include/GL
else
	OPENGL_HEADERS = /usr/i686-w64-mingw32/include/GL
endif


CXX = i686-w64-mingw32-g++ -m32

CFLAGS_DEBUG = -s -g -Wall -Wno-narrowing -I"$(CURDIR)" -I"$(CURDIR)/../libpng" -I"$(CURDIR)/../libz" -I"$(CURDIR)/../SDL" -I$(OPENGL_HEADERS)
CFLAGS = -s -O3 -Os -Wall -Wno-narrowing -I"$(CURDIR)" -I"$(CURDIR)/../libpng" -I"$(CURDIR)/../libz" -I"$(CURDIR)/../SDL" -I$(OPENGL_HEADERS)

LIBS = -static -lmingw32 -lSDLmain -lSDL -lpng -lz -lopengl32 -lcomctl32 -lole32 -lwinmm -ldxguid

LDFLAGS = -mwindows -L"$(CURDIR)/../libpng" -L"$(CURDIR)/../libz" -L"$(CURDIR)/../SDL"

all: $(PROJECT)

clean:
	rm -f $(ALL_OBJS) framedisplay framedisplay.exe

.cpp.o:
	$(CXX) $(CFLAGS) -o $@ -c $<

framedisplay: $(SUBDIRS) $(LINUX_OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(LINUX_OBJS) $(LIBS)

framedisplay.exe: $(SUBDIRS) $(WINDOWS_OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(WINDOWS_OBJS) $(LIBS)

imageset: $(OBJS) imageset.o
	$(CXX) $(LDFLAGS) -o $@ $(OBJS) imageset.o $(LIBS)
