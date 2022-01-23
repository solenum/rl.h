# compiler
CC     =gcc
# ---

# dirs
ODIR   =obj
BDIR   =build
_IDIRS =../
IDIRS  =$(patsubst %,-I%/,$(_IDIRS))
BIN    =game
# ---

# flags
CFLAGS =-O3 -lm -ldl -std=gnu99 -lGL -lSDL2 -I. $(IDIRS)
CFLAGS+=-g -Wall -Wno-unused -Wno-switch -Wno-int-in-bool-context -fdiagnostics-color=always
# ---

# deps
SRCDIR =*.c */*.c */*/*.c
HDIR   =*.h */*.h */*/*.h ../rl.h
DEPS   =$(filter-out lib/*,$(wildcard $(HDIR)))
LDEPS  =$(wildcard lib/*/.c lib/*/*/.c)

_OBJ   =$(patsubst %.c,%.o,$(wildcard $(SRCDIR)))
OBJ    =$(addprefix $(ODIR)/, $(notdir $(_OBJ)))
# ---

# windows
ifeq ($(OS),Windows_NT)
CC    =x86_64-w64-mingw32-gcc
LD    =x86_64-w64-mingw32-ld
FLAGS   =-Wl,-Bstatic -g -lm -static -static-libgcc -static-libstdc++ -lstdc++ \
-Llib/win -lopengl32 -lmingw32 -Wl,-Bdynamic -lSDL2main -lSDL2 -I. $(IDIRS)
#
CFLAGS  =$(FLAGS)
CFLAGS +=-std=c99 -DAL_LIBTYPE_STATIC
BNAME   =game.exe
endif
# ---

# macos
UNAME  = $(shell uname -s)
ifeq ($(UNAME),Darwin)
CFLAGS =-O3 -g -lm -framework OpenGl -framework Foundation \
-framework IOKit -lSDL2 \
-I. $(IDIRS) -Wno-unused-command-line-argument
CFLAGS+=-std=c99 -Wall -Wno-unused
endif
# ---

# rules
$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
# ---

# main
all: files game

game: $(OBJ)
	$(CC) -o $(BDIR)/$(BIN) $^ $(CFLAGS)
# ---

# make files etc
files:
	mkdir -p $(ODIR)
	mkdir -p $(BDIR)
# ---

# util
.PHONY: clean

clean:
	rm -rf $(ODIR)
# ---
