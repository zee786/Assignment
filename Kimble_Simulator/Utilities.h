#pragma once

/* --- Various Macros defined. Setting the color of Traces etc. --- */
#define     ANSI_COLOR_YELLOW   "\x1b[33m"
#define     ANSI_COLOR_RED      "\x1b[31m"
#define     ANSI_COLOR_GREEN    "\x1b[32m"
#define     ANSI_COLOR_RESET    "\x1b[0m"
#define     ANSI_COLOR_CYAN     "\x1b[36m"
#define     ANSI_COLOR_GRAY     "\x1b[1;30m"



#define LOG(...)			printf(__VA_ARGS__);








class Utilities
{
public:
	Utilities(void);
	~Utilities(void);
};

