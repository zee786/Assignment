#pragma once
#ifndef INCLUDES_UTILITIES_H_
#define INCLUDES_UTILITIES_H_

/* --- Various Macros defined. Setting the colour of Traces etc. --- */
#if defined(__UNIX__) || defined(linux)

#define     ANSI_COLOR_YELLOW   "\x1b[33m"
#define     ANSI_COLOR_RED      "\x1b[31m"
#define     ANSI_COLOR_GREEN    "\x1b[32m"
#define     ANSI_COLOR_RESET    "\x1b[0m"
#define     ANSI_COLOR_CYAN     "\x1b[36m"
#define     ANSI_COLOR_GRAY     "\x1b[1;30m"

#elif defined(_WIN32)
#define     ANSI_COLOR_YELLOW
#define     ANSI_COLOR_RED
#define     ANSI_COLOR_GREEN
#define     ANSI_COLOR_RESET
#define     ANSI_COLOR_CYAN
#define     ANSI_COLOR_GRAY

#endif

#define     LOG(...)                 printf(__VA_ARGS__);


#endif /* INCLUDES_UTILITIES_H_ */

#define LOG(...)			printf(__VA_ARGS__);








class Utilities
{
public:
	Utilities(void);
	~Utilities(void);
};

