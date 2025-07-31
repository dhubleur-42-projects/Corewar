ASM_NAME	=	asm
VM_NAME		=	corewar

all			:	$(ASM_NAME) $(VM_NAME)

$(ASM_NAME)	:
	make -C $(ASM_NAME)

$(VM_NAME)	:
	make -C $(VM_NAME)

clean		:
	make -C $(ASM_NAME) clean
	make -C $(VM_NAME) clean

fclean		:
	make -C $(ASM_NAME) fclean
	make -C $(VM_NAME) fclean

re		:	fclean all

.PHONY: $(ASM_NAME) $(VM_NAME) all clean fclean re
