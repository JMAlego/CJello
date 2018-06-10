CC = clang
PREFIX ?= /usr/local
CFLAGS ?= -std=c11 -Wall -Wextra -pedantic -I.
DEPS = instructions.h components.h
OBJ = instructions.o components.o

TARGET := cjello

.PHONY: all run install clean

all: $(TARGET) $(OBJ)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(TARGET).c $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c $(OBJ)
	mv *.o obj/
	mv $(TARGET) bin/$(TARGET)

run: $(TARGET)
	./$(TARGET) $(ARGS)

install: $(TARGET)
	mkdir -p $(PREFIX)/bin
	cp $^ $(PREFIX)/bin

clean:
	$(RM) $(TARGET) $(OBJ)
	$(RM) bin/$(TARGET)
	cd obj && $(RM) $(OBJ)
