


CC=gcc
CFLAGS=-Wall -Wextra -std=c99


TARGETS=battleships


build: $(TARGETS)

battleships: battleships.c
	$(CC) $(CFLAGS) battleships.c -o battleships -lm

clean:
	rm -f $(TARGETS)

.PHONY: pack clean
