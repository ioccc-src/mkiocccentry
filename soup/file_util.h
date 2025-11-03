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
 * number of bytes to hold an int converted to decimal
 */
#if !defined(INT_DECIMAL_SIZE)
  #define INT_DECIMAL_SIZE ((((((size_t)CHAR_BIT) * sizeof(int)) * 302) / 1000) + 1)
#endif

/*
 * invalid exit codes (values < 0) that may be returned by shell_cmd()
 */
#define EXIT_CALLOC_FAILED (-2)		/* invalid exit code - calloc() failure */
#define EXIT_SYSTEM_FAILED (-3)		/* invalid exit code - system() failed - returned exit 127 */
#define EXIT_FFLUSH_FAILED (-4)		/* invalid exit code - fflush() failed */
#define EXIT_NULL_ARGS (-5)		/* invalid exit code - function called with a NULL arg */

/*
 * st_mode related convenience macros
 *
 * See also stat(2) and chmod(2)
 */
#define ITEM_TYPE(st_mode) ((st_mode) & (S_IFMT))
#define ITEM_PERM_BITS (S_ISUID | S_ISGID | S_ISVTX | S_IRWXU | S_IRWXG | S_IRWXO)
#define ITEM_PERM_0755 (S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)
#define ITEM_PERM_0555 (S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)
#define ITEM_PERM_0444 (S_IRUSR | S_IRGRP | S_IROTH)
#define ITEM_PERM(st_mode) ((st_mode) & ITEM_PERM_BITS)
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
extern mode_t filemode(char const *path);
extern bool is_open_file_stream(FILE *stream);
extern bool array_has_path(struct dyn_array *array, char *path, bool match_case, bool fn, intmax_t *idx);
extern off_t file_size(char const *path);
extern off_t size_if_file(char const *path);
extern bool is_empty(char const *path);
extern char *resolve_path(char const *cmd);
extern size_t copyfile(char const *src, char const *dest, bool copy_mode, mode_t mode);
extern void touch(char const *path, mode_t mode);
extern void touchat(char const *path, mode_t mode, char const *dir, int dirfd);
extern int mkdirs(int dirfd, const char *str, mode_t mode);
extern char *calloc_path(char const *dirname, char const *filename);
extern char const *file_type_name(mode_t st_mode);
extern int chdir_save_cwd(char const *newdir);
extern void restore_cwd(int prev_cwd);
extern bool file_mode_size(char const *path, mode_t *st_mode, off_t *st_size);

#endif				/* INCLUDE_FILE_UTIL_H */
