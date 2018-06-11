CC = clang
PREFIX ?= /usr/local
CFLAGS ?= -std=c11 -Wall -Wextra -pedantic -I.
DEPS = src/instructions.h src/components.h
OBJ = obj/instructions.o obj/components.o

TARGET := cjello

.PHONY: all run install clean

all: bin/$(TARGET) $(OBJ)

obj/%.o: src/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

bin/$(TARGET): src/$(TARGET).c $(OBJ)
	$(CC) $(CFLAGS) -o bin/$(TARGET) src/$(TARGET).c $(OBJ)

run: bin/$(TARGET)
	./bin/$(TARGET) $(ARGS)

install: bin/$(TARGET)
	mkdir -p $(PREFIX)/bin
	cp bin/$^ $(PREFIX)/bin

clean:
	$(RM) $(TARGET) $(OBJ)
	$(RM) bin/$(TARGET)
	cd obj && $(RM) $(OBJ)
