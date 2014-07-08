# Project: SVC
# Modified by Mederic Blestel <mblestel@insa-rennes.fr>

DEBUG = no

OBJ  = obj/PC.o
LINKOBJ  = obj/PC.o 
INCS = -I"../../AVC/h264_baseline_decoder/lib_baseline" -I"../../h264_baseline_decoder/lib_baseline" -I"../../AVC/h264_main_decoder/lib_main" -I"../lib_svc" -I"/usr/include/SDL"
BIN  = ../bin/svc


LIBS = -L"../../AVC/h264_baseline_decoder/lib_baseline" -L"../../AVC/h264_main_decoder/lib_main" -L"../lib_svc" -L"../bin/" -lviewsdl -lSDL -lsvc -lmain -lbaseline
ifeq ($(DEBUG),yes)
	CXFLAGS = -g 
else	
	CXFLAGS = -O3 -fomit-frame-pointer -frerun-loop-opt -fgcse -fexpensive-optimizations -finline-functions -mmmx -msse -msse2 -msse3 -fstrict-aliasing
endif



export CFLAGS = $(INCS) -DDISPLAY -DHAVE_SDL $(CXFLAGS)


RM = rm -f
.PHONY: all all-before all-after clean clean-custom


all: all-before $(BIN) all-after

clean: clean-custom
	@(cd "../../AVC/h264_baseline_decoder/lib_baseline" && $(MAKE) clean)
	@(cd "../../AVC/h264_main_decoder/lib_main" && $(MAKE) clean)
	@(cd "../libview" && $(MAKE) clean)
	@(cd "../lib_svc" && $(MAKE) clean)
	${RM} $(OBJ) $(BIN)
	rm -rf obj

$(BIN): $(OBJ)
	@(cd "../../AVC/h264_baseline_decoder/lib_baseline" && $(MAKE))
	@(cd "../../AVC/h264_main_decoder/lib_main" && $(MAKE))
	@(cd "../lib_svc" && $(MAKE))
	@(cd "../libview" && $(MAKE))
	$(CC) $(LINKOBJ) -o $(BIN) $(LIBS)

obj/PC.o: PC.c
	mkdir -p obj
	$(CC) -c PC.c -o obj/PC.o $(CFLAGS)

