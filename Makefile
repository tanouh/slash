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
SRC_FILES = $(filter-out $(SRC_DIR)/$(PROG).c, $(wildcard $(SRC_DIR)/*.c))
TEST_FILES = $(wildcard $(TEST_DIR)/*.c)

#BINARIES 
OBJECTS = $(SRC_FILES:%.c= %.o)
TEST_OBJ = $(TEST_FILES:%.c= %.o)

#Run
all: $(PROG)

run : $(PROG)
	@./$(PROG)

test : $(TEST_PROG)
	@./$(TEST_PROG)

$(PROG) : $(OBJECTS)
	@$(CC) $(CFLAGS)  -o $@ $^ $(LDLIBS) 

$(TEST_PROG): $(TEST_OBJ)
	@$(CC) $(CFLAGS) -o $@ $^

$(SRC_DIR)/%.o: %.c
	@$(CC) $(CFLAGS) -c $^

clean:
	@rm -f $(PROG) $(TEST_PROG)
	@rm -f $(OBJECTS) $(TEST_OBJ)
	@rm -f *.o