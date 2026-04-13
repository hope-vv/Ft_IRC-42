CC      = c++
CFLAGS  = -Wall -Wextra -Werror -std=c++98
NAME    = server
SRCS    = main.cpp \
	sources/Server.cpp \
	sources/Client.cpp \
	sources/Commands.cpp \
	sources/Utils.cpp \
	sources/Channel.cpp \
	sources/bot.cpp 

OBJS    = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all
