/**
 * @file
 *
 *
 */

#ifndef XTCOMPILER_H
#define XTCOMPILER_H

#ifdef UNUSED
#elif defined(__GNUC__)
// # define UNUSED(x) UNUSED_ ## x __attribute__ ((unused))
# define UNUSED(x) (void)(x)
#elif defined(__LCLINT__)
# define UNUSED(x) /*@unused@*/ x
#elif defined(__cplusplus)
# define UNUSED(x)
#else
# define UNUSED(x) x
#endif



#endif // XTCOMPILER_H