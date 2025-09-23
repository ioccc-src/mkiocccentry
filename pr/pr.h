/*
 * libpr - stdio helper library
 *
 * "Small acts of kindness can fill the world with light."
 *
 *	-- J.R.R. Tolkien
 *
 * Copyright (c) 2008-2025 by Landon Curt Noll and Cody Boone Ferguson.  All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright, this permission notice and text
 * this comment, and the disclaimer below appear in all of the following:
 *
 *       supporting documentation
 *       source copies
 *       source works derived from this source
 *       binaries derived from this source or from derived source
 *
 * THE AUTHORS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHORS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY
 * DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE OR JSON.
 *
 * The code in this from repo was copied out of the mkiocccentry toolkit repo:
 *
 *	https://github.com/ioccc-src/mkiocccentry
 *
 * and out of the jparse repo:
 *
 *	https://github.com/xexyl/jparse
 *
 * The origin of libpr dates back to code written by Landon Curt Noll around 2008.
 *
 * That 2008 code was copied into the jparse repo, and the mkiocccentry toolkit repo
 * by Landon Curt Noll.  While in the jparse repo, both Landon Curt Noll and
 * Cody Boone Ferguson added to and improved this code base.
 *
 * "Share and Enjoy!"
 *     --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)
 */


#if !defined(INCLUDE_PR_H)
#    define  INCLUDE_PR_H


/*
 * system includes
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include <float.h>      /* for long doubles */
#include <inttypes.h>   /* uintmax_t and intmax_t and perhaps SIZE_MAX */
#include <string.h>     /* for strcmp */
#include <sys/types.h>  /* various things */
#include <sys/stat.h>   /* for stat(2) and others */
#include <fts.h>        /* FTS and FTSENT */
#include <fnmatch.h>    /* for fnmatch(3) (for ignored paths - if desired) */
#include <limits.h>	/* for CHAR_BIT */
#include <errno.h>
#include <poll.h>



/*
 * dbg - info, debug, warning, error, and usage message facility
 */
#if defined(INTERNAL_INCLUDE)
#include "../dbg/c_bool.h"
#include "../dbg/c_compat.h"
#include "../dbg/dbg.h"
#include "../dyn_array/dyn_array.h"
#else
#include <c_bool.h>
#include <c_compat.h>
#include <dbg.h>
#include <dyn_array.h>
#endif

/*
 * definitions
 */

/*
 * official version
 */
#define PR_VERSION "1.1.3 2025-09-23"    /* format: major.minor YYYY-MM-DD */

/*
 * byte as octet constants
 */
#if !defined(CHAR_BIT)
# define CHAR_BIT (8)		/* paranoia - in case limits.h is very old */
#endif
#define BITS_IN_BYTE (CHAR_BIT)	    /* assume 8 bit bytes */
#define MAX_BYTE (0xff)		    /* maximum byte value */
#define BYTE_VALUES (MAX_BYTE+1)    /* number of different combinations of bytes */


/*
 * off_t MAX and MIN
 */
#if !defined(OFF_MAX)
#define OFF_MAX (~((off_t)1 << (sizeof(off_t) * BITS_IN_BYTE - 1)))
#endif /* OFF_MAX */
#if !defined(OFF_MIN)
#define OFF_MIN (((off_t)1 << (sizeof(off_t) * BITS_IN_BYTE - 1)))
#endif /* OFF_MIN */


/*
 * size_t MAX and MIN
 */
#if !defined(SIZE_MAX)
#define SIZE_MAX (~((size_t)0))
#endif /* SIZE_MAX */
#if !defined(SIZE_MIN)
#define SIZE_MIN ((size_t)(0))
#endif /* SIZE_MIN */


/*
 * ssize_t MAX and MIN
 */
#if !defined(SSIZE_MAX)
#define SSIZE_MAX (~((ssize_t)1 << (sizeof(ssize_t) * BITS_IN_BYTE - 1)))
#endif /* SSIZE_MAX */
#if !defined(SSIZE_MIN)
#define SSIZE_MIN (((ssize_t)1 << (sizeof(ssize_t) * BITS_IN_BYTE - 1)))
#endif /* SSIZE_MIN */

/*
 * MAX and MIN macros
 */
#if !defined(MAX)
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif /* MAX */
#if !defined(MIN)
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif /* MIN */


/*
 * invalid exit codes (values < 0) that may be returned by shell_cmd()
 */
#define EXIT_CALLOC_FAILED (-2)		/* invalid exit code - calloc() failure */
#define EXIT_SYSTEM_FAILED (-3)		/* invalid exit code - system() failed - returned exit 127 */
#define EXIT_FFLUSH_FAILED (-4)		/* invalid exit code - fflush() failed */
#define EXIT_NULL_ARGS (-5)		/* invalid exit code - function called with a NULL arg */


/*
 * utility macros for find_text functions
 */
#define is_all_text(buf, len) (find_text((buf), (len), NULL) == (len))
#define is_all_text_str(str) (is_all_text((str), strlen(str)))
#define is_all_whitespace(buf, len) (find_text((buf), (len), NULL) == 0)
#define is_all_whitespace_str(str) (is_all_whitespace((str), strlen(str)))

