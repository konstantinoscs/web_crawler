CC = gcc
FLAGS = -Wall
DEPS = server.h utilities.h
NAME = myhttpd
SOURCE = servermain.c server.c utilities.c
OBJ = $(SOURCE:.c=.o)

%.o : %.c
	$(CC) -c $<

$(NAME): $(OBJ)
	$(CC) $(FLAGS) -o $(NAME) $(OBJ)

$(OBJ): $(DEPS)

.PHONY: clean

clean:
	rm *.o $(NAME)
