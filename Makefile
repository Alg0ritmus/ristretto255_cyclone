CC=gcc
CFLAGS= -c -Os -Wall -Wno-unused-function -Werror -Wextra
# NOTE: You can set various additional internal
# flags in config.h
SOURCES=main.c gf25519.c utils.c modl.c ristretto255.c
OBJECTS=$(SOURCES:.c=.o)
EXECUATBLE=main
all: $(SOURCES) $(EXECUATBLE)
$(EXECUATBLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@
clean:
	rm -f -r $(OBJECTS)
