CC=gcc
CFLAGS= -c -Os -Wall -Werror -Wno-unused-value -Wextra
SOURCES=main.c gf25519.c utils.c modl.c ristretto255.c
OBJECTS=$(SOURCES:.c=.o)
EXECUATBLE=main
all: $(SOURCES) $(EXECUATBLE)
$(EXECUATBLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@
clean:
	del /f /s $(OBJECTS)
