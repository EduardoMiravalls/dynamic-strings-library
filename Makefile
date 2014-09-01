#
# Author: Eduardo Miravalls Sierra
# Date: 31-08-2014

# Dynamic C Strings library.
# Copyright (C) 2014 Eduardo Miravalls Sierra
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

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
DEP_FILES :=$(shell find $(OBJDIR) -type f -name '*.d')

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
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@


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

-include $(DEP_FILES)
