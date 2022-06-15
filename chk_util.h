/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * json_ckk - support chkinfo and chkauth services
 *
 * This is currently being worked on by:
 *
 *
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * and
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * This is very much a work in progress! See jparse.h, jparse.l, jparse.y,
 * json_util.h and json_util.c.
 */


#if !defined(INCLUDE_CHK_UTIL_H)
#    define  INCLUDE_CHK_UTIL_H


/*
 * util - utility functions and definitions
 */
#include "util.h"

/*
 * json - json file structs
 */
#include "json_parse.h"

/*
 * json_util - utility functions related to json
 */
#include "json_util.h"


/*
 * defines
 */

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
 * JSON value: a linked list of all values of the same json_field (below).
 *
 * NOTE: This struct and the struct json_field below will be of use even after
 * the json parser is completed as these structs and the linked lists will allow
 * for easily determining if a field is seen too many times, not enough times
 * (or at all) etc. The line number member probably will not be relevant at this
 * point but for now it stays. The way these structs will be initialised is by
 * using the json tree walk function and calling the appropriate functions as
 * necessary. This is all an idea of how it might work prior to that
 * functionality and the completion of the json parser but having these structs
 * will be of use (it also means I would not have to completely rewrite the
 * functions that check the found json fields).
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
 * NOTE: This struct and the struct json_value above will be of use even after
 * the json parser is completed as these structs and the linked lists will allow
 * for easily determining if a field is seen too many times, not enough times
 * (or at all) etc. Some of the members might not be relevant at this point but
 * for now they're all staying (and are all currently being used). The way these
 * structs will be initialised is by using the json tree walk function and
 * calling the appropriate functions as necessary. This is all an idea of how it
 * might work prior to that functionality and the completion of the json parser
 * but having these structs will be of use (it also means I would not have to
 * completely rewrite the functions that check the found json fields).
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
     * Data type: one of JSON_NUM, JTYPE_BOOL, JSON_CHARS or
     * JTYPE_ARRAY_ equivalents.
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


/*
 * global variables
 */

/*
 * common JSON fields
 */
extern struct json_field common_json_fields[];
extern size_t SIZEOF_COMMON_JSON_FIELDS_TABLE;

/*
 * .info.json fields
 */
extern struct json_field info_json_fields[];
extern size_t SIZEOF_INFO_JSON_FIELDS_TABLE;

/*
 * .author.json fields
 */
extern struct json_field author_json_fields[];
extern size_t SIZEOF_AUTHOR_JSON_FIELDS_TABLE;

/*
 * linked list of the common json fields found in the .info.json and
 * .author.json files.
 *
 * A common json field is a field that is supposed to be in both .info.json and
 * .author.json.
 */
extern struct json_field *found_common_json_fields;

/*
 * global for chk_warn
 */
extern bool show_full_json_warnings;


/*
 * external function declarations
 */
extern bool is_json_code_ignored(int code);
extern void ignore_json_code(int code);
extern struct json_field *find_json_field_in_table(struct json_field *table, char const *name, size_t *loc);
extern void check_json_fields_tables(void);
extern void check_common_json_fields_table(void);
extern void check_info_json_fields_table(void);
extern void check_author_json_fields_table(void);
extern char const *json_filename(int type);
extern int check_first_json_char(char const *file, char *data, char **first, char ch);
extern int check_last_json_char(char const *file, char *data, char **last, char ch);
extern struct json_field *add_found_common_json_field(char const *json_filename, char const *name, char const *val, int line_num);
extern bool add_common_json_field(char const *json_filename, char *name, char *val, int line_num);
extern int check_found_common_json_fields(char const *json_filename, char const *fnamchk, bool test);
extern struct json_field *new_json_field(char const *json_filename, char const *name, char const *val, int line_num);
extern struct json_value *add_json_value(char const *json_filename, struct json_field *field, char const *val, int line_num);
extern void free_json_field_values(struct json_field *field);
extern void free_found_common_json_fields(void);
extern void free_json_field(struct json_field *field);
extern bool chk_warn(int code, char const *name, char const *filename,
		     char const *line, int line_num, const char *fmt, ...) \
		     __attribute__((format(printf, 6, 7)));		/* 6=format 7=params */
extern bool chk_warnp(int code, char const *name, char const *filename,
		      char const *line, int line_num, const char *fmt, ...) \
		      __attribute__((format(printf, 6, 7)));		/* 6=format 7=params */
extern void chk_err(int exitcode, const char *name, char const *filename,
		    char const *line, int line_num, const char *fmt, ...) \
		    __attribute__((noreturn)) __attribute__((format(printf, 6, 7))); /* 6=format 7=params */
extern void chk_errp(int exitcode, const char *name,
		     char const *filename, char const *line, int line_num, const char *fmt, ...) \
		     __attribute__((noreturn)) __attribute__((format(printf, 6, 7))); /* 6=format 7=params */


#endif /* INCLUDE_CHK_UTIL_H */
