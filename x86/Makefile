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

main2: main2.c inst.c
	$(CC) -o $@ $^ -lm

unit_test: unit_test.c
	$(CC) -o $@ $^

.PHONY: clean
clean:
	rm -f $(TARGET) bin_view debug main2 unit_test
