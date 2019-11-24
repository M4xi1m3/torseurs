CC		= gcc
CFLAGS	= -I. -fsanitize=address -fno-omit-frame-pointer -ffast-math -g -Wall -Werror
DEPS	= $(wildcard inc/*.c)
SRC		= $(wildcard src/*.c)
OBJ		= $(SRC:.c=.o)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

torseur.elf: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	-rm $(OBJ) torseur.elf
