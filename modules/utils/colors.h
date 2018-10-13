/*
 *  colors.h - Terminal colors definitions
 *
 *  Copyright (C) 2018 Atanas Tulbenski <top4ester@gmail.com>
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

#ifndef __COLORS_H_
#define __COLORS_H_

/* unsets color to term's fg color */
#define COLOR_NONE	"\033[0m"

/* regular colors */
#define COLOR_BLACK	"\033[0;30m"
#define COLOR_RED	"\033[0;31m"
#define COLOR_GREEN	"\033[0;32m"
#define COLOR_YELLOW	"\033[0;33m"
#define COLOR_BLUE	"\033[0;34m"
#define COLOR_MAGENTA	"\033[0;35m"
#define COLOR_CYAN	"\033[0;36m"
#define COLOR_WHITE	"\033[0;37m"

/* emphasized (bolded) colors */
#define COLOR_EMK	"\033[1;30m"
#define COLOR_EMR	"\033[1;31m"
#define COLOR_EMG	"\033[1;32m"
#define COLOR_EMY	"\033[1;33m"
#define COLOR_EMB	"\033[1;34m"
#define COLOR_EMM	"\033[1;35m"
#define COLOR_EMC	"\033[1;36m"
#define COLOR_EMW	"\033[1;37m"

/* background colors */
#define COLOR_BGK	"\033[40m"
#define COLOR_BGR	"\033[41m"
#define COLOR_BGG	"\033[42m"
#define COLOR_BGY	"\033[43m"
#define COLOR_BGB	"\033[44m"
#define COLOR_BGM	"\033[45m"
#define COLOR_BGC	"\033[46m"
#define COLOR_BGW	"\033[47m"

#endif /* __COLORS_H_ */
