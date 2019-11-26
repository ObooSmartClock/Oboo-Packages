BUILDDIR := build
BINDIR := bin

# define common variables
# added for debug
#CFLAGS := -g
SRCEXT := c
#SOURCES := main.c
#OBJECTS := main.o
SOURCES := $(shell find . -type f -iname "*.$(SRCEXT)" )
OBJECTS := $(patsubst %,%,$(SOURCES:.$(SRCEXT)=.o))
INC := -I . -I lvgl -I oboo_gui -I lv_drivers -I duktape
LIB := -lm -lmosquitto

TARGET := card-manager

all: info $(TARGET)

$(TARGET): $(OBJECTS) 
	@mkdir -p $(BINDIR)
	@echo " Linking..."
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -o $(BINDIR)/$(TARGET) $(LIB)


# generic: build any object file required
%.o: %.c
	$(CC) $(CFLAGS) $(INC) $(LIB) -c -o $@ $<
	
clean: 
	rm -rf $(BINDIR) $(OBJECTS)

info:
	@echo "INFO!"
	@echo "SOURCES=$(SOURCES)"
	@echo "OBJECTS=$(OBJECTS)"
	@echo "CC=$(CC)"
	@echo "CFLAGS=$(CFLAGS)"
	@echo "LDFLAGS=$(LDFLAGS)"
	@echo "LIB=$(LIB)"
	
