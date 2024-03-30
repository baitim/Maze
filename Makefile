CFLAGS   = 
LFLAGS   = -lsfml-graphics -lsfml-window -lsfml-system

CPP      = g++
LINKER   = g++
rm       = rm -rf

SRCDIR   = src
OBJDIR   = obj

TARGET   = maze

SOURCES  := $(wildcard $(SRCDIR)/*.cpp)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

$(TARGET): $(OBJECTS)
	@$(LINKER) $(OBJECTS) $(LFLAGS) -o $@
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@$(CPP) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

.PHONY: clean
clean:
	@$(rm) $(OBJECTS)
	@$(rm) $(TARGET)
	@echo "Cleanup complete!"