/*
 * useful util macros for bitvectors
 */
#define IS_SET(flag,bit)  ((flag) & (bit))
#define SET_BIT(var,bit)  ((var) |= ((uintmax_t)bit))
#define REMOVE_BIT(var,bit)  ((var) &= ~((uintmax_t)bit))
#define TOGGLE_BIT(var,bit) ((var)) = ((var)) ^ ((uintmax_t)bit)

#define LSET_BIT(var,bit)  ((var) | ((uintmax_t)bit))
#define LREMOVE_BIT(var,bit)  ((var) & ~((uintmax_t)bit))
#define LTOGGLE_BIT(var,bit) ((var) ^ ((uintmax_t)bit))

/*
 * non-strict floating match to 1 part in MATCH_PRECISION
 */
#define MATCH_PRECISION ((long double)(1<<22))


#if !defined(INITIAL_BUF_SIZE)
    #define INITIAL_BUF_SIZE (8192)	/* initial size of buffer allocated by read_all */
#endif
#if !defined(READ_ALL_CHUNK)
    #define READ_ALL_CHUNK (65536)	/* grow this read_all by this amount when needed */
#endif

/*
 * enums
 */

/*
 * for the path sanity functions
 */
enum path_sanity
{
    PATH_ERR_UNKNOWN = 0,               /* unknown error code (default in switch) */
    PATH_OK,                            /* path (str) is a sane relative path */
    PATH_ERR_PATH_IS_NULL,              /* path string (str) is NULL */
    PATH_ERR_PATH_EMPTY,                /* path string (str) is 0 length (empty) */
    PATH_ERR_PATH_TOO_LONG,             /* path (str) > max_path_len */
    PATH_ERR_MAX_PATH_LEN_0,            /* max_path_len <= 0 */
    PATH_ERR_MAX_DEPTH_0,               /* max_depth is <= 0 */
    PATH_ERR_NOT_RELATIVE,              /* path (str) not relative (i.e. it starts with a '/') */
    PATH_ERR_NAME_TOO_LONG,             /* path component > max_filename_len */
    PATH_ERR_MAX_NAME_LEN_0,            /* max filename length <= 0 */
    PATH_ERR_PATH_TOO_DEEP,             /* current depth > max_depth */
    PATH_ERR_NOT_POSIX_SAFE             /* invalid/not sane path component */
};

/*
 * enum for the find_path() functions (bits to be ORed)
 */
enum fts_type
{
    FTS_TYPE_FILE       = 1,        /* regular files type allowed */
    FTS_TYPE_DIR        = 2,        /* directories allowed */
    FTS_TYPE_SYMLINK    = 4,        /* symlinks allowed */
    FTS_TYPE_SOCK       = 8,        /* sockets allowed */
    FTS_TYPE_CHAR       = 16,       /* character devices allowed */
    FTS_TYPE_BLOCK      = 32,       /* block devices allowed */
    FTS_TYPE_FIFO       = 64,        /* FIFO allowed */
    FTS_TYPE_ANY        = FTS_TYPE_FILE | FTS_TYPE_DIR | /* all types of files */
                          FTS_TYPE_SYMLINK | FTS_TYPE_SOCK | /* all types of files */
                          FTS_TYPE_CHAR | FTS_TYPE_BLOCK | FTS_TYPE_FIFO /* all types of files allowed */
};

/*
 * file_type enum - for file_type() function to determine type of file
 */

enum file_type
{
    FILE_TYPE_ERR       = -1,   /* some error other than ENOENT (no such file or directory) */
    FILE_TYPE_ENOENT    = 0,    /* errno == ENOENT (no such file or directory) */
    FILE_TYPE_FILE      = 1,    /* regular file */
    FILE_TYPE_DIR       = 2,    /* directory */
    FILE_TYPE_SYMLINK   = 3,    /* symlink */
    FILE_TYPE_SOCK      = 4,    /* socket */
    FILE_TYPE_CHAR      = 5,    /* character device */
    FILE_TYPE_BLOCK     = 6,    /* block device */
    FILE_TYPE_FIFO      = 7     /* FIFO */
};

/*
 * structures
 */

/*
 * struct fts - for the FTS related functions
 */
struct fts
{
    FTS *tree;              /* tree from fts_open() */
    int options;            /* options for fts_read() */
    bool logical;           /* true ==> set FTS_LOGICAL, false ==> set FTS_PHYSICAL */
    enum fts_type type;     /* types of files desired (bitwise OR) */
    int count;              /* > 0 - if more than one match, return count-th match */
    int depth;              /* > 0 - make sure that the FTS level matches this depth exactly */
    int min_depth;          /* > 0 - depth of path must be >= this value if depth <= 0 */
    int max_depth;          /* > 0 - depth of path must be <= this value if depth <= 0 */
    bool base;              /* true ==> basename match */
    bool seedot;            /* true ==> analogous to FTS_SEEDOT */
    bool match_case;        /* true ==> case-sensitive match */
    struct dyn_array *ignore;   /* paths to ignore */
    struct dyn_array *match;    /* list of globs (or files) that are allowed */
    int (*cmp)(const FTSENT **, const FTSENT **);   /* function pointer to use when traversing (NULL ==> fts_cmp()) */
    int fn_ignore_flags;    /* flags for fnmatch(3) for ignored list or < 0 (default) if undesired */
    int fn_match_flags;     /* flags for fnmatch(3) to find (nothing else will be added) */
    bool(*check)(FTS *, FTSENT *);  /* function pointer to use to check an FTSENT * (NULL ==> check_fts_info()) */
    bool initialised;       /* internal use: after first call we can safely check pointers */
};

