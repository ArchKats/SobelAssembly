SHELL:=/bin/bash
CC:=gcc
OBJECTS:=bmptogray.o sobel_detection.o border_pixel_calc.o
C_FLAGS:=-Wall -lm -m32 -no-pie
G_FLAGS:=
EXEC:=exec
OBJDIR:=obj/
ASMDIR:=asm/
OUTDIR:=../
THRESHOLD:=$(shell grep "#define" main.cpp | cut -d " " -f3)

OBJECTS:=$(addprefix $(OBJDIR),$(OBJECTS))

all: $(OBJECTS)
	$(CC) $(C_FLAGS) $(G_FLAGS) main.cpp $(OBJECTS) -o $(OUTDIR)$(EXEC)

debug: G_FLAGS+=-g
debug: EXEC:=debug
debug: clean all

obj/%.o: $(ASMDIR)%.nasm main.cpp
	nasm -f elf32 $(G_FLAGS) -DTHRESHOLD=$(THRESHOLD) -o $@ $<

clean: 
	@rm -f $(OBJECTS)
	@cd $(OUTDIR); \
	rm -f exec debug Output*.bmp

