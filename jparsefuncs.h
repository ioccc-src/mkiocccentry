/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * jparsefuncs - structs and functions specific to jparse and related code
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * This is currently being worked on by:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 *
 * and
 *
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * This is very much a work in progress!
 */


#if !defined(INCLUDE_JPARSEFUNCS_H)
#    define  INCLUDE_JPARSEFUNCS_H


/*
 * util - utility functions and definitions
 */
#include "util.h"


/*
 * json - json file structs
 */
#include "json.h"


/*
 * dbg - debug, warning and error reporting facility
 */
#include "dbg.h"

/*
 * IOCCC size and rule related limitations
 */
#include "limit_ioccc.h"


/*
 * the next five are for the json_filename() function
 */
#define INFO_JSON	(0)	    /* file is assumed to be a .info.json file */
#define AUTHOR_JSON	(1)	    /* file is assumed to be a .author.json file */
#define INFO_JSON_FILENAME ".info.json"
#define AUTHOR_JSON_FILENAME ".author.json"
#define INVALID_JSON_FILENAME "null"

#define FORMED_UTC_FMT "%a %b %d %H:%M:%S %Y UTC"   /* format of strptime() for formed_UTC check */

/*
 * JSON warn/error code struct, variables, constants and macros for the jwarn()
 * and jerr() functions. Functions are prototyped later in the file.
 */
extern bool show_full_json_warnings;

/*
 * JSON warning codes to ignore
 *
 * When a tool is given command line arguments of the form:
 *
 *	.. -W 123 -W 1345 -W 56 ...
 *
 * this means the tool will ignore {JSON-0123}, {JSON-1345} and {JSON-0056}.
 * The ignore_json_code_set[] table holds the JSON codes to ignore.
 */
#define JSON_CODE_IGNORE_CHUNK (64)	/* number of codes to calloc or realloc at a time */

struct ignore_json_code
{
    int next_free;	/* the index of the next allowed but free JSON error code */
    int alloc;		/* number of JSON error codes allocated */
    int *code;		/* pointer to the allocated list of codes, or NULL (not allocated) */
};
extern struct ignore_json_code *ignore_json_code_set;

/*
 * JSON warn / error codes
 *
 * Codes 0 - 199 are reserved for special purposes so all normal codes should be
 * >= JSON_CODE_MIN && <= JSON_CODE_MAX via the JSON_CODE macro.
 *
 * NOTE: The reason 200 codes are reserved is because it's more than enough ever
 * and we don't want to have to ever change the codes after the parser and
 * checkers are complete as this would cause problems for the tools as well as
 * the test suites. Previously the range was 0 - 99 and although this is also
 * probably more than enough we want to be sure that there is never a problem
 * and we cannot imagine how 200 codes will ever not be a large enough range but
 * if the use of reserved codes change this might not be the case for just 100
 * (unlikely though that is).
 */
/* reserved code: all normal codes should be >= JSON_CODE_MIN && <= JSON_CODE_MAX via JSON_CODE macro */
#define JSON_CODE_RESERVED_MIN (0)
/* reserved code: all normal codes should be >= JSON_CODE_MIN && <= JSON_CODE_MAX via JSON_CODE macro */
#define JSON_CODE_RESERVED_MAX (199)
/* based on minimum reserved code, form an invalid json code number */
#define JSON_CODE_INVALID (JSON_CODE_RESERVED_MIN - 1)
/*
 * The minimum code for jwarn() is the JSON_CODE_RESERVED_MAX (currently 199) + 1.
 * However this does not mean that calls to jwarn() cannot use <= the
 * JSON_CODE_RESERVED_MAX: it's just for special purposes e.g. codes that are
 * used in more than one location.
 */
#define JSON_CODE_MIN (1+JSON_CODE_RESERVED_MAX)
/* The maximum json code for jwarn()/jerr(). This was arbitrarily selected. */
#define JSON_CODE_MAX (9999)
/* the number of unreserved JSON codes for jwarn()/jerr(): the max - the min + 1 */
#define NUM_UNRESERVED_JSON_CODES (JSON_CODE_MAX-JSON_CODE_MIN)
/*
 * To distinguish that a number is a JSON warn/error code rather than any other
 * type of number we use these macros.
 */
#define JSON_CODE(x) ((x)+JSON_CODE_RESERVED_MAX)
/* reserved JSON code */
#define JSON_CODE_RESERVED(x) (x)

/*
 * JSON value: a linked list of all values of the same json_field (below).
 *
 * NOTE: As the parser is built this struct (and struct json_field below) might
 * be removed but this has not been decided yet.
 */
