CC = gcc
FLAGS = -Wall -lm -O0 -Werror
SRC = main.c
EXE = main

all: run clean

val: debug clean

run: build 
	@echo "Running"
	@./$(EXE)

build:
	@echo "Comp"
	@$(CC) -o $(EXE) $(SRC) $(FLAGS)


.PHONY : clean
clean :
	@rm -f $(EXE) *~

view: build 
	@echo "Running"
	@./$(EXE) > output.txt
	@echo "Building .txt"
	# @rm -f $(EXE) *~

debug: build
	@echo "Running Valgrind"
	@valgrind --tool=memcheck --leak-check=full --error-exitcode=1 ./$(EXE)