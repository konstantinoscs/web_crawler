CC = gcc
FLAGS = -Wall
DEPS = pool.h utilities.h
NAME = mycrawler
SOURCE = pool.c crawlermain.c utilities.c
OBJ = $(SOURCE:.c=.o)

%.o : %.c
	$(CC) -c $<

$(NAME): $(OBJ)
	$(CC) $(FLAGS) -o $(NAME) $(OBJ) -lpthread

$(OBJ): $(DEPS)

.PHONY: clean

clean:
	rm *.o $(NAME)
