CFLAGS   = -O3

LFLAGS   = -l uv -lpthread `sdl2-config --cflags --libs` -lSDL2 -lSDL2_mixer -lSDL2_image -lSDL2_ttf

CPP      = gcc
LINKER   = gcc
rm       = rm -rf

TXTDIR   = txt
IMAGEDIR = images
SRCDIR   = src
OBJDIR   = obj

CMD_FLAGS = --map_txt_file $(TXTDIR)/map.txt\
			--screenshot_file $(IMAGEDIR)/Maze.png\
			--font_file arial.ttf\
			--help

TARGET   = client

SOURCES  := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

$(TARGET): $(OBJDIR) $(OBJECTS)
	@$(LINKER) $(OBJECTS) $(LFLAGS) -o $@
	@echo "Linking complete!"

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