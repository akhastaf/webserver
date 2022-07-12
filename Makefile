SRCS		=  	main.cpp srcs/Parser.cpp srcs/Tokenizer.cpp srcs/RequestTokenizer.cpp srcs/RequestParser.cpp \
				srcs/Request.cpp srcs/Response.cpp srcs/VirtualServers.cpp srcs/Socket.cpp srcs/Server.cpp \
				srcs/utils.cpp srcs/MimeTypes.cpp

CPP_VERSION	=	-std=c++98

COMPILER	=	clang++

FLAGS		=	-Wextra -Wall -Werror

NAME		=	webserve

all : $(NAME)

$(NAME) : $(SRC) 
	@$(COMPILER) $(CPP_VERSION) $(FLAGS) $(SRCS) -o $(NAME)
debug:
	@$(COMPILER) $(CPP_VERSION) $(FLAGS) -g -fsanitize=address $(SRCS) -o $(NAME)

clean :
	@rm -rf *.dSYM .vscode a.out

fclean : clean
	@rm -rf $(NAME) 
re : fclean all

.PHONY: all clean fclean