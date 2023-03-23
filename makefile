compiler := gcc

sources := $(wildcard \
             *.c \
            )

myexe := FirstSDL2

objects := $(sources:%.c=%.o) 
objects := $(objects) $(wildcard Gfx/*.o) $(wildcard Sfx/*.o)

CFLAGS = -Wall \
         -c \

libs :=  -lm \
	 -lSDL2main \
	 -lSDL2 \


all: $(objects)
	rm -f $(myexe)
	@echo "Linking ...."
	$(compiler) $+ -s $(libs) $(libincludes) -Wl,-Map,$(myexe).map -o bin/Release/$(myexe)
%.o: %.c
	$(compiler) -o $@  $(CFLAGS) $(includes) $<

clean:
	rm -f *.o
	rm -f bin/Release/$(myexe)

