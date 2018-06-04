CC = gcc
FLAGS = -Wall
DEPS = crawler.h pool.h utilities.h wget.h
NAME = mycrawler
SOURCE = crawler.c crawlermain.c pool.c utilities.c wget.c
OBJ = $(SOURCE:.c=.o)

%.o : %.c
	$(CC) -c $< -g

$(NAME): $(OBJ)
	$(CC) $(FLAGS) -o $(NAME) $(OBJ) -g -lpthread

$(OBJ): $(DEPS)

.PHONY: clean

clean:
	rm *.o $(NAME)
