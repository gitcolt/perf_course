TARGET = main
SRCS = main.c haversine.c json_gen.c
LIBS = -lm
CFLAGS = -g

.PHONY: all
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(LIBS) -o $@ $^

validator: validator.c
	$(CC) $(CFLAGS) $(LIBS) -o $@ $^

.PHONY: clean
clean:
	rm -f $(TARGET) *.json
