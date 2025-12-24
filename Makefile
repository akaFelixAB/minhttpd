CC := gcc

# output directory
OBJDIR := build

# sources
SRCS := start.S httpd.c 
BASENAMES := $(basename $(notdir $(SRCS)))
OBJS := $(addprefix $(OBJDIR)/,$(addsuffix .o,$(BASENAMES)))

COMMON_FLAGS := -std=c11 -nostdlib -ffreestanding -static -Wall

CFLAGS_DEBUG := $(COMMON_FLAGS) -g -O0 -fno-omit-frame-pointer
LDFLAGS_DEBUG := $(COMMON_FLAGS)

CFLAGS_RELEASE := $(COMMON_FLAGS) -O2 -s -pedantic -fno-unwind-tables -fno-asynchronous-unwind-tables -fno-stack-protector
LDFLAGS_RELEASE := $(COMMON_FLAGS) -Wl,-n -Wl,--gc-sections -Wl,--strip-all -Wl,--build-id=none -Wl,-z,separate-code -Wl,-z,noexecstack

.PHONY: all debug release clean

all: release

# Debug build (produces build/httpd-debug)
debug: CFLAGS := $(CFLAGS_DEBUG)
debug: LDFLAGS := $(LDFLAGS_DEBUG)
debug: $(OBJDIR)/httpd-debug

$(OBJDIR)/httpd-debug: $(OBJS) | $(OBJDIR)
	$(CC) $(LDFLAGS) -o $@ $^

# Release build (produces build/httpd)
release: CFLAGS := $(CFLAGS_RELEASE)
release: LDFLAGS := $(LDFLAGS_RELEASE)
release: $(OBJDIR)/httpd

$(OBJDIR)/httpd: $(OBJS) | $(OBJDIR)
	$(CC) $(LDFLAGS) -o $@ $^

# Compile rules (use CFLAGS from the active target)
$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/%.o: %.S | $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR):
	@mkdir -p $(OBJDIR)

clean:
	-rm -rf $(OBJDIR)