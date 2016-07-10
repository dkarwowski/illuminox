CC := gcc
SRCDIR := src
BUILDDIR := build
TARGETDIR := bin
TARGET := proto
GAME := libgame

SOURCES := $(shell find $(SRCDIR) -type f -name *.c)
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.c=.o))
OPTIM  :=
CFLAGS := -fPIC $(shell sdl2-config --cflags) -D_THREAD_SAFE $(OPTIM)
WFLAGS := -Wall -Wno-missing-braces -Wno-unused-function -DDEBUG -g
LIBS = -ldl -lm $(shell sdl2-config --libs) -lSDL2_ttf -lSDL2_image

CONFIGDIR := config
CONFIGEXT := json
JSON := $(shell find $(CONFIGDIR) -type f -name *.$(CONFIGEXT))

default: $(GAME)
	@echo -e "\e[1;92m-> Done \e[0m"

all: config $(TARGET) $(GAME)
	@echo -e "\e[1;92m-> Done\e[0m"

$(TARGET): $(BUILDDIR)/main.o
	@echo -e "\e[1;94m-> Creating main... \e[0m"
	$(CC) $^ $(OPTIM) -o $(TARGETDIR)/$(TARGET) $(LIBS)

$(GAME): $(OBJECTS)
	@echo -e "\e[1;94m-> Creating libgame.so... \e[0m"
	$(CC) $^ $(OPTIM) -shared -o $(TARGETDIR)/$@.so -Wl,-soname,$@.so $(LIBS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@echo -e "\e[1;96m-> Creating $@...\e[0m"
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(WFLAGS) -c -o $@ $<
	$(CC) $(CFLAGS) $(WFLAGS) -MM $< > $(BUILDDIR)/$*.d
	@mv -f $(BUILDDIR)/$*.d $(BUILDDIR)/$*.d.tmp
	@sed -e 's|.*:|$(BUILDDIR)/$*.o:|' < $(BUILDDIR)/$*.d.tmp > $(BUILDDIR)/$*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $(BUILDDIR)/$*.d.tmp | fmt -1 | \
		sed -e 's/^ *//' -e 's/$$/:/' >> $(BUILDDIR)/$*.d
	@rm -f $(BUILDDIR)/$*.d.tmp

config:
	$(CONFIGDIR)/json2h.py $(JSON)

clean:
	@echo -e "\e[1;91m-> Cleaning... \e[0m"
	rm -r $(BUILDDIR)/* $(TARGETDIR)/*

-include $(OBJECTS:.o=.d)

.PHONY: clean config
