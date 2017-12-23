
CC = gcc
CFLAGS = -g3 -O0 -rdynamic -Wall
SHARED = -fPIC --shared

cmap.so: src/lua-map.c src/map.c src/pathfind.c src/heapq.c src/config.c src/hash.c
	$(CC) $(CFLAGS) $(SHARED) -Iinclude $^ -o $@ -lm

clean:
	rm cmap.so