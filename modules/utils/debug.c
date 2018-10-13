/*
 *  debug.c - Pretty debug printf
 *
 *  Copyright (C) 2018 Atanas Tulbenski <top4ester@gmail.com>
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "debug.h"
#include "colors.h"

/*

*/
static void debug_printf(int domain,
			 const char *name,
			 const char *fmt,
			 va_list args)
{
	switch (domain) {
	case ERROR:
		printf("%s[%sERROR  %s]%s %15s:%s",
			COLOR_CYAN,
			COLOR_RED,
			COLOR_CYAN,
			COLOR_EMC,
			name,
			COLOR_EMG);
		break;
	case WARNING:
		printf("%s[%sWARNING%s]%s %15s:%s",
			COLOR_CYAN,
			COLOR_YELLOW,
			COLOR_CYAN,
			COLOR_EMC,
			name,
			COLOR_EMG);
		break;
	case INFO:
		printf("%s[%sINFO   %s]%s %15s:%s",
			COLOR_CYAN,
			COLOR_GREEN,
			COLOR_CYAN,
			COLOR_EMC,
			name,
			COLOR_EMG);
		break;
	default:
		break;
	}
	vprintf(fmt, args);
	printf(COLOR_NONE);
}

/*

*/
static void debug_printf_simple(int domain,
				const char *name,
				const char *fmt,
				va_list args)
{
	switch (domain) {
	case ERROR:
		printf("%s[%sERROR  %s]%s %15s:%s ",
			COLOR_CYAN,
			COLOR_RED,
			COLOR_CYAN,
			COLOR_EMC,
			name,
			COLOR_EMY);
		break;
	case WARNING:
		printf("%s[%sWARNING%s]%s %15s:%s ",
			COLOR_CYAN,
			COLOR_YELLOW,
			COLOR_CYAN,
			COLOR_EMC,
			name,
			COLOR_EMY);
		break;
	case INFO:
		printf("%s[%sINFO   %s]%s %15s:%s ",
			COLOR_CYAN,
			COLOR_GREEN,
			COLOR_CYAN,
			COLOR_EMC,
			name,
			COLOR_EMY);
		break;
	default:
		break;
	}
	vprintf(fmt, args);
	printf(COLOR_NONE);
}

/*

*/
static void debug_printf_verbose(int domain,
				 const char *name,
				 const char *file,
				 const char *func,
				 int line,
				 const char *fmt,
				 va_list args)
{
	switch (domain) {
	case ERROR:
		printf("%s[%sERROR  %s]%s %s:%s %s: %s%s():%d - %s",
			COLOR_CYAN,
			COLOR_RED,
			COLOR_CYAN,
			COLOR_EMC,
			name,
			COLOR_EMM,
			file,
			COLOR_EMG,
			func,
			line,
			COLOR_EMB);
		break;
	case WARNING:
		printf("%s[%sWARNING%s]%s %s:%s %s: %s%s():%d - %s",
			COLOR_CYAN,
			COLOR_YELLOW,
			COLOR_CYAN,
			COLOR_EMC,
			name,
			COLOR_EMM,
			file,
			COLOR_EMG,
			func,
			line,
			COLOR_EMB);
		break;
	case INFO:
		printf("%s[%sINFO   %s]%s %s:%s %s: %s%s():%d - %s",
			COLOR_CYAN,
			COLOR_GREEN,
			COLOR_CYAN,
			COLOR_EMC,
			name,
			COLOR_EMM,
			file,
			COLOR_EMG,
			func,
			line,
			COLOR_EMB);
		break;
	default:
		break;
	}
	vprintf(fmt, args);
	printf(COLOR_NONE);
}

__attribute__ ((visibility ("default")))
void debug_print(dbg_domain_ctx_t	*ctx,
		 int			domain,
		 const char		*file,
		 const char		*func,
		 int			line,
		 const char		*fmt,
		 ...)
{
	va_list args;

	if (!ctx)
		return;
	va_start(args, fmt);

	switch (ctx->verbose) {
	case DBG_QUIET:
		debug_printf_simple(domain, ctx->name, fmt, args);
		break;
	case DBG_NORMAL:
		debug_printf(domain, ctx->name, fmt, args);
		break;
	case DBG_VERBOSE:
		debug_printf_verbose(domain, ctx->name,
				     file, func, line, fmt, args);
		break;
	}
	va_end(args);
}
