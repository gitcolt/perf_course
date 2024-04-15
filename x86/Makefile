TARGET = main
SRCS = main.c
LIBS = -lm

.PHONY: all
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(LIBS) -o $@ $^

bin_view: bin_view.c
	$(CC) $(LIBS) -o $@ $^

test: test.c
	$(CC) -o $@ $^

debug: debug.c
	$(CC) -o $@ $^

.PHONY: clean
clean:
	rm -f $(TARGET) bin_view debug
