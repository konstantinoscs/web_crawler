CC = gcc
FLAGS = -Wall
DEPS = get.h pool.h server.h utilities.h
NAME = myhttpd
SOURCE = get.c pool.c servermain.c server.c utilities.c
OBJ = $(SOURCE:.c=.o)

%.o : %.c
	$(CC) -c $< -g

$(NAME): $(OBJ)
	$(CC) $(FLAGS) -o $(NAME) $(OBJ) -g -lpthread

$(OBJ): $(DEPS)

.PHONY: clean

clean:
	rm *.o $(NAME)
