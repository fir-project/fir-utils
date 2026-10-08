CC ?= cc
CFLAGS ?= -std=c99 -Wall -Wextra -O2
CPPFLAGS ?= -Isrc -MMD -MP -D_POSIX_C_SOURCE=200809L

BIN := fir-parser
SRC := $(wildcard src/*.c)
OBJ := $(SRC:.c=.o)
DEP := $(OBJ:.o=.d)

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

test: $(BIN)
	./$(BIN) examples/config.fir -o /tmp/opencode/config.txt
	cat /tmp/opencode/config.txt

clean:
	rm -f $(OBJ) $(DEP) $(BIN)

-include $(DEP)

.PHONY: all test clean
