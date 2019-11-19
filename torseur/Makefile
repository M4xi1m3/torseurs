CC=gcc
CFLAGS=-I. -fsanitize=address -fno-omit-frame-pointer -g
DEPS = expression_node.h expression.h expression_solver.h expression_parser.h
OBJ = main.o expression_node.o expression.o expression_solver.o expression_parser.o


%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

torseur.elf: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	-rm $(OBJ) torseur.elf
