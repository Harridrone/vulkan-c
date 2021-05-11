
CC = clang
CFLAGS = -std=c89 -Wall -pedantic
LIBS = -lSDL2 -lvulkan

vulkan-c: main.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

test: vulkan-c
	./vulkan-c
