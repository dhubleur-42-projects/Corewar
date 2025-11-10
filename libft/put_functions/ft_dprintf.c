#include "libft.h"

void ft_dprintf(int fd, const char *format, ...)
{
	va_list args;

	va_start(args, format);
	for (size_t i = 0; i < ft_strlen(format); i++) {
		if (format[i] != '%') {
			write(fd, &format[i], 1);
			continue;
		}

		i++;
		if (i >= ft_strlen(format)) {
			break;
		}
		switch (format[i]) {
			case 'c': {
				char c = (char)va_arg(args, int);
				write(fd, &c, 1);
				break;
			}
			case 'i':
			case 'd': {
				int num = va_arg(args, int);
				ft_putnbr_fd(num, fd);
				break;
			}
			case 's': {
				char *str = va_arg(args, char *);
				if (str != NULL) {
					ft_putstr_fd(str, fd);
				} else {
					ft_putstr_fd("(null)", fd);
				}
				break;
			}
			case 'x': {
				unsigned int num = va_arg(args, unsigned int);
				ft_putnbr_hexa_fd(num, 0, fd);
				break;
			}
			case 'X': {
				unsigned int num = va_arg(args, unsigned int);
				ft_putnbr_hexa_fd(num, 1, fd);
				break;
			}
			case '%': {
				write(fd, "%", 1);
				break;
			}
			default: {
				write(fd, &format[i], 1);
				break;
			}
		}
	}
	va_end(args);
}