TARGET = main
SRCS = main.c sim86_lib.o
CC = clang

.PHONY: all
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) -g -o $@ $^

.PHONY: clean
clean:
	rm -f $(TARGET)
