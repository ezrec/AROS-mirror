/*
    Copyright © 2012, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <aros/system.h>

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <dirent.h>

#include <sys/stat.h>

off_t lseek(int fd, off_t offset, int whence) { return -1; }

int fclose(FILE *f) { return -1; }

int puts(const char *s) { return -1; }

struct dirent *readdir(DIR *dirp) { return NULL; }

int rename(const char *oldpath, const char *newpath) { return -1; }

int rmdir(const char *pathname) { return -1; }

char *setlocale(int category, const char *locale) { return NULL; }

int stat(const char *path, struct stat *buf) { return -1; }

time_t time(time_t *t) { return 0; }

long int random(void) { return (long int)0xdeadbeef; }

ssize_t read(int fd, void *buf, size_t count) { return -1; }

ssize_t write(int fd, const void *buf, size_t count) { return -1; }

int unlink(const char *pathname) { return -1; }

int access(const char *pathname, int mode) { return -1; }

int closedir(DIR *dir) { return -1; }

int dup(int fd) { return -1; }

int ferror(FILE *stream)
{
	return -1;
}

int fflush(FILE *stream)
{
	return -1;
}

FILE *fopen(const char *path, const char *mode) { return NULL; }

int fputc(int c, FILE *stream) { return -1; }

int fputs(const char *s, FILE *stream) { return -1; }

int fstat(int fd, struct stat *buf) { return -1; }

char *getcwd(char *buf, size_t size) { return NULL; }

char *getenv(const char *name) { return NULL; }

struct lconv *localeconv(void) { return NULL; }

int mkdir(const char *pathname, mode_t mode) { return -1; }

DIR *opendir(const char *name) { return NULL; }

void perror(const char *s) { }

int printf(const char *format, ...) { return -1; }

int vfprintf(FILE *stream, const char *format, va_list ap) { return -1; }

int putchar(int c) { return -1; }

int fprintf(FILE *stream, const char *format, ...)
{
	return -1;
}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	return -1;
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	return -1;
}
