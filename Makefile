CC = gcc
ASM = fasm

CFLAGS = -fno-omit-frame-pointer -rdynamic -O0 -Iinclude -Ilib

example:
	$(ASM) src/stack.asm obj/stack.o
	$(CC) $(CFLAGS) src/main.c obj/stack.o -o ./bin/example -ldl

clean:
	rm -rf obj/*.o bin/*