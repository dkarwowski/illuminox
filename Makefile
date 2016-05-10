CC := clang
SRCDIR := src
BUILDDIR := build
TARGETDIR := bin
TARGET := proto
GAME := libgame

SRCEXT := c
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS := -fPIC -I/usr/local/include/SDL2 -D_THREAD_SAFE
WFLAGS := -Wall -Wno-missing-braces -Wno-unused-function -O0 -DDEBUG -g
LIBS = -ldl -lm -lSDL2 -lSDL2_ttf -L/usr/local/lib

default: $(TARGET) $(GAME)
	@echo "-> Done"

$(TARGET): $(BUILDDIR)/main.o
	@echo "-> Creating main... "
	$(CC) $^ -o $(TARGETDIR)/$(TARGET) $(LIBS)

$(GAME): $(OBJECTS)
	@echo "-> Creating libgame.so... "
	$(CC) $(CFLAGS) $(WFLAGS) $^ -shared -o $(TARGETDIR)/$@.so -Wl,-soname,$@.so $(LIBS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@echo "-> Creating all *.o..."
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(WFLAGS) -c -o $@ $<

clean:
	@echo " Cleaning... "
	rm -r $(BUILDDIR)/* $(TARGETDIR)/*

.PHONY: clean
