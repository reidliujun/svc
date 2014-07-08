# Project: lib_view
# Makefile created by Dev-C++ 4.9.9.2


OBJ  = sdl.o extract_picture.o WriteYUV.o
BIN  = ../bin/libviewsdl.a
INCS = -I"./include"

RM = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before $(BIN) all-after


clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	ar r $(BIN) $(OBJ)
	ranlib $(BIN)

sdl.o: sdl.c
	$(CC) -c sdl.c -o sdl.o $(INCS)

extract_picture.o: extract_picture.c
	$(CC) -c extract_picture.c -o extract_picture.o $(INCS)

WriteYUV.o: WriteYUV.c
	$(CC) -c WriteYUV.c -o WriteYUV.o $(INCS)