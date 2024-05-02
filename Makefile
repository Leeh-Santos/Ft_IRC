SRCS		= main.cpp \
			Server.cpp \
			Client.cpp \
			ServerUtils.cpp \
			
OBJS		= $(SRCS:.cpp=.o)

CXX			= c++

CXXFLAGS	= -Wall -Wextra -Werror -std=c++98 

NAME		= ft_irc

all: $(NAME)

$(NAME): $(OBJS) 
		$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME) -fsanitize=address -g

RM		= rm -rf

clean: 
	$(RM) $(OBJS)

fclean:
	$(RM) $(OBJS) $(NAME)

re:fclean all