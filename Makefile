#
# Author: Eduardo Miravalls Sierra
# Date: 31-08-2014

CC := gcc
LINKER := gcc

INCL := include/
CFLAGS := -Wall -Wextra -O2 $(addprefix -I,$(INCL))

OBJDIR := obj
SRCDIR := src
LIBDIR := lib
BINDIR := bin

SRC := src/DStrings.c
OBJ := $(patsubst $(SRCDIR)/%, $(OBJDIR)/%, $(SRC:.c=.o))

.PHONY: all dirs makelib clean tests


all:	dirs makelib
	@echo 'DONE!'

dirs:
	@mkdir -p $(OBJDIR) $(LIBDIR) $(BINDIR) -v


makelib:	$(OBJ)
	@echo "#------------------------------"
	ar rcs $(LIBDIR)/libDStrings.a $(shell lorder $^ | tsort)


$(OBJDIR)/%.o:	$(SRCDIR)/%.c
	@echo "#------------------------------"
	$(CC) $(CFLAGS) -MMD -MP -c $^ -o $@


tests:	CFLAGS += -g
tests:	$(OBJ) $(OBJDIR)/DStrings_test.o
	@echo "#------------------------------"
	$(LINKER) $(CFLAGS) $^ -o $(BINDIR)/$@
	@echo "#------------------------------"
	@echo "Executing tests with valgrind"
	@valgrind --leak-check=full --track-origins=yes $(BINDIR)/$@


clean:
	@find $(OBJDIR) -type f \( -name '*.o' -o -name '*.d' \) -delete
	@find $(LIBDIR) -type f -delete
	@find $(BINDIR) -type f -delete
	@find . -type f -name '*~'
