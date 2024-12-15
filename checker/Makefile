PHONY: checker libchunk clean

CFLAGS = -Wall -Wextra -std=gnu11 -g

BUILDDIR ?= ../build
LIBCHUNK ?= $(BUILDDIR)/libchunk.a

CHECK_SRCS = \
	check.c

CHECK_OBJS = $(patsubst %.c,%.o,$(CHECK_SRCS))
CHECK_TARGET = $(BUILDDIR)/check

checker: $(CHECK_TARGET)
	@:

$(CHECK_TARGET): $(CHECK_OBJS) libchunk
	@mkdir -p $(BUILDDIR)
	gcc $(CFLAGS) $(CHECK_OBJS) $(LIBCHUNK) -o $@ -lm

libchunk:
	$(MAKE) -C .. libchunk

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

clean:
	rm -f $(CHECK_TARGET) $(CHECK_OBJS)
