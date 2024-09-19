 #ifndef CUSTOM_PRINTF_H
 #define CUSTOM_PRINTF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stdint.h>
#include "xparameters.h"
#include "bspconfig.h"

struct params_s;


typedef int32_t (*func_ptr)(int c);

/*                                                   */

void print( const char *ptr);
extern void outbyte (char c);
extern char inbyte(void);

#ifdef __cplusplus
}
#endif

#endif	/* end of protection macro */
