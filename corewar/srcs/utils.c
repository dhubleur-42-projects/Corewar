#include "utils.h"

uint32_t big_to_little_endian(uint32_t big_endian) {
	return ((big_endian >> 24) & 0xFF) |
		   ((big_endian >> 8) & 0xFF00) |
		   ((big_endian << 8) & 0xFF0000) |
		   ((big_endian << 24) & 0xFF000000);
}