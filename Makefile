SRCS		=   main.cpp Config.cpp

CPP_VERSION	=	-std=c++98

COMPILER	=	clang++

FLAGS		=	-Wextra -Wall -Werror

NAME		=	webserve

all : $(NAME)

$(NAME) : $(SRC) 
	@$(COMPILER) $(CPP_VERSION) $(FLAGS) $(SRCS) -o $(NAME)

clean :
	@rm -rf *.dSYM .vscode a.out

fclean : clean
	@rm -rf $(NAME) 
re : fclean all

.PHONY: all clean fclean