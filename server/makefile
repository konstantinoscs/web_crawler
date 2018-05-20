CC = gcc
FLAGS = -Wall
DEPS = pool.h server.h utilities.h
NAME = myhttpd
SOURCE = servermain.c pool.c server.c utilities.c
OBJ = $(SOURCE:.c=.o)

%.o : %.c
	$(CC) -c $<

$(NAME): $(OBJ)
	$(CC) $(FLAGS) -o $(NAME) $(OBJ) -lpthread

$(OBJ): $(DEPS)

.PHONY: clean

clean:
	rm *.o $(NAME)