struct json_value
{
    char *value;

    int line_num;	    /* the 'line number': actually the field number in the list */

    struct json_value *next;
};

/*
 * JSON field: a JSON field consists of the name and all the values (if more
 * than one field of the same name is found in the file).
 *
 * NOTE: As the parser is built this struct (and struct json_value above) might
 * be removed but this has not been decided yet.
 */
struct json_field
{
    char *name;			/* field name */
    struct json_value *values;	/* linked list of values */

    /*
     * the below are for the tables: common_json_fields, info_json_fields and
     * author_json_fields:
     *
     * Number of times this field has been seen in the list, how many are
     * actually allowed and whether the field has been found: This is for the
     * tables that say many times a field has been seen, how many times it
     * is allowed and whether or not it's been seen (it is true that this could
     * simply be count > 0 but this is to be more clear, however slight).
     *
     * In other words this is done as part of the checks after the field:value
     * pairs have been extracted.
     *
     * NOTE: A max_count == 0 means that there's no limit and that it's not
     * required.
     */
    size_t count;		/* how many of this field in the list (or how many values) */
    size_t max_count;		/* how many of this field is allowed */
    bool found;			/* if this field was found */

    /*
     * These are used in both checking and parsing: checking that the data is
     * valid and parsing in that certain data types have to be parsed
     * differently.
     *
     * Data type: one of JSON_NUM, JSON_BOOL, JSON_CHARS or
     * JSON_ARRAY_ equivalents.
     */
    int field_type;
    /*
     * Some strings can be empty but others cannot; there are no other values
     * that can currently be empty but this is for all lists so we have to
     * include this bool
     */
    bool can_be_empty;	    /* if the value can be empty */


    /* NOTE: don't add to more than one list */
    struct json_field *next;	/* the next in the whatever list */
};

extern struct json_field common_json_fields[];
extern size_t SIZEOF_COMMON_JSON_FIELDS_TABLE;

/*
 * linked list of the common json fields found in the .info.json and
 * .author.json files.
 *
 * A common json field is a field that is supposed to be in both .info.json and
 * .author.json.
 */
struct json_field *found_common_json_fields;


/* function prototypes */

/* jinfochk and jauthchk related */
extern struct json_field *find_json_field_in_table(struct json_field *table, char const *name, size_t *loc);
extern char const *json_filename(int type);
/* checks on the JSON fields tables */
extern void check_json_fields_tables(void);
extern void check_common_json_fields_table(void);
extern void check_author_json_fields_table(void);
extern void check_info_json_fields_table(void);
extern int check_first_json_char(char const *file, char *data, char **first, char ch);
extern int check_last_json_char(char const *file, char *data, char **last, char ch);
extern struct json_field *add_found_common_json_field(char const *json_filename, char const *name, char const *val, int line_num);
extern bool add_common_json_field(char const *program, char const *json_filename, char *name, char *val, int line_num);
extern int check_found_common_json_fields(char const *program, char const *json_filename, char const *fnamchk, bool test);
extern struct json_field *new_json_field(char const *json_filename, char const *name, char const *val, int line_num);
extern struct json_value *add_json_value(char const *json_filename, struct json_field *field, char const *val, int line_num);

/* warning and error specific functions */


extern void jwarn(int code, const char *program, char const *name, char const *filename, char const *line,
		  int line_num, const char *fmt, ...) \
	__attribute__((format(printf, 7, 8)));		/* 7=format 8=params */
extern void jwarnp(int code, const char *program, char const *name, char const *filename, char const *line,
		   int line_num, const char *fmt, ...) \
	__attribute__((format(printf, 7, 8)));		/* 7=format 8=params */
extern void jerr(int exitcode, char const *program, const char *name, char const *filename, char const *line,
		 int line_num, const char *fmt, ...) \
	__attribute__((noreturn)) __attribute__((format(printf, 7, 8))); /* 7=format 8=params */
extern void jerrp(int exitcode, char const *program, const char *name, char const *filename, char const *line,
		  int line_num, const char *fmt, ...) \
	__attribute__((noreturn)) __attribute__((format(printf, 7, 8))); /* 7=format 8=params */

/* ignored warnings via the -W option in jinfochk and jauthchk */
extern bool is_json_code_ignored(int code);
extern void ignore_json_code(int code);


/* free() functions for the parser */
extern void free_json_field_values(struct json_field *field);
extern void free_found_common_json_fields(void);
extern void free_json_field(struct json_field *field);

#endif /* INCLUDE_JPARSEFUNCS_H */
