#ifndef ENABLE_DEBUG_MESSAGE_H
# define ENABLE_DEBUG_MESSAGE_H

# define DEBUG_SAFE_EXIT /* activate or deactivate for debug */

# ifdef DEBUG_SAFE_EXIT
#  define DEBUG_SAFE_FREE_ALL
#  define DEBUG_SAFE_CLOSE_ALL
# endif

#endif
