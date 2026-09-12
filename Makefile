# Makefile - VM_Model_001
CC      = cc
CFLAGS  = -Wall -Wextra -g -std=c11 -MMD -MP
SRCS    = vm.c hal.c alu.c opcodes.c main.c
OBJS    = $(SRCS:.c=.o)
DEPS    = $(OBJS:.o=.d)
TARGET  = emulator

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(DEPS) $(TARGET)

# pulls in the generated .d files; the - means "fine if absent"
-include $(DEPS)

.PHONY: all run clean

