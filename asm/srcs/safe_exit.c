#include <stdlib.h>

#include "safe_alloc.h"
#include "safe_open.h"

#include "safe_exit.h"

void safe_exit(int exit_status)
{
	safe_free_all();
	safe_close_all();
	exit(exit_status);
}
