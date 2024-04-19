CFLAGS   = -O3 # -D _DEBUG -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++\
-Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align\
-Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy\
-Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness\
-Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual\
-Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion\
-Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn\
-Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default\
-Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast\
-Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing\
-Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation\
-fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer\
-Wlarger-than=1310720 -Wstack-usage=1310720 -pie -fPIE -Werror=vla\
-Itests -Isrc\

LFLAGS   = -lm -lpthread `sdl2-config --cflags --libs` -lSDL2 -lSDL2_mixer -lSDL2_image -lSDL2_ttf

CPP      = gcc
LINKER   = gcc
rm       = rm -rf

IMAGEDIR = images
TXTDIR   = txt
SRCDIR   = src
OBJDIR   = obj

CMD_FLAGS = --map_txt_file $(TXTDIR)/map.txt\
			--screenshot_file $(IMAGEDIR)/Maze.png\
			--font_file arial.ttf\
			--help

TARGET   = maze

SOURCES  := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

$(TARGET): $(IMAGEDIR) $(TXTDIR) $(OBJDIR) $(OBJECTS)
	@$(LINKER) $(OBJECTS) $(LFLAGS) -o $@
	@echo "Linking complete!"

$(IMAGEDIR):
	@mkdir $(IMAGEDIR)
	@echo "Created images directory!"

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
