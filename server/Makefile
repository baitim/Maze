CFLAGS   = -O3

LFLAGS   = -l uv -lm

CPP      = gcc
LINKER   = gcc
rm       = rm -rf

TXTDIR   = txt
SRCDIR   = src
OBJDIR   = obj

CMD_FLAGS = --map_txt_file $(TXTDIR)/map.txt

TARGET   = server

SOURCES  := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

$(TARGET): $(TXTDIR) $(OBJDIR) $(OBJECTS)
	@$(LINKER) $(OBJECTS) $(LFLAGS) -o $@
	@echo "Linking complete!"

$(TXTDIR):
	@mkdir $(TXTDIR)
	@echo "Created txt directory!"

$(OBJDIR):
	@mkdir $(OBJDIR)
	@echo "Created obj directory!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@$(CPP) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

.PHONY: clean
clean:
	@$(rm) $(OBJECTS)
	@$(rm) $(TARGET)
	@echo "Cleanup complete!"

.PHONY: run
run:
	@./$(TARGET) $(CMD_FLAGS)