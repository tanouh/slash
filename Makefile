#Variables
CC = gcc
CFLAGS = -Wall -g -pedantic -MMD
LDLIBS = -lreadline

#Directory 
SRC_DIR = src
TEST_DIR = test
BIN=bin

#EXECUTABLE
PROG = slash
# TEST_PROG = test/ (à compléter)

#.c FILES
PROG_FILE=$(SRC_DIR)/$(PROG).c
SRC_FILES =$(filter-out $(SRC_DIR)/$(PROG).c, $(wildcard $(SRC_DIR)/*.c))
TEST_FILES =$(wildcard $(TEST_DIR)/*.c)

#BINARIES 
PROG_OBJECT=$(BIN)/$(PROG).o
OBJECTS =$(SRC_FILES:$(SRC_DIR)/%.c=$(BIN)/%.o)
TEST_OBJ =$(TEST_FILES:$(SRC_DIR)/%.c=$(BIN)/%.o)
DEP=$(OBJECTS:%.o=%.d) $(TEST_OBJ:%.o=.d) $(PROG_OBJECT:%.o=%.d)


#Run
all: $(PROG)

run : $(PROG)
	@./$(PROG)

test : $(TEST_PROG)
	@./$(TEST_PROG)

-include $(DEP)

$(PROG) : $(PROG_OBJECT) $(OBJECTS)
	@$(CC) $(CFLAGS) $^ -o $(PROG) $(LDLIBS)

$(TEST_PROG): $(TEST_OBJ) $(OBJECTS)
	@$(CC) $(CFLAGS) -o $(TEST_PROG) $^

$(BIN)/%.o:$(SRC_DIR)/%.c
	@mkdir -p $(BIN)
	@$(CC) $(CFLAGS) -o $@ -c $< 

clean:
	@rm -f $(PROG) $(TEST_PROG)
	@rm -rf $(BIN)
