#ifndef _USER_H_
#define _USER_H_
#include "pstat.h"

struct stat;

// system calls
int fork(void);
int exit(void) __attribute__((noreturn));
int wait(void);
int pipe(int*);
int write(int, const void*, int);
int read(int, void*, int);
int close(int);
int kill(int);
int exec(char*, char**);
int open(const char*, int);
int mknod(char*, short, short);
int unlink(const char*);
int fstat(int fd, struct stat*);
int link(char*, char*);
int mkdir(char*);
int chdir(char*);
int dup(int);
int getpid(void);
char* sbrk(int);
int sleep(int);
int uptime(void);
int getpinfo(struct pstat*);
int settickets(int);

// user library functions (ulib.c)
int stat(char*, struct stat*);
char* strcpy(char*, char*);
void *memmove(void*, void*, int);
char* strchr(const char*, char c);
int strcmp(const char*, const char*);
void fprintf(int, const char*, ...);
void putchar(char c);
char getchar(void);
char* gets(char*, int max);
uint strlen(const char*);
void* memset(void*, int, uint);
void* malloc(uint);
void* calloc(uint, uint);
void free(void*);
int atoi(const char*);

// Constants
extern const int stdin;
extern const int stdout;
extern const int stderr;

#endif // _USER_H_

