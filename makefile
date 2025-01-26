CC := gcc
FLAGS := `sdl2-config --libs --cflags` --std=gnu18 -lm -Wall
HDRS := $(wildcard *.h)
SRCS := $(wildcard *.c)
OBJS := $(SRCS:%.c=tmp/%.o) $(wildcard gfx/*.o) $(wildcard sfx/*.o)
EXEC := bin/Release/FirstSDL2

.PHONY: all

all: $(EXEC)

$(EXEC): $(OBJS) $(HDRS)
	$(CC) -o $@ $(OBJS) $(FLAGS) && echo "EXEC [OK]  $@"


tmp/%.o: %.c
	@mkdir -p tmp
	@$(CC) $(FLAGS) -c $< -o $@ && echo "tmp/%.o: %.c [OK]  $@"


.PHONY: clean, clear

clean clear:
	@rm -f bin/Release/FirstSDL2 && echo "clean FirstSDL2"
	@rm -f tmp/* && echo "clean tmp"
	@rm -f gfx/*.o gfx/*.h gfx/creategfx gfx/objectzipper gfx/bin2c && echo "clean gfx"
	@rm -f sfx/*.o sfx/*.h sfx/createsfx sfx/objectzipper sfx/bin2c && echo "clean sfx"
	@rm -f creategfx/creategfx creategfx/*.o createsfx/createsfx createsfx/*.o objectzipper/objectzipper objectzipper/*.o bin2c/bin2c bin2c/*.o && echo "clean creategfx/createsfx/objectzipper/bin2c"

