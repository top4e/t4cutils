/*
 *  debug.h - Pretty debug printf
 *
 *  Copyright (C) 2018 Atanas Tulbenski <top4ester@gmail.com>
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

#ifndef __DEBUG_H_
#define __DEBUG_H_

typedef enum verbose_level_e {
	DBG_QUIET,
	DBG_NORMAL,
	DBG_VERBOSE,
} verbose_level_t;

typedef struct dbg_domain_ctx_s {
	char		*name;
	verbose_level_t	verbose;
} dbg_domain_ctx_t;


enum {
	INFO,
	WARNING,
	ERROR,
};

#define DEBUG_CREATE_CTX(__x, __l)		\
	static dbg_domain_ctx_t dbg_ctx = { #__x, __l};

//extern dbg_domain_ctx_t dbg_ctx;

void debug_print(dbg_domain_ctx_t	*ctx,
		 int			domain,
		 const char		*file,
		 const char		*func,
		 int			line,
		 const char		*fmt,
		 ...);

#define DEBUG_PRINT	debug_print

#define IPRN(__fmt, ...)	\
	DEBUG_PRINT(&dbg_ctx,	\
		INFO,		\
		__FILE__,	\
		__FUNCTION__,	\
		__LINE__,	\
		__fmt,		\
		## __VA_ARGS__);

#define WPRN(__fmt, ...)	\
	DEBUG_PRINT(&dbg_ctx,	\
		WARNING,	\
		__FILE__,	\
		__FUNCTION__,	\
		__LINE__,	\
		__fmt,		\
		## __VA_ARGS__);

#define EPRN(__fmt, ...)	\
	DEBUG_PRINT(&dbg_ctx,	\
		ERROR,		\
		__FILE__,	\
		__FUNCTION__,	\
		__LINE__,	\
		__fmt,		\
		## __VA_ARGS__);

#endif /* __DEBUG_H_ */
