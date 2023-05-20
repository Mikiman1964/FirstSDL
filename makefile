CC := gcc
FLAGS := `sdl2-config --libs --cflags` --std=gnu18 -lm -Wall
HDRS := $(wildcard *.h)
SRCS := $(wildcard *.c)
OBJS := $(SRCS:%.c=tmp/%.o) $(wildcard Gfx/*.o) $(wildcard Sfx/*.o)
EXEC := bin/Release/FirstSDL2

.PHONY: all

all: $(EXEC)

$(EXEC): $(OBJS) $(HDRS)
	$(CC) -o $@ $(OBJS) $(FLAGS) && echo "EXEC [OK]  $@"


tmp/%.o: %.c
	@$(CC) $(FLAGS) -c $< -o $@ && echo "tmp/%.o: %.c [OK]  $@"


.PHONY: clean, clear

clean clear:
	@rm -f bin/Release/FirstSDL2 && echo "[CL]  out/"
	@rm -f tmp/* && echo "[CL]  tmp/"


