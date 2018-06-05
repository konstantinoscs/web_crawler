CC = gcc
FLAGS = -Wall
DEPS = crawler.h pool.h utilities.h wget.h parent.h plist.h search.h trie.h searchutilities.h worker.h
NAME = mycrawler
SOURCE = crawler.c crawlermain.c pool.c utilities.c wget.c \
searchmain.c parent.c plist.c search.c trie.c searchutilities.c worker.c
OBJ = $(SOURCE:.c=.o)

%.o : %.c
	$(CC) -c $< -g

$(NAME): $(OBJ)
	$(CC) $(FLAGS) -o $(NAME) $(OBJ) -g -lpthread -lm

$(OBJ): $(DEPS)

.PHONY: clean

clean:
	rm *.o $(NAME)
