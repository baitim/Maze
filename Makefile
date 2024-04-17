EXE = server.bin

FILE_PATHS = $(wildcard server/*.c)
FILES = $(FILE_PATHS:server/%.c=%.c)
OBJECTS_DIR = build
OBJECTS_NAMES = $(FILES:.c=.o)
OBJECTS = $(addprefix $(OBJECTS_DIR)/, $(OBJECTS_NAMES))

CC = gcc

.PHONY : clean

all : $(EXE)

$(EXE) : $(OBJECTS)
	@$(CC) -o $(EXE) $(OBJECTS) -l uv

$(OBJECTS) : $(OBJECTS_DIR)/%.o: server/%.c | $(OBJECTS_DIR)
	@$(CC) -c $< -o $@

$(OBJECTS_DIR) :
	mkdir -p $(OBJECTS_DIR) %2>/dev/null

clean :
	rm $(EXE) $(OBJECTS)