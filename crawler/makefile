CC = gcc
FLAGS = -Wall
DEPS = crawler.h pool.h utilities.h
NAME = mycrawler
SOURCE = crawler.c crawlermain.c pool.c utilities.c
OBJ = $(SOURCE:.c=.o)

%.o : %.c
	$(CC) -c $<

$(NAME): $(OBJ)
	$(CC) $(FLAGS) -o $(NAME) $(OBJ) -lpthread

$(OBJ): $(DEPS)

.PHONY: clean

clean:
	rm *.o $(NAME)
