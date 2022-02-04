/*
 * utils.h - header file for utils.c
 *
 */
 

#if !defined(INCLUDE_UTILS_H)
#define UTILS_H

/*
 * standard truth :-)
 */
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
/* have a C99 compiler - we should expect to have <stdbool.h> */
#include <stdbool.h>
#elif !defined(__cplusplus)
/* do not have a C99 compiler - fake a <stdbool.h> header file */
typedef unsigned char bool;
#undef true
#define true ((bool)(1))
#undef false
#define false ((bool)(0))
#endif

/* externs */
extern FILE *input_stream;

/* function prototypes */
bool exists(char const *path);
bool is_file(char const *path);
bool is_exec(char const *path);
bool is_dir(char const *path);
bool is_read(char const *path);
bool is_write(char const *path);
ssize_t file_size(char const *path);
ssize_t readline(char **linep, FILE * stream);
char *readline_dup(char **linep, bool strip, size_t *lenp, FILE * stream);
void para(char const *line, ...);
void fpara(FILE * stream, char const *line, ...);
char *prompt(char const *str, size_t *lenp);
ssize_t file_size(char const *path);

#endif /* !defined(UTILS_H) */
