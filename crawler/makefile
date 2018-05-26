CC = gcc
FLAGS = -Wall
DEPS = get.h pool.h server.h utilities.h
NAME = mycrawler
SOURCE = get.c pool.c servermain.c server.c utilities.c
OBJ = $(SOURCE:.c=.o)

%.o : %.c
	$(CC) -c $<

$(NAME): $(OBJ)
	$(CC) $(FLAGS) -o $(NAME) $(OBJ) -lpthread

$(OBJ): $(DEPS)

.PHONY: clean

clean:
	rm *.o $(NAME)
