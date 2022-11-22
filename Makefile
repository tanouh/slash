#Variables
CC = gcc
CFLAGS = -Wall -g -pedantic
LDLIBS = -lreadline

#Directory 
SRC_DIR = src
TEST_DIR = test

#EXECUTABLE
PROG = slash
# TEST_PROG = test/ (à compléter)

#.c FILES
PROG_FILE=$(SRC_DIR)/$(PROG).c
SRC_FILES =$(filter-out $(SRC_DIR)/$(PROG).c, $(wildcard $(SRC_DIR)/*.c))
TEST_FILES =$(wildcard $(TEST_DIR)/*.c)

#Run
all: $(PROG)

run : $(PROG)
	@./$(PROG)

test : $(TEST_PROG)
	@./$(TEST_PROG)

$(PROG) : $(PROG_FILE) $(SRC_FILES)
	@$(CC) $(CFLAGS) $< -o $@ $(LDLIBS)

$(TEST_PROG): $(TEST_FILES) $(SRC_FILES)
	@$(CC) $(CFLAGS) $< -o $@

clean:
	@rm -f $(PROG) $(TEST_PROG)