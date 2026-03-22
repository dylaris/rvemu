CC 	   = clang
CFLAGS = -ggdb -Wall -Wextra -O3 -Isrc/

EXE_CFLAGS  = $(CFLAGS)
EXE_LDFLAGS =

LIB_CFLAGS  = $(CFLAGS) -fPIC
LIB_LDFLAGS = -shared -Wl,--version-script=src/export.sym

all: rvemu lib

rvemu: src/one.c
	$(CC) $(EXE_CFLAGS) -o rvemu src/one.c $(EXE_LDFLAGS)

lib: src/api.c
	$(CC) $(LIB_CFLAGS) -o librvemu.so src/api.c $(LIB_LDFLAGS)

clean:
	rm -f rvemu librvemu.so

help:
	@echo "===================================================="
	@echo "Available Targets: (builds the exec and lib defaultly)"
	@echo "===================================================="
	@echo "help:  		Display this information"
	@echo "clean: 		Clean object files and executable file"
	@echo "lib: 		Generate shared library"
	@echo "===================================================="

.PHONY: all clean help lib
