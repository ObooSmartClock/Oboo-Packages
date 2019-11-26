# find the os
UNAME_S := $(shell uname -s)

BUILDDIR := build
BINDIR := bin

# define common variables
SRCEXT := c
#SOURCES := main.c
#OBJECTS := main.o
SOURCES := $(shell find . -type f -iname "*.$(SRCEXT)" )
OBJECTS := $(patsubst %,%,$(SOURCES:.$(SRCEXT)=.o))
INC := -I . -I duktape
LIB := -lm -lcurl -lmosquitto -pthread


TARGET := ort

ifeq ($(UNAME_S),Darwin)
	# only add this when compiling on OS X
	CFLAGS += -D MAC_DEBUG=1
endif

test: CFLAGS += -D MAC_DEBUG=1 -g -O0 -ggdb3

all: info $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(BINDIR)
	@echo " Linking..."
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -o $(BINDIR)/$(TARGET) $(LIB)


# generic: build any object file required
%.o: %.c
	@echo " Compiling"
	$(CC) $(CFLAGS) $(INC) $(LIB) -c -o $@ $<

clean:
	rm -rf $(BINDIR) $(OBJECTS)

test: all
	service mosquitto start
	cp bin/ort .
	./ort ../test/buffer.js

info:
	@echo "INFO!"
	@echo "SOURCES=$(SOURCES)"
	@echo "OBJECTS=$(OBJECTS)"
	@echo "CC=$(CC)"
	@echo "CFLAGS=$(CFLAGS)"
	@echo "LDFLAGS=$(LDFLAGS)"
	@echo "LIB=$(LIB)"
