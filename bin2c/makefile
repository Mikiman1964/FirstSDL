CC := gcc
FLAGS := -lm -Wall
HDRS := $(wildcard *.h)
SRCS := $(wildcard *.c)
OBJS := $(SRCS:%.c=%.o)
EXEC := bin2c

.PHONY: all

all: $(EXEC)

$(EXEC): $(OBJS) $(HDRS)
	$(CC) -o $@ $(OBJS) $(FLAGS) && echo "EXEC [OK]  $@"


%.o: %.c
	@$(CC) $(FLAGS) -c $< -o $@ && echo "%.o: %.c [OK]  $@"


.PHONY: clean, clear

clean clear:
	@rm -f *.o bin2c && echo "clear *.o bin2c"