/*
 * external function declarations
 */
extern char *base_name(char const *path);
extern char *dir_name(char const *path, int level);
extern size_t count_comps(char const *str, char comp, bool remove_all);
extern size_t count_dirs(char const *path);
extern bool exists(char const *path);
extern enum file_type file_type(char const *path);
extern bool is_mode(char const *path, mode_t mode);
extern bool has_mode(char const *path, mode_t mode);
extern bool is_file(char const *path);
extern bool is_dir(char const *path);
extern bool is_symlink(char const *path);
extern bool is_socket(char const *path);
extern bool is_chardev(char const *path);
extern bool is_blockdev(char const *path);
extern bool is_fifo(char const *path);
extern bool is_exec(char const *path);
extern bool is_read(char const *path);
extern bool is_write(char const *path);
extern mode_t filemode(char const *path, bool printing);
extern bool is_open_file_stream(FILE *stream);
extern void reset_fts(struct fts *fts, bool free_ignored, bool free_match);
extern char *fts_path(FTSENT *ent);
extern int fts_cmp(const FTSENT **a, const FTSENT **b);
extern int fts_rcmp(const FTSENT **a, const FTSENT **b);
extern bool check_fts_info(FTS *fts, FTSENT *ent);
extern FTSENT *read_fts(char *dir, int dirfd, int *cwd, struct fts *fts);
extern bool array_has_path(struct dyn_array *array, char *path, bool match_case, bool fn, intmax_t *idx);
extern uintmax_t paths_in_array(struct dyn_array *array);
extern char *find_path_in_array(char *path, struct dyn_array *paths, bool match_case, bool fn, intmax_t *idx);
extern bool append_path(struct dyn_array **paths, char *str, bool unique, bool duped, bool match_case, bool fn);
extern void free_paths_array(struct dyn_array **paths, bool only_empty);
extern char *find_path(char const *path, char *dir, int dirfd, int *cwd, bool abspath, struct fts *fts);
extern struct dyn_array *find_paths(struct dyn_array *paths, char *dir, int dirfd, int *cwd, bool abspath, struct fts *fts);
extern off_t file_size(char const *path);
extern off_t size_if_file(char const *path);
extern bool is_empty(char const *path);
extern char *resolve_path(char const *cmd);
extern void flush_tty(char const *name, bool flush_stdin, bool abort_on_error);
extern size_t copyfile(char const *src, char const *dest, bool copy_mode, mode_t mode);
extern void touch(char const *path, mode_t mode);
extern void touchat(char const *path, mode_t mode, char const *dir, int dirfd);
extern int mkdirs(int dirfd, const char *str, mode_t mode);
extern enum path_sanity sane_relative_path(char const *str, uintmax_t max_path_len, uintmax_t max_filename_len,
        uintmax_t max_depth, bool dot_slash_okay);
extern char const *path_sanity_name(enum path_sanity sanity);
extern char const *path_sanity_error(enum path_sanity sanity);
extern bool path_has_component(char const *path, char const *name);
extern char *calloc_path(char const *dirname, char const *filename);
extern bool chk_stdio_printf_err(FILE *stream, int ret);
extern void para(char const *line, ...);
extern void fpara(FILE *stream, char const *line, ...);
extern void vfpr(FILE *stream, char const *name, char const *fmt, va_list ap);
extern void fpr(FILE *stream, char const *name, char const *fmt, ...)
        __attribute__((format(printf, 3, 4)));          /* 3=format 4=params */
extern void pr(char const *name, char const *fmt, ...)
        __attribute__((format(printf, 2, 3)));          /* 2=format 3=params */
extern ssize_t readline(char **linep, FILE *stream);
extern char *readline_dup(char **linep, bool strip, size_t *lenp, FILE *stream);
extern void *read_all(FILE *stream, size_t *psize);
extern void clearerr_or_fclose(FILE *stream);
extern ssize_t fprint_line_buf(FILE *stream, const void *buf, size_t len, int start, int end);
extern ssize_t fprint_line_str(FILE *stream, char *str, size_t *retlen, int start, int end);
extern bool fd_is_ready(char const *name, bool open_test_only, int fd);
extern FILE *open_dir_file(char const *dir, char const *file);
extern bool fchk_inval_opt(FILE *stream, char const *prog, int ch, int opt);
extern bool posix_plus_safe(char const *str, bool lower_only, bool slash_ok, bool first);
extern void posix_safe_chk(char const *str, size_t len, bool *slash, bool *posix_safe,
			   bool *first_alphanum, bool *upper);

#endif				/* INCLUDE_PR_H */
