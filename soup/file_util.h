/*
 * file_util - common utility functions for file operations
 *
 * "Courage is found in unlikely places."
 *
 *      -- J.R.R Tolkien
 *
 * Copyright (c) 2022-2025 by Cody Boone Ferguson and Landon Curt Noll. All
 * rights reserved.
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
 * This JSON parser, library and tools were co-developed in 2022-2025 by Cody Boone
 * Ferguson and Landon Curt Noll:
 *
 *  @xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 * and:
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * "Share and Enjoy!"
 *     --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)
 */


#if !defined(INCLUDE_FILE_UTIL_H)
#    define  INCLUDE_FILE_UTIL_H

#include <float.h>      /* for long doubles */
#include <inttypes.h>   /* uintmax_t and intmax_t and perhaps SIZE_MAX */
#include <string.h>     /* for strcmp */
#include <sys/types.h>  /* various things */
#include <sys/stat.h>   /* for stat(2) and others */
#include <fts.h>        /* FTS and FTSENT */
#include <fnmatch.h>    /* for fnmatch(3) (for ignored paths - if desired) */
#include <limits.h>	/* for CHAR_BIT */


/*
 * dbg - info, debug, warning, error, and usage message facility
 * dyn_array - dynamic array facility
 * util - various utils
 * jparse/util - various other functions
 */
#include "../dbg/dbg.h"
#include "../dyn_array/dyn_array.h"


/*
 * byte as octet constants
 */
#if !defined(CHAR_BIT)
 #define CHAR_BIT (8)		/* paranoia - in case limits.h is very old */
#endif
#if !defined(BITS_IN_BYTE)
 #define BITS_IN_BYTE (CHAR_BIT)	    /* assume 8 bit bytes */
#endif
#if !defined(BYTE_VALUES)
 #define BYTE_VALUES (1<<CHAR_BIT)   /* number of different combinations of bytes */
#endif
#if !defined(MAX_BYTE)
 #define MAX_BYTE (BYTE_VALUES-1)    /* maximum byte value */
#endif

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
 * definitions
 */
#define LITLEN(x) (sizeof(x)-1)	/* length of a literal string w/o the NUL byte */
#define INITIAL_BUF_SIZE (8192)	/* initial size of buffer allocated by read_all */
#define LLONG_MAX_BASE10_DIGITS (19) /* for string to int functions */
#define TBLLEN(x) (sizeof(x)/sizeof((x)[0]))	/* number of elements in an initialized table array */
#define UNUSED_ARG(x) (void)(x)			/* prevent compiler from complaining about an unused arg */


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

/*
 * st_mode related convenience macros
 *
 * See also stat(2)
 */
#define ITEM_TYPE(st_mode) ((st_mode) & (S_IFMT))
/**/
#define FILE_TYPE (S_IFREG)
#define DIR_TYPE (S_IFDIR)
#define SYMLINK_TYPE (S_IFLNK)
/**/
#define ITEM_IS_FILE(st_mode) (ITEM_TYPE(st_mode) == FILE_TYPE)
#define ITEM_IS_DIR(st_mode) (ITEM_TYPE(st_mode) == DIR_TYPE)
#define ITEM_IS_SYMLINK(st_mode) (ITEM_TYPE(st_mode) == SYMLINK_TYPE)
/**/
#define ITEM_IS_NOT_FILE(st_mode) (ITEM_TYPE(st_mode) != FILE_TYPE)
#define ITEM_IS_NOT_DIR(st_mode) (ITEM_TYPE(st_mode) != DIR_TYPE)
#define ITEM_IS_NOT_SYMLINK(st_mode) (ITEM_TYPE(st_mode) != SYMLINK_TYPE)
/**/
#define ITEM_IS_FILEDIR(st_mode) (ITEM_IS_FILE(st_mode) || ITEM_IS_DIR(st_mode))
#define ITEM_IS_FILESYM(st_mode) (ITEM_IS_FILE(st_mode) || ITEM_IS_SYMLINK(st_mode))
#define ITEM_IS_DIRSYM(st_mode) (ITEM_IS_DIR(st_mode) || ITEM_IS_SYMLINK(st_mode))
#define ITEM_IS_FILEDIRSYM(st_mode) (ITEM_IS_FILE(st_mode) || ITEM_IS_DIR(st_mode) || ITEM_IS_SYMLINK(st_mode))
/**/
#define ITEM_IS_NOT_FILEDIR(st_mode) (! ITEM_IS_FILEDIR(st_mode))
#define ITEM_IS_NOT_FILESYM(st_mode) (! ITEM_IS_FILESYM(st_mode))
#define ITEM_IS_NOT_DIRSYM(st_mode) (! ITEM_IS_DIRSYM(st_mode))
#define ITEM_IS_NOT_FILEDIRSYM(st_mode) (! ITEM_IS_FILEDIRSYM(st_mode))


/*
 * enums
 */

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
extern char *base_name(char const *path);
extern char *dir_name(char const *path);
extern bool exists(char const *path);
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
/* XXX - remove after table driven walk code is in place - XXX */
extern char *fts_path(FTSENT *ent);
extern FTSENT *read_fts(char *dir, int dirfd, int *cwd, struct fts *fts);
extern bool array_has_path(struct dyn_array *array, char *path, bool match_case, bool fn, intmax_t *idx);
extern uintmax_t paths_in_array(struct dyn_array *array);
/* XXX - remove after table driven walk code is in place - XXX */
extern char *find_path_in_array(char *path, struct dyn_array *paths, bool match_case, bool fn, intmax_t *idx);
extern bool append_path(struct dyn_array **paths, char *str, bool unique, bool duped, bool match_case, bool fn);
extern void free_paths_array(struct dyn_array **paths, bool only_empty);
extern char *find_path(char const *path, char *dir, int dirfd, int *cwd, bool abspath, struct fts *fts);
extern struct dyn_array *find_paths(struct dyn_array *paths, char *dir, int dirfd, int *cwd, bool abspath, struct fts *fts);
extern off_t file_size(char const *path);
extern off_t size_if_file(char const *path);
extern bool is_empty(char const *path);
extern char *resolve_path(char const *cmd);
extern size_t copyfile(char const *src, char const *dest, bool copy_mode, mode_t mode);
extern void touch(char const *path, mode_t mode);
extern void touchat(char const *path, mode_t mode, char const *dir, int dirfd);
extern int mkdirs(int dirfd, const char *str, mode_t mode);
extern bool path_has_component(char const *path, char const *name);
extern char *calloc_path(char const *dirname, char const *filename);
extern char const *file_type_name(mode_t st_mode);

#endif				/* INCLUDE_FILE_UTIL_H */
