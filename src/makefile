CC = gcc
FLAGS = -Wall -lm -O0 -Werror
SRC = main.c
EXE = main

all: run clean

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
	@rm -f $(EXE) *~