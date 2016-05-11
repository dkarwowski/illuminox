CC := gcc
SRCDIR := src
BUILDDIR := build
TARGETDIR := bin
TARGET := proto
GAME := libgame

SRCEXT := c
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS := -fPIC $(shell sdl2-config --cflags) -D_THREAD_SAFE
WFLAGS := -Wall -Wno-missing-braces -Wno-unused-function -O0 -DDEBUG -g
LIBS = -ldl -lm $(shell sdl2-config --libs)

default: $(GAME)
	@echo -e "\e[1;92m-> Done \e[0m"

all: $(TARGET) $(GAME)
	@echo -e "\e[1;92m-> Done\e[0m"

$(TARGET): $(BUILDDIR)/main.o
	@echo -e "\e[1;94m-> Creating main... \e[0m"
	$(CC) $^ -o $(TARGETDIR)/$(TARGET) $(LIBS)

$(GAME): $(OBJECTS)
	@echo -e "\e[1;94m-> Creating libgame.so... \e[0m"
	$(CC) $(CFLAGS) $(WFLAGS) $^ -shared -o $(TARGETDIR)/$@.so -Wl,-soname,$@.so $(LIBS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@echo -e "\e[1;96m-> Creating $@...\e[0m"
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(WFLAGS) -c -o $@ $<

clean:
	@echo -e "\e[1;91m-> Cleaning... \e[0m"
	rm -r $(BUILDDIR)/* $(TARGETDIR)/*

.PHONY: clean
