CC 	     = clang
CFLAGS   = -ggdb -Wall -Wextra -O0 -Iinc/ -DTEST_TVM
CLDFLAGS = -lm

EXE_CFLAGS  = $(CFLAGS)
EXE_LDFLAGS = $(CLDFLAGS)

LIB_CFLAGS  = $(CFLAGS) -fPIC
LIB_LDFLAGS = $(CLDFLAGS) -shared -Wl,--version-script=src/export.sym

all: rvemu

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
