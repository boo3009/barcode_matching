.PHONY: all run clean
ELF=main
MAIN_C=main.c
#DEST=files/#
DEPS=deps.mk
CC=gcc
CFLAGS=-Wall -g -fsanitize=address -fno-omit-frame-pointer -O0
SOURCES=$(filter-out $(MAIN_C),$(wildcard *.c))
OBJECTS=$(SOURCES:.c=.o)

all: $(ELF)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

$(ELF): $(MAIN_C) $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

ifneq (clean,$(MAKECMDGOALS))
-include $(DEPS)
endif

$(DEPS): $(SOURCES)
	$(CC) -MM $^ > $@

run: $(ELF)
#	mv $(ELF) $(DEST) &&\#
#	cd $(DEST) &&\#
	./$(ELF)

debug: $(ELF)
#	cd $(DEST) &&\#
	gdb ./$(ELF)
	trash-put -f $(ELF)

clean:
	trash-put -f *.o
#	trash-put -f $(DEST)$(ELF)#
#	find $(DEST) -type f -name \*_FIN* -exec trash-put -f {} +#
	find . -type f -name \*_FIN* -exec trash-put -f {} +
