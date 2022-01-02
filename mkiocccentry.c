/*
 * mkiocccentry - make an ioccc entry
 *
 * Make an IOCCC compressed tarball for an IOCCC entry.
 *
 * We will form the IOCCC entry compressed tarball "by hand" in C.
 * Why?  Because this is a C contest.  But then why isn't this
 * code obfuscated?  Because the IOCCC judges prefer to write
 * robust unobfuscated code.  Besides, the IOCCC was started
 * as an ironic commentary on the Bourne shell and finger daemon.
 * Well, irony is well baked-in to the IOCCC.  :-)
 *
 * If you do find a problem with this code, let the judges know.
 *
 * Copyright (c) 2021,2022 by Landon Curt Noll.  All Rights Reserved.
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
 * LANDON CURT NOLL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL LANDON CURT NOLL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * Share and enjoy! :-)
 */

/*ooo*/ /* exit code out of numerical order - ignore in sequencing */
/*coo*/ /* exit code change of order - use new value in sequencing */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <ctype.h>


/*
 * definitions
 */
#define VERSION "0.6 2022-01-01"	/* use format: major.minor YYYY-MM-DD */
#define REQUIRED_IOCCCSIZE_MAJVER (28)	/* iocccsize major version must match */
#define MIN_IOCCCSIZE_MINVER (2)	/* iocccsize minor version must be >= */
#define DBG_NONE (0)		/* no debugging */
#define DBG_LOW (1)		/* minimal debugging */
#define DBG_MED (3)		/* somewhat more debugging */
#define DBG_HIGH (5)		/* verbose debugging */
#define DBG_VHIGH (7)		/* very verbose debugging */
#define DBG_VVHIGH (9)		/* very very verbose debugging */
#define DBG_DEFAULT (DBG_NONE)	/* default level of debugging */
#define UUID_LEN (36)		/* characters in a UUID string */
#define UUID_VERSION (4)	/* version 4 - random UUID */
#define UUID_VARIANT (0xa)	/* variant 1 - encoded as 0xa */
#define MAX_ENTRY_NUM (9)	/* entry numbers from 0 to MAX_ENTRY_NUM allowed */
#define MAX_ENTRY_CHARS (1)	/* characters that represent the maximum entry number */
#define MAX_AUTHORS (5)		/* maximum number of authors of an entry */
#define MAX_NAME_LEN (64)	/* max author name length */
#define MAX_EMAIL_LEN (64)	/* max Email address length */
#define MAX_URL_LEN (64)	/* max home URL, including http:// or https:// */
#define MAX_TWITTER_LEN (18+1)	/* max twitter handle, including the leading @, length */
#define MAX_GITHUB_LEN (15+1)	/* max GitHub account, including the leading @, length */
#define MAX_AFFILIATION_LEN (64)/* max affiliation name length */
#define ISO_3166_1_CODE_URL0 "    https://en.wikipedia.org/wiki/ISO_3166-1#Officially_assigned_code_elements"
#define ISO_3166_1_CODE_URL1 "    https://en.wikipedia.org/wiki/ISO_3166-1_alpha-2"
#define ISO_3166_1_CODE_URL2 "    https://www.iso.org/obp/ui/#iso:pub:PUB500001:en"
#define ISO_3166_1_CODE_URL3 "    https://www.iso.org/obp/ui/#search"
#define ISO_3166_1_CODE_URL4 "    https://www.iso.org/glossary-for-iso-3166.html"


/*
 * standard truth :-)
 */
#if defined __STDC__ && defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
/* have a C99 compiler - we should expect to have <stdbool.h> */
# include <stdbool.h>
#else
/* do not have a C99 compiler - fake a <stdbool.h> header */
typedef unsigned char bool;
# undef true
# define true ((bool)(1))
# undef false
# define false ((bool)(0))
#endif


/*
 * usage message
 *
 * Use the usage() function to print the usage message.
 */
static char const *usage_msg =
"usage: %s [-h] [-v level] [-V] [-t tar] [-c cp] [-l ls] work_dir iocccsize\n"
"\n"
"\t-h\t\tprint help message and exit 0\n"
"\t-v level\tset verbosity level: (def level: %d)\n"
"\t-V\t\tprint version string and exit\n"
"\t-t tar\t\tpath to tar executable that supports -cjvf (def: %s)\n"
"\t-c cp\t\tpath to cp executable (def: %s)\n"
"\t-l ls\t\tpath to ls executable (def: %s)\n";
static char const *usage_msg2 =
"\n"
"\twork_dir\tdirectory where the entry directory and tarball are formed\n"
"\tiocccsize\tpath to the iocccsize tool\n"
"\t\t\tNOTE: Source for the iocccsize tool may be found at:\n"
"\n"
"\t\t\t    https://www.ioccc.org/YYYY/iocccsize.c\n"
"\n"
"\t\t\twhere YYYY is the IOCCC contest year.\n"
"\n"
"mkiocccentry version: %s\n";


/*
 * author info
 */
struct author {
    char *name;			/* name of the author */
    char *location_code;	/* author country code */
    char *email;		/* Email address of author or empty string */
    char *url;			/* home URL of author or empty string */
    char *twitter;		/* author twitter handle or empty string */
    char *github_user;		/* author GitHub username or empty string */
    char *affiliation;		/* author affiliation or empty string */
    int author_num;		/* author number */
};


/*
 * location/country codes
 *
 * We list all Officially assigned code elements,
 * all Exceptionally reserved code elements,
 * all Transitionally reserved code elements,
 * all Indeterminately reserved code elements,
 * all Formerly assigned code elements,
 * all User-assigned code elements,
 * as of 2021 Dec 31.  See:
 *
 *	https://en.wikipedia.org/wiki/ISO_3166-1#Officially_assigned_code_elements
 *	https://en.wikipedia.org/wiki/ISO_3166-1_alpha-2
 *	https://www.iso.org/obp/ui/#iso:pub:PUB500001:en
 *	https://www.iso.org/obp/ui/#search
 *
 * See also:
 *
 *	https://www.iso.org/glossary-for-iso-3166.html
 *
 * This means you will find a few codes that belong to
 * entities that no longer are recognized, entities
 * that are/were temporary, codes that are reserved for
 * future use, codes for Road Traffic Conventions,
 * codes for Property Organizations, duplicate codes, etc.
 *
 * We do not list WIPO codes as they are not formally
 * reserved.  They refer to Patent and related WIPO
 * organizations: as such they do not represent places
 * where a IOCCC winner would live.
 *
 * We mean no offense by this list: we simply tried to
 * include all ISO 3166 codes.  Please pardon any typos.
 * Spelling corrections welcome.
 */
static struct location {
    char *code;		/* ISO 3166-1 Alpha-2 Code */
    char *name;		/* name (short name lower case) */
} loc[] = {
    {"AA", "User-assigned code AA"}, /* User-assigned code */
    {"AC", "Ascension Island"}, /* Exceptionally reserved code */
    {"AD", "Andorra"},
    {"AE", "United Arab Emirates"},
    {"AF", "Afghanistan"},
    {"AG", "Antigua and Barbuda"},
    {"AI", "Anguilla"},
    {"AL", "Albania"},
    {"AM", "Armenia"},
    {"AN", "Netherlands Antilles"}, /* Transitionally reserved code */
    {"AO", "Angola"},
/*  {"AP", "African Regional Industrial Property Organization"}, */ /* WIPO Indeterminately reserved code */
    {"AQ", "Antarctica"},
    {"AR", "Argentina"},
    {"AS", "American Samoa"},
    {"AT", "Austria"},
    {"AU", "Australia"},
    {"AW", "Aruba"},
    {"AX", "Åland Islands"},
    {"AZ", "Azerbaijan"},
    {"BA", "Bosnia and Herzegovina"},
    {"BB", "Barbados"},
    {"BD", "Bangladesh"},
    {"BE", "Belgium"},
    {"BF", "Burkina Faso"},
    {"BG", "Bulgaria"},
    {"BH", "Bahrain"},
    {"BI", "Burundi"},
    {"BJ", "Benin"},
    {"BL", "Saint Barthélemy"},
    {"BM", "Bermuda"},
    {"BN", "Brunei Darussalam"},
    {"BO", "Bolivia (Plurinational State of)"},
    {"BQ", "Bonaire, Sint Eustatius and Saba"},
    {"BR", "Brazil"},
    {"BS", "Bahamas"},
    {"BT", "Bhutan"},
    {"BU", "Burma"}, /* Transitionally reserved code */
    {"BV", "Bouvet Island"},
    {"BW", "Botswana"},
/*  {"BX", "Benelux Trademarks and Designs Office"}, */ /* WIPO Indeterminately reserved code */
    {"BY", "Belarus"},
    {"BZ", "Belize"},
    {"CA", "Canada"},
    {"CC", "Cocos (Keeling) Islands (the)"},
    {"CD", "Congo, Democratic Republic of the"},
    {"CF", "Central African Republic"},
    {"CG", "Congo (the)"},
    {"CH", "Switzerland"},
    {"CI", "Côte d'Ivoire"},
    {"CK", "Cook Islands"},
    {"CL", "Chile"},
    {"CM", "Cameroon"},
    {"CN", "China"},
    {"CO", "Colombia"},
    {"CP", "Clipperton Island"}, /* Exceptionally reserved code */
    {"CQ", "island of Sark"}, /* Exceptionally reserved code */
    {"CR", "Costa Rica"},
    {"CS", "Serbia and Montenegro"}, /* Transitionally reserved code */
    {"CT", "Canton and Enderbury Island"}, /* Formerly assigned code */
    {"CU", "Cuba"},
    {"CV", "Cabo Verde"},
    {"CW", "Curaçao"},
    {"CX", "Christmas Island"},
    {"CY", "Cyprus"},
    {"CZ", "Czechia"},
    {"DD", "German Democratic Republic"}, /* Formerly assigned code */
    {"DE", "Germany"},
    {"DG", "Diego Garcia"}, /* Exceptionally reserved code */
    {"DJ", "Djibouti"},
    {"DK", "Denmark"},
    {"DM", "Dominica"},
    {"DO", "Dominican Republic"},
    {"DY", "Benin"}, /* Indeterminately reserved code */
    {"DZ", "Algeria"},
    {"EA", "Ceuta, Melilla"}, /* Exceptionally reserved code */
    {"EC", "Ecuador"},
    {"EE", "Estonia"},
/*  {"EF", "Union of Countries under the European Community Patent Convention"}, */ /* WIPO Indeterminately reserved code */
    {"EG", "Egypt"},
    {"EH", "Western Sahara"},
/*  {"EM", "European Trademark Office"}, */ /* WIPO Indeterminately reserved code */
/*  {"EP", "European Patent Organization"}, */ /* WIPO Indeterminately reserved code */
    {"ER", "Eritrea"},
    {"ES", "Spain"},
    {"ET", "Ethiopia"},
    {"EU", "European Union"}, /* Exceptionally reserved code */
/*  {"EV", "Eurasian Patent Organization"}, */ /* WIPO Indeterminately reserved code */
    {"EW", "Estonia"}, /* Indeterminately reserved code */
    {"EZ", "European OTC derivatives"}, /* Exceptionally reserved code */
    {"FI", "Finland"},
    {"FJ", "Fiji"},
    {"FK", "Falkland Islands (the) [Malvinas]"},
    {"FL", "Liechtenstein"}, /* Indeterminately reserved code */
    {"FM", "Micronesia (Federated States of)"},
    {"FO", "Faroe Islands"},
    {"FQ", "French Southern and Antarctic Territories"}, /* Formerly assigned code */
    {"FR", "France"},
    {"FX", "France, Metropolitan"}, /* Exceptionally reserved code */
    {"GA", "Gabon"},
    {"GB", "United Kingdom of Great Britain and Northern Ireland (the)"},
/*  {"GC", "Patent Office of the Cooperation Council for the Arab States of the Gulf"}, */ /* WIPO Indeterminately reserved code */
    {"GD", "Grenada"},
    {"GE", "Georgia"},
    {"GF", "French Guiana"},
    {"GG", "Guernsey"},
    {"GH", "Ghana"},
    {"GI", "Gibraltar"},
    {"GL", "Greenland"},
    {"GM", "Gambia"},
    {"GN", "Guinea"},
    {"GP", "Guadeloupe"},
    {"GQ", "Equatorial Guinea"},
    {"GR", "Greece"},
    {"GS", "South Georgia and the South Sandwich Islands"},
    {"GT", "Guatemala"},
    {"GU", "Guam"},
    {"GW", "Guinea-Bissau"},
    {"GY", "Guyana"},
    {"HK", "Hong Kong"},
    {"HM", "Heard Island and McDonald Islands"},
    {"HN", "Honduras"},
    {"HR", "Croatia"},
    {"HT", "Haiti"},
    {"HU", "Hungary"},
    {"HV", "Upper Volta"}, /* Formerly assigned code */
/*  {"IB", "International Bureau of WIPO"}, */ /* WIPO Indeterminately reserved code */
    {"IC", "Canary Islands"}, /* Exceptionally reserved code */
    {"ID", "Indonesia"},
    {"IE", "Ireland"},
    {"IL", "Israel"},
    {"IM", "Isle of Man"},
    {"IN", "India"},
    {"IO", "British Indian Ocean Territory"},
    {"IQ", "Iraq"},
    {"IR", "Iran (Islamic Republic of)"},
    {"IS", "Iceland"},
    {"IT", "Italy"},
    {"JA", "Jamaica"}, /* Indeterminately reserved code */
    {"JE", "Jersey"},
    {"JM", "Jamaica"},
    {"JO", "Jordan"},
    {"JP", "Japan"},
    {"JT", "Johnston Island"}, /* Formerly assigned code */
    {"KE", "Kenya"},
    {"KG", "Kyrgyzstan"},
    {"KH", "Cambodia"},
    {"KI", "Kiribati"},
    {"KM", "Comoros"},
    {"KN", "Saint Kitts and Nevis"},
    {"KP", "Korea (the Democratic People's Republic of)"},
    {"KR", "Korea (the Republic of)"},
    {"KW", "Kuwait"},
    {"KY", "Cayman Islands"},
    {"KZ", "Kazakhstan"},
    {"LA", "Lao People's Democratic Republic"},
    {"LB", "Lebanon"},
    {"LC", "Saint Lucia"},
    {"LF", "Libya Fezzan"}, /* Indeterminately reserved code */
    {"LI", "Liechtenstein"},
    {"LK", "Sri Lanka"},
    {"LR", "Liberia"},
    {"LS", "Lesotho"},
    {"LT", "Lithuania"},
    {"LU", "Luxembourg"},
    {"LV", "Latvia"},
    {"LY", "Libya"},
    {"MA", "Morocco"},
    {"MC", "Monaco"},
    {"MD", "Moldova (the Republic of)"},
    {"ME", "Montenegro"},
    {"MF", "Saint Martin (French part)"},
    {"MG", "Madagascar"},
    {"MH", "Marshall Islands (the)"},
    {"MI", "Midway Islands"}, /* Formerly assigned code */
    {"MK", "North Macedonia"},
    {"ML", "Mali"},
    {"MM", "Myanmar"},
    {"MN", "Mongolia"},
    {"MO", "Macao"},
    {"MP", "Northern Mariana Islands"},
    {"MQ", "Martinique"},
    {"MR", "Mauritania"},
    {"MS", "Montserrat"},
    {"MT", "Malta"},
    {"MU", "Mauritius"},
    {"MV", "Maldives"},
    {"MW", "Malawi"},
    {"MX", "Mexico"},
    {"MY", "Malaysia"},
    {"MZ", "Mozambique"},
    {"NA", "Namibia"},
    {"NC", "New Caledonia"},
    {"NE", "Niger"},
    {"NF", "Norfolk Island"},
    {"NG", "Nigeria"},
    {"NH", "New Hebrides"}, /* Formerly assigned code */
    {"NI", "Nicaragua"},
    {"NL", "Netherlands"},
    {"NO", "Norway"},
    {"NP", "Nepal"},
    {"NQ", "Dronning Maud Land"}, /* Formerly assigned code */
    {"NR", "Nauru"},
    {"NT", "Neutral Zone"}, /* Transitionally reserved code */
    {"NU", "Niue"},
    {"NZ", "New Zealand"},
/*  {"OA", "African Intellectual Property Organization"}, */ /* WIPO Indeterminately reserved code */
    {"OO", "user-assigned escape code"}, /* User-assigned code */
    {"OM", "Oman"},
    {"PA", "Panama"},
    {"PC", "Pacific Islands Trust Territory"}, /* Formerly assigned code */
    {"PE", "Peru"},
    {"PF", "French Polynesia"},
    {"PG", "Papua New Guinea"},
    {"PH", "Philippines"},
    {"PI", "Philippines"}, /* Indeterminately reserved code */
    {"PK", "Pakistan"},
    {"PL", "Poland"},
    {"PM", "Saint Pierre and Miquelon"},
    {"PN", "Pitcairn"},
    {"PR", "Puerto Rico"},
    {"PS", "Palestine, State of"},
    {"PT", "Portugal"},
    {"PU", "United States Miscellaneous Pacific Islands"}, /* Formerly assigned code */
    {"PW", "Palau"},
    {"PY", "Paraguay"},
    {"PZ", "Panama Canal Zone"}, /* Formerly assigned code */
    {"QA", "Qatar"},
    {"QM", "User-assigned code QM"}, /* User-assigned code */
    {"QN", "User-assigned code QN"}, /* User-assigned code */
    {"QO", "User-assigned code QO"}, /* User-assigned code */
    {"QP", "User-assigned code QP"}, /* User-assigned code */
    {"QQ", "User-assigned code QQ"}, /* User-assigned code */
    {"QR", "User-assigned code QR"}, /* User-assigned code */
    {"QS", "User-assigned code QS"}, /* User-assigned code */
    {"QT", "User-assigned code QT"}, /* User-assigned code */
    {"QU", "User-assigned code QU"}, /* User-assigned code */
    {"QV", "User-assigned code QV"}, /* User-assigned code */
    {"QW", "User-assigned code QW"}, /* User-assigned code */
    {"QX", "User-assigned code QX"}, /* User-assigned code */
    {"QY", "User-assigned code QY"}, /* User-assigned code */
    {"QZ", "User-assigned code QZ"}, /* User-assigned code */
    {"RA", "Argentina"}, /* Indeterminately reserved code */
    {"RB", "Bolivia or Botswana"}, /* Indeterminately reserved code */
    {"RC", "China"}, /* Indeterminately reserved code */
    {"RE", "Réunion"},
    {"RH", "Haiti"}, /* Indeterminately reserved code */
    {"RI", "Indonesia"}, /* Indeterminately reserved code */
    {"RL", "Lebanon"}, /* Indeterminately reserved code */
    {"RM", "Madagascar"}, /* Indeterminately reserved code */
    {"RN", "Niger"}, /* Indeterminately reserved code */
    {"RO", "Romania"},
    {"RP", "Philippines"}, /* Indeterminately reserved code */
    {"RS", "Serbia"},
    {"RU", "Russian Federation"},
    {"RW", "Rwanda"},
    {"SA", "Saudi Arabia"},
    {"SB", "Solomon Islands"},
    {"SC", "Seychelles"},
    {"SD", "Sudan"},
    {"SE", "Sweden"},
    {"SF", "Finland"}, /* Indeterminately reserved code */
    {"SG", "Singapore"},
    {"SH", "Saint Helena, Ascension and Tristan da Cunha"},
    {"SI", "Slovenia"},
    {"SJ", "Svalbard and Jan Mayen"},
    {"SK", "Slovakia"},
    {"SL", "Sierra Leone"},
    {"SM", "San Marino"},
    {"SN", "Senegal"},
    {"SO", "Somalia"},
    {"SR", "Suriname"},
    {"SS", "South Sudan"},
    {"ST", "Sao Tome and Principe"},
    {"SU", "USSR"}, /* Exceptionally reserved code */
    {"SV", "El Salvador"},
    {"SX", "Sint Maarten (Dutch part)"},
    {"SY", "Syrian Arab Republic"},
    {"SZ", "Eswatini"},
    {"TA", "Tristan da Cunha"}, /* Exceptionally reserved code */
    {"TC", "Turks and Caicos Islands"},
    {"TD", "Chad"},
    {"TF", "French Southern Territories"},
    {"TG", "Togo"},
    {"TH", "Thailand"},
    {"TJ", "Tajikistan"},
    {"TK", "Tokelau"},
    {"TL", "Timor-Leste"},
    {"TM", "Turkmenistan"},
    {"TN", "Tunisia"},
    {"TO", "Tonga"},
    {"TP", "Turkey"}, /* Transitionally reserved code */
    {"TR", "Turkey"},
    {"TT", "Trinidad and Tobago"},
    {"TV", "Tuvalu"},
    {"TW", "Taiwan"},
    {"TZ", "Tanzania, United Republic of"},
    {"UA", "Ukraine"},
    {"UG", "Uganda"},
    {"UK", "United Kingdom"}, /* Exceptionally reserved code */
    {"UM", "United States Minor Outlying Islands"},
    {"UN", "United Nations"}, /* Exceptionally reserved code */
    {"US", "United States of America"},
    {"UY", "Uruguay"},
    {"UZ", "Uzbekistan"},
    {"VA", "Holy See"},
    {"VC", "Saint Vincent and the Grenadines"},
    {"VD", "Democratic Republic of Viet-Nam"}, /* Formerly assigned code */
    {"VE", "Venezuela (Bolivarian Republic of)"},
    {"VG", "Virgin Islands (British)"},
    {"VI", "Virgin Islands (U.S.)"},
    {"VN", "Viet Nam"},
    {"VU", "Vanuatu"},
    {"WF", "Wallis and Futuna"},
    {"WG", "Grenada"}, /* Indeterminately reserved code */
    {"WK", "Wake Island"}, /* Formerly assigned code */
    {"WL", "Saint Lucia"}, /* Indeterminately reserved code */
/*  {"WO", "World Intellectual Property Organization"}, */ /* WIPO Indeterminately reserved code */
    {"WS", "Samoa"},
    {"WV", "Saint Vincent"}, /* Indeterminately reserved code */
    {"XA", "User-assigned code XA"}, /* User-assigned code */
    {"XB", "User-assigned code XB"}, /* User-assigned code */
    {"XC", "User-assigned code XC"}, /* User-assigned code */
    {"XD", "User-assigned code XD"}, /* User-assigned code */
    {"XE", "User-assigned code XE"}, /* User-assigned code */
    {"XF", "User-assigned code XF"}, /* User-assigned code */
    {"XG", "User-assigned code XG"}, /* User-assigned code */
    {"XH", "User-assigned code XH"}, /* User-assigned code */
    {"XI", "User-assigned code XI"}, /* User-assigned code */
    {"XJ", "User-assigned code XJ"}, /* User-assigned code */
    {"XK", "User-assigned code XK"}, /* User-assigned code */
    {"XL", "User-assigned code XL"}, /* User-assigned code */
    {"XM", "User-assigned code XM"}, /* User-assigned code */
    {"XN", "User-assigned code XN"}, /* User-assigned code */
    {"XO", "User-assigned code XO"}, /* User-assigned code */
    {"XP", "User-assigned code XP"}, /* User-assigned code */
    {"XR", "User-assigned code XR"}, /* User-assigned code */
    {"XS", "User-assigned code XS"}, /* User-assigned code */
    {"XT", "User-assigned code XT"}, /* User-assigned code */
    {"XU", "User-assigned code XU"}, /* User-assigned code */
    {"XV", "User-assigned code XV"}, /* User-assigned code */
    {"XW", "User-assigned code XW"}, /* User-assigned code */
    {"XX", "User-assigned code XX"}, /* User-assigned code */
    {"XY", "User-assigned code XY"}, /* User-assigned code */
    {"XZ", "User-assigned code XZ"}, /* User-assigned code */
    {"YD", "Democratic Yemen"}, /* Formerly assigned code */
    {"YE", "Yemen"},
    {"YT", "Mayotte"},
    {"YU", "Yugoslavia"}, /* Transitionally reserved code */
    {"YV", "Venezuela"}, /* Indeterminately reserved code */
    {"ZA", "South Africa"},
    {"ZM", "Zambia"},
    {"ZR", "Zaire"}, /* Transitionally reserved code */
    {"ZW", "Zimbabwe"},
    {"ZZ", "User-assigned code ZZ"}, /* User-assigned code */
    {NULL, NULL}, /* MUST BE LAST */
};


/*
 * globals
 */
static char *program = NULL;			/* our name */
static int verbosity_level = DBG_DEFAULT;	/* debug level set by -v */
static char *work_dir = NULL;			/* where the entry directory and tarball are formed */
static char *iocccsize = NULL;			/* path to the iocccsize tool */
static char *tar = "/usr/bin/tar";		/* path to tar executable that supports -cjvf */
static char *cp = "/bin/cp";			/* path to cp executable */
static char *ls = "/bin/ls";			/* path to ls executable */


/*
 * forward declarations
 */
static void usage(int exitcode, char const *name, char const *str);
static void dbg(int level, char const *fmt, ...);
static void warn(char const *name, char const *fmt, ...);
static void err(int exitcode, char const *name, char const *fmt, ...);
static void errp(int exitcode, char const *name, char const *fmt, ...);
static void free_author_array(struct author *authorp, int author_count);
static bool exists(char const *path);
static bool is_file(char const *path);
static bool is_exec(char const *path);
static bool is_dir(char const *path);
static bool is_write(char const *path);
static ssize_t readline(char **linep, FILE *stream);
static char *readline_dup(char **linep, bool strip, size_t *lenp, FILE *stream);
static void sanity_chk(char const *work_dir, char const *iocccsize, char const *tar);
static void para(char *line, ...);
static void fpara(FILE *stream, char *line, ...);
static char *prompt(char *str, size_t *lenp);
static char *get_contest_id(bool *testp);
static int get_entry_num(void);
static char *mk_entry_dir(char *work_dir, char *ioccc_id, int entry_num);
static char *lookup_location_name(char *upper_code);
static bool yes_or_no(char *question);
static int get_author_info(char *ioccc_id, int entry_num, struct author **author_set);


int
main(int argc, char *argv[])
{
    extern char *optarg;	/* option argument */
    extern int optind;		/* argv index of the next arg */
    bool test_mode = false;	/* true ==> contest ID is test */
    char *ioccc_id = NULL;	/* IOCCC contest ID */
    int entry_num = -1;		/* entry number or -1 ==> unset */
    char *entry_dir = NULL;	/* entry directory from which to form a compressed tarball */
    int author_count = -1;	/* number of authors */
    struct author *author_set = NULL;	/* authors array */
    int ret;			/* libc return code */
    int i;

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, "hv:Vt:c:l:")) != -1) {
	switch (i) {
	case 'h':	/* -h - print help to stderr and exit 0 */
	    usage(0, __FUNCTION__, "-h help mode:\n");
	    /*NOTREACHED*/
	case 'v':	/* -v verbosity */
	    /* parse verbosity */
	    errno = 0;
	    verbosity_level = strtol(optarg, NULL, 0);
	    if (errno != 0) {
		err(1, __FUNCTION__, "cannot parse -v arg: %s error: %s", optarg, strerror(errno));
		/*NOTREACHED*/
	    }
	    break;
	case 'V':	/* -V - print version and exit */
	    (void) printf("%s\n", VERSION);
	    exit(0); /*ooo*/
	    /*NOTREACHED*/
	    break;
	case 't':	/* -t tar */
	    tar = optarg;
	    break;
	case 'c':	/* -c cp */
	    tar = optarg;
	    break;
	case 'l':	/* -l ls */
	    tar = optarg;
	    break;
	default:
	    usage(2, __FUNCTION__, "invalid -flag");
	    /*NOTREACHED*/
	}
    }
    /* must have 2 args */
    switch (argc-optind) {
    case 2:
	break;
    default:
	usage(3, __FUNCTION__, "requires 2 arguments");
	/*NOTREACHED*/
	break;
    }
    /* collect required args */
    work_dir = argv[optind];
    dbg(DBG_LOW, "work_dir: %s", work_dir);
    iocccsize = argv[optind+1];
    dbg(DBG_LOW, "iocccsize: %s", iocccsize);
    dbg(DBG_LOW, "tar: %s", tar);
    dbg(DBG_LOW, "cp: %s", cp);
    dbg(DBG_LOW, "ls: %s", ls);

    /*
     * welcome
     */
    errno = 0;	/* pre-clear errno for errp() */
    ret = printf("Welcome to mkiocccentry version: %s\n", VERSION);
    if (ret < 0) {
	errp(4, __FUNCTION__, "printf error printing the welcome string");
	/*NOTREACHED*/
    }

    /*
     * environment sanity checks
     */
    para("", "Performing sanity checks on your environment ...", NULL);
    sanity_chk(work_dir, iocccsize, tar);
    para("... environment looks OK", "", NULL);

    /*
     * obtain the IOCCC contest ID
     */
    ioccc_id = get_contest_id(&test_mode);
    dbg(DBG_MED, "IOCCC contest ID: %s", ioccc_id);

    /*
     * obtain entry number
     */
    entry_num = get_entry_num();
    dbg(DBG_MED, "entry number: %d", entry_num);

    /*
     * create entry directory
     */
    entry_dir = mk_entry_dir(work_dir, ioccc_id, entry_num);
    dbg(DBG_LOW, "formed entry directory: %s", entry_dir);

    /*
     * obtain author information
     */
    author_count = get_author_info(ioccc_id, entry_num, &author_set);
    dbg(DBG_LOW, "collected information on %d authors", author_count);

    /*
     * free storage
     */
    if (ioccc_id != NULL) {
	free(ioccc_id);
	ioccc_id = NULL;
    }
    if (entry_dir != NULL) {
	free(entry_dir);
	entry_dir = NULL;
    }
    if (author_set != NULL) {
        free_author_array(author_set, author_count);
	free(author_set);
	author_set = NULL;
    }

    /*
     * All Done!!! - Jessica Noll, age 2
     */
    warn(__FUNCTION__, "XXX - code is NOT complete");	/* XXX - remove when code complete */
    exit(0); /*ooo*/
}


/*
 * usage - print usage to stderr
 *
 * Example:
 *	usage(3, __FUNCTION__, "missing required argument(s)");
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 */
void
usage(int exitcode, char const *name, char const *str)
{
    int ret;		/* libc return code */

    /*
     * firewall
     */
    if (name == NULL) {
	name = "((NULL name))";
	warn(__FUNCTION__, "\nin usage(): program was NULL, forcing it to be: %s\n", program);
    }
    if (str == NULL) {
	str = "((NULL str))";
	warn(__FUNCTION__, "\nin usage(): str was NULL, forcing it to be: %s\n", str);
    }
    if (program == NULL) {
	program = "((NULL program))";
	warn(__FUNCTION__, "\nin usage(): program was NULL, forcing it to be: %s\n", program);
    }
    if (tar == NULL) {
	tar = "((NULL tar))";
	warn(__FUNCTION__, "\nin usage(): tar was NULL, forcing it to be: %s\n", tar);
    }

    /*
     * print the formatted usage stream
     */
    ret = fprintf(stderr, "%s\n", str);
    if (ret < 0) {
	warn(__FUNCTION__, "\nin usage(): fprintf #0 returned error: %d\n", ret);
    }
    ret = fprintf(stderr, usage_msg, program, DBG_DEFAULT, tar, cp, ls);
    if (ret < 0) {
	warn(__FUNCTION__, "\nin usage(): fprintf #1 returned error: %d\n", ret);
    }
    ret = fprintf(stderr, usage_msg2, VERSION);
    if (ret < 0) {
	warn(__FUNCTION__, "\nin usage(): fprintf #1 returned error: %d\n", ret);
    }

    /*
     * exit
     */
    exit(exitcode);
    /*NOTREACHED*/
}


/*
 * dbg - print debug message if we are verbose enough
 *
 * given:
 * 	level	print message if >= verbosity level
 * 	fmt	printf format
 * 	...
 *
 * Example:
 *
 * 	dbg(1, "foobar information");
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 */
static void
dbg(int level, char const *fmt, ...)
{
    va_list ap;		/* argument pointer */
    int ret;		/* return code holder */

    /* start the var arg setup and fetch our first arg */
    va_start(ap, fmt);

    /* firewall */
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__FUNCTION__, "\nin dbg(%d, %s ...): NULL fmt, forcing use of: %d\n", level, fmt);
    }

    /*
     * print the debug message if allowed by the verbosity level
     */
    if (level <= verbosity_level) {
	ret = fprintf(stderr, "debug[%d]: ", level);
	if (ret < 0) {
	    warn(__FUNCTION__, "\nin dbg(%d, %s, %s ...): fprintf returned error: %d\n", level, fmt, ret);
	}
	ret = vfprintf(stderr, fmt, ap);
	if (ret < 0) {
	    warn(__FUNCTION__, "\nin dbg(%d, %s, %s ...): vfprintf returned error: %d\n", level, fmt, ret);
	}
	ret = fputc('\n', stderr);
	if (ret != '\n') {
	    warn(__FUNCTION__, "\nin dbg(%d, %s, %s ...): fputc returned error: %d\n", level, fmt, ret);
	}
	ret = fflush(stderr);
	if (ret < 0) {
	    warn(__FUNCTION__, "\nin dbg(%d, %s, %s ...): fflush returned error: %d\n", level, fmt, ret);
	}
    }

    /* clean up stdarg stuff */
    va_end(ap);
    return;
}


/*
 * warn - issue a warning message
 *
 * given:
 * 	name	name of function issuing the warning
 * 	fmt	format of the warning
 * 	...	optional format args
 *
 * Example:
 *
 * 	warn(__FUNCTION__, "unexpected foobar: %d", value);
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 */
static void
warn(char const *name, char const *fmt, ...)
{
    va_list ap;		/* argument pointer */
    int ret;		/* return code holder */

    /* start the var arg setup and fetch our first arg */
    va_start(ap, fmt);

    /*
     * NOTE: We cannot use warn because this is the warn function!
     */

    /* firewall */
    if (name == NULL) {
	name = "((NULL name))";
	(void) fprintf(stderr, "\nWarning: in warn(): called with NULL name, forcing name: %s\n", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	(void) fprintf(stderr, "\nWarning: in warn(): called with NULL fmt, forcing fmt: %s\n", fmt);
    }

    /* issue the warning */
    ret = fprintf(stderr, "Warning: %s: ", name);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in warn(%s, %s ...): fprintf returned error: %d\n", name, fmt, ret);
    }
    ret = vfprintf(stderr, fmt, ap);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in warn(%s, %s ...): vfprintf returned error: %d\n", name, fmt, ret);
    }
    ret = fputc('\n', stderr);
    if (ret != '\n') {
	(void) fprintf(stderr, "\nWarning: in warn(%s, %s ...): fputc returned error: %d\n", name, fmt, ret);
    }
    ret = fflush(stderr);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in warn(%s, %s ...): fflush returned error: %d\n", name, fmt, ret);
    }

    /* clean up stdarg stuff */
    va_end(ap);
    return;
}


/*
 * err - issue a fatal error message and exit
 *
 * given:
 * 	exitcode	value to exit with
 * 	name		name of function issuing the warning
 * 	fmt		format of the warning
 * 	...		optional format args
 *
 * This function does not return.
 *
 * Example:
 *
 * 	err(1, __FUNCTION__, "bad foobar: %s", message);
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 */
static void
err(int exitcode, char const *name, char const *fmt, ...)
{
    va_list ap;		/* argument pointer */
    int ret;		/* return code holder */

    /* start the var arg setup and fetch our first arg */
    va_start(ap, fmt);

    /* firewall */
    if (exitcode < 0) {
	warn(__FUNCTION__, "\nin err(): called with exitcode <0: %d\n", exitcode);
	exitcode = 255;
	warn(__FUNCTION__, "\nin err(): forcing exit code: %d\n", exitcode);
    }
    if (name == NULL) {
	name = "((NULL name))";
	warn(__FUNCTION__, "\nin err(): called with NULL name, forcing name: %s\n", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__FUNCTION__, "\nin err(): called with NULL fmt, forcing fmt: %s\n", fmt);
    }

    /* issue the FATAL error */
    ret = fprintf(stderr, "FATAL[%d]: %s: ", exitcode, name);
    if (ret < 0) {
	warn(__FUNCTION__, "\nin err(%d, %s, %s ...): fprintf returned error: %d\n", exitcode, name, fmt, ret);
    }
    ret = vfprintf(stderr, fmt, ap);
    if (ret < 0) {
	warn(__FUNCTION__, "\nin err(%d, %s, %s ...): vfprintf returned error: %d\n", exitcode, name, fmt, ret);
    }
    ret = fputc('\n', stderr);
    if (ret != '\n') {
	warn(__FUNCTION__, "\nin err(%d, %s, %s ...): fputc returned error: %d\n", exitcode, name, fmt, ret);
    }
    ret = fflush(stderr);
    if (ret < 0) {
	warn(__FUNCTION__, "\nin err(%d, %s, %s ...): fflush returned error: %d\n", exitcode, name, fmt, ret);
    }

    /* clean up stdarg stuff */
    va_end(ap);

    /* terminate */
    exit(exitcode);
}


/*
 * errp - issue a fatal error message with errno information and exit
 *
 * given:
 * 	exitcode	value to exit with
 * 	name		name of function issuing the warning
 * 	fmt		format of the warning
 * 	...		optional format args
 *
 * This function does not return.
 *
 * Example:
 *
 * 	errp(1, __FUNCTION__, "bad foobar: %s", message);
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 */
static void
errp(int exitcode, char const *name, char const *fmt, ...)
{
    va_list ap;		/* argument pointer */
    int ret;		/* return code holder */
    int saved_errno;	/* errno value when called */

    /* save errno in case we need it for strerror() */
    saved_errno = errno;

    /* start the var arg setup and fetch our first arg */
    va_start(ap, fmt);

    /* firewall */
    if (exitcode < 0) {
	warn(__FUNCTION__, "\nin err(): called with exitcode <0: %d\n", exitcode);
	exitcode = 255;
	warn(__FUNCTION__, "\nin err(): forcing exit code: %d\n", exitcode);
    }
    if (name == NULL) {
	name = "((NULL name))";
	warn(__FUNCTION__, "\nin err(): called with NULL name, forcing name: %s\n", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__FUNCTION__, "\nin err(): called with NULL fmt, forcing fmt: %s\n", fmt);
    }

    /* issue the FATAL error */
    ret = fprintf(stderr, "FATAL[%d]: %s: ", exitcode, name);
    if (ret < 0) {
	warn(__FUNCTION__, "\nin err(%d, %s, %s ...): fprintf #0 returned error: %d\n", exitcode, name, fmt, ret);
    }
    ret = vfprintf(stderr, fmt, ap);
    if (ret < 0) {
	warn(__FUNCTION__, "\nin err(%d, %s, %s ...): vfprintf returned error: %d\n", exitcode, name, fmt, ret);
    }
    ret = fprintf(stderr, " errno[%d]: %s", saved_errno, strerror(saved_errno));
    if (ret < 0) {
	warn(__FUNCTION__, "\nin err(%d, %s, %s ...): fprintf #1  returned error: %d\n", exitcode, name, fmt, ret);
    }
    ret = fputc('\n', stderr);
    if (ret != '\n') {
	warn(__FUNCTION__, "\nin err(%d, %s, %s ...): fputc returned error: %d\n", exitcode, name, fmt, ret);
    }
    ret = fflush(stderr);
    if (ret < 0) {
	warn(__FUNCTION__, "\nin err(%d, %s, %s ...): fflush returned error: %d\n", exitcode, name, fmt, ret);
    }

    /* clean up stdarg stuff */
    va_end(ap);

    /* terminate */
    exit(exitcode);
}


/*
 * free_author_array - free storage related to a struct author
 *
 * given:
 * 	author_set		- pointer to a struct author array
 * 	author_count	- length of author array
 */
static void
free_author_array(struct author *author_set, int author_count)
{
    int i;

    /*
     * firewall
     */
    if (author_set == NULL) {
	err(5, __FUNCTION__, "author_set is NULL");
	/*NOTREACHED*/
    }
    if (author_count < 0) {
	err(6, __FUNCTION__, "author_count: %d < 0", author_count);
	/*NOTREACHED*/
    }

    /*
     * free elements of each array member
     */
    for (i=0; i < author_count; ++i) {
	if (author_set[i].name != NULL) {
	    free(author_set[i].name);
	    author_set[i].name = NULL;
	}
	if (author_set[i].location_code != NULL) {
	    free(author_set[i].location_code);
	    author_set[i].location_code = NULL;
	}
	if (author_set[i].email != NULL) {
	    free(author_set[i].email);
	    author_set[i].email = NULL;
	}
	if (author_set[i].url != NULL) {
	    free(author_set[i].url);
	    author_set[i].url = NULL;
	}
	if (author_set[i].twitter != NULL) {
	    free(author_set[i].twitter);
	    author_set[i].twitter = NULL;
	}
	if (author_set[i].github_user != NULL) {
	    free(author_set[i].github_user);
	    author_set[i].github_user = NULL;
	}
	if (author_set[i].affiliation != NULL) {
	    free(author_set[i].affiliation);
	    author_set[i].affiliation = NULL;
	}
    }
    return;
}


/*
 * exists - if a path exists
 *
 * This function tests if a path exists.
 *
 * given:
 *
 *	path	- the path to test
 *
 * returns:
 *	true ==> path exists,
 *	false ==> path does not exist
 */
static bool
exists(char const *path)
{
    int ret;		/* return code holder */
    struct stat buf;	/* path status */

    /*
     * firewall
     */
    if (path == NULL) {
	err(7, __FUNCTION__, "exists called with NULL path");
	/*NOTREACHED*/
    }

    /*
     * test for existence of path
     */
    ret = stat(path, &buf);
    if (ret < 0) {
	dbg(DBG_HIGH, "path %s does not exist, stat returned: %d", path, ret);
	return false;
    }
    dbg(DBG_VHIGH, "path %s size: %ld", path, buf.st_size);
    return true;
}


/*
 * is_file - if a path is a file
 *
 * This function tests if a path exists and is a regular file.
 *
 * given:
 *
 *	path	- the path to test
 *
 * returns:
 *	true ==> path exists and is a regular file,
 *	false ==> path does not exist OR is not a regular file
 */
static bool
is_file(char const *path)
{
    int ret;		/* return code holder */
    struct stat buf;	/* path status */

    /*
     * firewall
     */
    if (path == NULL) {
	err(8, __FUNCTION__, "exists called with NULL path");
	/*NOTREACHED*/
    }

    /*
     * test for existence of path
     */
    ret = stat(path, &buf);
    if (ret < 0) {
	dbg(DBG_HIGH, "path %s does not exist, stat returned: %d", path, ret);
	return false;
    }
    dbg(DBG_VHIGH, "path %s size: %ld", path, buf.st_size);

    /*
     * test if path is a regular file
     */
    if (! S_ISREG(buf.st_mode)) {
	dbg(DBG_HIGH, "path %s is not a regular file");
	return false;
    }
    dbg(DBG_VHIGH, "path %s is a regular file", path);
    return true;
}


/*
 * is_exec - if a path is executable
 *
 * This function tests if a path exists and we have permissions to execute it.
 *
 * given:
 *
 *	path	- the path to test
 *
 * returns:
 *	true ==> path exists and we have executable access,
 *	false ==> path does not exist OR is not executable OR
 *		  we don't have permission to execute it
 */
static bool
is_exec(char const *path)
{
    int ret;		/* return code holder */
    struct stat buf;	/* path status */

    /*
     * firewall
     */
    if (path == NULL) {
	err(9, __FUNCTION__, "exists called with NULL path");
	/*NOTREACHED*/
    }

    /*
     * test for existence of path
     */
    ret = stat(path, &buf);
    if (ret < 0) {
	dbg(DBG_HIGH, "path %s does not exist, stat returned: %d", path, ret);
	return false;
    }
    dbg(DBG_VHIGH, "path %s size: %ld", path, buf.st_size);

    /*
     * test if we are allowed to execute it
     */
    ret = access(path, X_OK);
    if (ret < 0) {
	dbg(DBG_HIGH, "path %s is not executable", path);
	return false;
    }
    dbg(DBG_VHIGH, "path %s is executable", path);
    return true;
}


/*
 * is_dir - if a path is a directory
 *
 * This function tests if a path exists and is a directory.
 *
 * given:
 *
 *	path	- the path to test
 *
 * returns:
 *	true ==> path exists and is a directory,
 *	false ==> path does not exist OR is not a directory
 */
static bool
is_dir(char const *path)
{
    int ret;		/* return code holder */
    struct stat buf;	/* path status */

    /*
     * firewall
     */
    if (path == NULL) {
	err(10, __FUNCTION__, "exists called with NULL path");
	/*NOTREACHED*/
    }

    /*
     * test for existence of path
     */
    ret = stat(path, &buf);
    if (ret < 0) {
	dbg(DBG_HIGH, "path %s does not exist, stat returned: %d", path, ret);
	return false;
    }
    dbg(DBG_VHIGH, "path %s size: %ld", path, buf.st_size);

    /*
     * test if path is a regular directory
     */
    if (! S_ISDIR(buf.st_mode)) {
	dbg(DBG_HIGH, "path %s is not a directory");
	return false;
    }
    dbg(DBG_VHIGH, "path %s is a directory", path);
    return true;
}


/*
 * is_write - if a path is writable
 *
 * This function tests if a path exists and we have permissions to write it.
 *
 * given:
 *
 *	path	- the path to test
 *
 * returns:
 *	true ==> path exists and we have write access,
 *	false ==> path does not exist OR is not writable OR
 *		  we don't have permission to write it
 */
static bool
is_write(char const *path)
{
    int ret;		/* return code holder */
    struct stat buf;	/* path status */

    /*
     * firewall
     */
    if (path == NULL) {
	err(11, __FUNCTION__, "exists called with NULL path");
	/*NOTREACHED*/
    }

    /*
     * test for existence of path
     */
    ret = stat(path, &buf);
    if (ret < 0) {
	dbg(DBG_HIGH, "path %s does not exist, stat returned: %d", path, ret);
	return false;
    }
    dbg(DBG_VHIGH, "path %s size: %ld", path, buf.st_size);

    /*
     * test if we are allowed to execute it
     */
    ret = access(path, W_OK);
    if (ret < 0) {
	dbg(DBG_HIGH, "path %s is not writable", path);
	return false;
    }
    dbg(DBG_VHIGH, "path %s is writable", path);
    return true;
}


/*
 * readline - read a line from a stream
 *
 * Read a line from an open stream.  Malloc or realloc the line
 * buffer as needed.  Remove the trailing newline that was read.
 *
 * given:
 *	linep	- malloced line buffer (may be realloced) or ptr to NULL
 *		  NULL ==> getline() will malloc() the linep buffer
 *		  else ==> getline() might realloc() the linep buffer
 *	stream - file stream to read from
 *
 * returns:
 *	number of characters in the line with newline removed
 *
 * This function does not return on error.
 */
static ssize_t
readline(char **linep, FILE *stream)
{
    size_t linecap = 0;	/* allocated capacity of linep buffer */
    ssize_t ret;	/* getline return and our modified size return */
    char *p;		/* printer to NUL */

    /*
     * firewall
     */
    if (linep == NULL) {
	err(12, __FUNCTION__, "linep is NULL");
	/*NOTREACHED*/
    }
    if (stream == NULL) {
	err(13, __FUNCTION__, "stream is NULL");
	/*NOTREACHED*/
    }

    /*
     * read the line
     */
    clearerr(stream);
    errno = 0;	/* pre-clear errno for errp() */
    ret = getline(linep, &linecap, stream);
    if (ret < 0) {
	if (feof(stream)) {
	    errp(14, __FUNCTION__, "EOF found while reading line");
	    /*NOTREACHED*/
	} else {
	    errp(15, __FUNCTION__, "getline() error");
	    /*NOTREACHED*/
	}
    }
    /* paranoia */
    if (*linep == NULL) {
	err(16, __FUNCTION__, "*linep is NULL after getline()");
	/*NOTREACHED*/
    }

    /*
     * scan for embedded NUL bytes (before end of line)
     *
     * We could use memchr() but
     */
    errno = 0;	/* pre-clear errno for errp() */
    p = memchr(*linep, 0, ret);
    if (p != NULL) {
	errp(17, __FUNCTION__, "found NUL before end of line");
	/*NOTREACHED*/
    }

    /*
     * process trailing newline or lack there of
     */
    if ((*linep)[ret-1] != '\n') {
	err(18, __FUNCTION__, "line does not end in newline");
	/*NOTREACHED*/
    }
    (*linep)[ret-1] = '\0';	/* clear newline */
    --ret;
    dbg(DBG_VVHIGH, "read %d bytes + newline into %d byte buffer", ret, linecap);

    /*
     * return length of line without the trailing newline
     */
    return ret;
}


/*
 * readline_dup - read a line from a stream and duplicate to a malloced buffer.
 *
 * given:
 *	linep	- malloced line buffer (may be realloced) or ptr to NULL
 *		  NULL ==> getline() will malloc() the linep buffer
 *		  else ==> getline() might realloc() the linep buffer
 *	strip	- true ==> remove trailing whitespace,
 *		  false ==> only remove the trailing newline
 *	lenp	- != NULL ==> pointer to length of final length of line malloced,
 *		  NULL ==> do not return length of line
 *	stream - file stream to read from
 *
 * returns:
 *	malloced buffer containing the input without a trailing newline,
 *	and if strip == true, without trailing whitespace
 *
 * NOTE: This function will NOT return NULL.
 *
 * This function does not return on error.
 */
static char *
readline_dup(char **linep, bool strip, size_t *lenp, FILE *stream)
{
    ssize_t len;	/* getline return and our modified size return */
    char *ret;		/* malloced input */
    int i;

    /*
     * firewall
     */
    if (linep == NULL) {
	err(19, __FUNCTION__, "linep is NULL");
	/*NOTREACHED*/
    }
    if (stream == NULL) {
	err(20, __FUNCTION__, "stream is NULL");
	/*NOTREACHED*/
    }

    /*
     * read the line
     */
    len = readline(linep, stream);
    dbg(DBG_VVHIGH, "readline returned %d bytes", len);

    /*
     * duplicate the line
     */
    errno = 0;	/* pre-clear errno for errp() */
    ret = strdup(*linep);
    if (ret == NULL) {
	err(21, __FUNCTION__, "strdup of read line of %d bytes failed", ret);
	/*NOTREACHED*/
    }

    /*
     * strip trailing whitespace if requested
     */
    if (strip == true) {
	if (len > 0) {
	    for (i=len-1; i >= 0; --i) {
		if (isascii(ret[i]) && isspace(ret[i])) {
		    /* strip trailing ASCII whitespace */
		    --len;
		    ret[i] = '\0';
		}
	    }
	}
	dbg(DBG_VVHIGH, "readline, after trailing whitespace strip is %d bytes", len);
    }
    if (lenp != NULL) {
	*lenp = len;
    }

    /*
     * return the malloced string
     */
    return ret;
}


/*
 * sanity_chk - perform basic firewall sanity checks
 *
 * We perform basic sanity checks on paths and the IOCCC contest ID.
 *
 * given:
 *
 *	work_dir - where the entry directory and tarball are formed
 *	iocccsize - path to the iocccsize tool
 *	tar - path to tar that supports -cjvf
 *
 * NOTE: This function does not return on error or if things are not sane.
 */
static void
sanity_chk(char const *work_dir, char const *iocccsize, char const *tar)
{
    FILE *iocccsize_stream;	/* pipe from iocccsize -V */
    char *popen_cmd;		/* iocccsize -V */
    int popen_cmd_len;		/* length of iocccsize buffer */
    char *linep = NULL;		/* allocated line read from iocccsize */
    int exit_code;		/* exit code from system(iocccsize -V) */
    int major_ver;		/* iocccsize major version */
    int minor_ver;		/* iocccsize minor version */
    int year;			/* iocccsize release year */
    int month;			/* iocccsize release month */
    int day;			/* iocccsize release day */
    int ret;			/* libc function return value */

    /*
     * firewall
     */
    if (work_dir == NULL || iocccsize == NULL || tar == NULL) {
	err(22, __FUNCTION__, "called with NULL arg");
	/*NOTREACHED*/
    }

    /*
     * tar must be executable
     */
    if (! exists(tar)) {
	fpara(stderr,
	      "",
	      "We cannot find a tar program.",
	      "",
	      "A tar program that supports -cjvf is required to build an compressed tarball.",
	      "Perhaps you need to use:",
	      "",
	      "    mkiocccentry -t tar ...",
	      "",
	      "and/or install a tar program?  You can find the source for tar:",
	      "",
	      "    https://www.gnu.org/software/tar/",
	      "",
	      NULL);
	err(23, __FUNCTION__, "tar does not exist: %s", tar);
	/*NOTREACHED*/
    }
    if (! is_file(tar)) {
	fpara(stderr,
	      "",
	      "The tar, while it exists, is not a file.",
	      "",
	      "Perhaps you need to use another path:",
	      "",
	      "    mkiocccentry -t tar ...",
	      "",
	      "and/or install a tar program?  You can find the source for tar:",
	      "",
	      "    https://www.gnu.org/software/tar/",
	      "",
	      NULL);
	err(24, __FUNCTION__, "tar is not a file: %s", tar);
	/*NOTREACHED*/
    }
    if (! is_exec(tar)) {
	fpara(stderr,
	      "",
	      "The tar, while it is a file, is not executable.",
	      "",
	      "We suggest you check the permissions on the tar program, or use another path:",
	      "",
	      "    mkiocccentry -t tar ...",
	      "",
	      "and/or install a tar program?  You can find the source for tar:",
	      "",
	      "    https://www.gnu.org/software/tar/",
	      "",
	      NULL);
	err(25, __FUNCTION__, "tar is not executable program: %s", tar);
	/*NOTREACHED*/
    }

    /*
     * cp must be executable
     */
    if (! exists(cp)) {
	fpara(stderr,
	      "",
	      "We cannot find a cp program.",
	      "",
	      "A cp program is required to copy files into a directory under work_dir.",
	      "Perhaps you need to use:",
	      "",
	      "    mkiocccentry -c cp ...",
	      "",
	      "and/or install a cp program?  You can find the source for cp in core utilities:",
	      "",
	      "    https://www.gnu.org/software/coreutils/",
	      "",
	      NULL);
	err(26, __FUNCTION__, "cp does not exist: %s", cp);
	/*NOTREACHED*/
    }
    if (! is_file(cp)) {
	fpara(stderr,
	      "",
	      "The cp, while it exists, is not a file.",
	      "",
	      "Perhaps you need to use another path:",
	      "",
	      "    mkiocccentry -t cp ...",
	      "",
	      "and/or install a cp program?  You can find the source for cp in core utilities:",
	      "",
	      "    https://www.gnu.org/software/cp/",
	      "",
	      NULL);
	err(27, __FUNCTION__, "cp is not a file: %s", cp);
	/*NOTREACHED*/
    }
    if (! is_exec(cp)) {
	fpara(stderr,
	      "",
	      "The cp, while it is a file, is not executable.",
	      "",
	      "We suggest you check the permissions on the cp program, or use another path:",
	      "",
	      "    mkiocccentry -t cp ...",
	      "",
	      "and/or install a cp program?  You can find the source for cp in core utilities:",
	      "",
	      "    https://www.gnu.org/software/cp/",
	      "",
	      NULL);
	err(28, __FUNCTION__, "cp is not executable program: %s", cp);
	/*NOTREACHED*/
    }

    /*
     * ls must be executable
     */
    if (! exists(ls)) {
	fpara(stderr,
	      "",
	      "We cannot find a ls program.",
	      "",
	      "A ls program is required to copy files into a directory under work_dir.",
	      "Perhaps you need to use:",
	      "",
	      "    mkiocccentry -c ls ...",
	      "",
	      "and/or install a ls program?  You can find the source for ls in core utilities:",
	      "",
	      "    https://www.gnu.org/software/coreutils/",
	      "",
	      NULL);
	err(29, __FUNCTION__, "ls does not exist: %s", ls);
	/*NOTREACHED*/
    }
    if (! is_file(ls)) {
	fpara(stderr,
	      "",
	      "The ls, while it exists, is not a file.",
	      "",
	      "Perhaps you need to use another path:",
	      "",
	      "    mkiocccentry -t ls ...",
	      "",
	      "and/or install a ls program?  You can find the source for ls in core utilities:",
	      "",
	      "    https://www.gnu.org/software/ls/",
	      "",
	      NULL);
	err(30, __FUNCTION__, "ls is not a file: %s", ls);
	/*NOTREACHED*/
    }
    if (! is_exec(ls)) {
	fpara(stderr,
	      "",
	      "The ls, while it is a file, is not executable.",
	      "",
	      "We suggest you check the permissions on the ls program, or use another path:",
	      "",
	      "    mkiocccentry -t ls ...",
	      "",
	      "and/or install a ls program?  You can find the source for ls in core utilities:",
	      "",
	      "    https://www.gnu.org/software/ls/",
	      "",
	      NULL);
	err(31, __FUNCTION__, "ls is not executable program: %s", ls);
	/*NOTREACHED*/
    }

    /*
     * iocccsize (iocccsize) must be executable
     */
    if (! exists(iocccsize)) {
	fpara(stderr,
	      "",
	      "The iocccsize file does not exist.",
	      "",
	      "Perhaps you need to supply a different path?",
	      "",
	      NULL);
	err(32, __FUNCTION__, "iocccsize does not exist: %s", iocccsize);
	/*NOTREACHED*/
    }
    if (! is_file(iocccsize)) {
	fpara(stderr,
	      "",
	      "The iocccsize file, while it exits, is not a file.",
	      "",
	      "We suggest you check the permissions on the iocccsize.",
	      "",
	      NULL);
	err(33, __FUNCTION__, "iocccsize is not a file: %s", iocccsize);
	/*NOTREACHED*/
    }
    if (! is_exec(iocccsize)) {
	fpara(stderr,
	      "",
	      "The iocccsize file, while it is a file, is not executable.",
	      "",
	      "We suggest you check the permissions on the iocccsize.",
	      "",
	      NULL);
	err(34, __FUNCTION__, "iocccsize is not executable program: %s", iocccsize);
	/*NOTREACHED*/
    }

    /*
     * work_dir must be a writable directory
     */
    if (! exists(work_dir)) {
	fpara(stderr,
	      "",
	      "The work_dir does not exist.",
	      "",
	      "You should either create work_dir, or use a different work_dir directory path on the command line.",
	      "",
	      NULL);
	err(35, __FUNCTION__, "work_dir does not exist: %s", work_dir);
	/*NOTREACHED*/
    }
    if (! is_dir(work_dir)) {
	fpara(stderr,
	      "",
	      "While work_dir exists, it is not a directory.",
	      "",
	      "You should move or remove work_dir and them make a new work_dir directory, or use a different",
	      "work_dir directory path on the command line.",
	      "",
	      NULL);
	err(36, __FUNCTION__, "work_dir is not a directory: %s", work_dir);
	/*NOTREACHED*/
    }
    if (! is_write(work_dir)) {
	fpara(stderr,
	      "",
	      "While the directory work_dir exists, it is not a writable directory.",
	      "",
	      "You should change the permission to make work_dir writable, or you move or remove work_dir and then",
	      "create a new writable directory, or use a different work_dir directory path on the command line.",
	      "",
	      NULL);
	err(37, __FUNCTION__, "work_dir is not a writable directory: %s", work_dir);
	/*NOTREACHED*/
    }

    /*
     * be sure iocccsize version is OK
     *
     * The tool:
     *
     *    iocccsize -V
     *
     * will print the version string and exit.  The version string is of the form:
     *
     *    major.minor YYYY-MM-DD
     *
     * For this code to accept iocccsize, the major version must match REQUIRED_IOCCCSIZE_MAJVER
     * AND the minor version must be >= MIN_IOCCCSIZE_MINVER.
     */
    popen_cmd_len = strlen(iocccsize) + sizeof(" -V >/dev/null 2>&1") + 1;
    errno = 0;	/* pre-clear errno for errp() */
    popen_cmd = malloc(popen_cmd_len + 1);
    if (popen_cmd == NULL) {
	errp(38, __FUNCTION__, "malloc #0 failed");
	/*NOTREACHED*/
    }
    errno = 0;	/* pre-clear errno for errp() */
    ret = snprintf(popen_cmd, popen_cmd_len, "%s -V >/dev/null 2>&1", iocccsize);
    if (ret < 0) {
	errp(39, __FUNCTION__, "snprintf error: %d", ret);
	/*NOTREACHED*/
    }
    /* try running iocccsize -V to see if we can execute it */
    dbg(DBG_MED, "testing if %s supports -V", iocccsize);
    errno = 0;	/* pre-clear errno for errp() */
    exit_code = system(popen_cmd);
    if (exit_code < 0) {
	errp(40, __FUNCTION__, "error calling system(\"%s\")", popen_cmd);
	/*NOTREACHED*/
    } else if (exit_code == 127) {
	errp(41, __FUNCTION__, "execution of the shell failed for system(\"%s\")", popen_cmd);
	/*NOTREACHED*/
    } else if (exit_code == 2) {
	err(42, __FUNCTION__, "%s appears to be too old to support -V", iocccsize);
	/*NOTREACHED*/
    } else if (exit_code != 0) {
	err(43, __FUNCTION__, "%s failed with exit code: %d", popen_cmd, exit_code);
	/*NOTREACHED*/
    }

    /*
     * obtain version string from iocccsize -V
     */
    errno = 0;	/* pre-clear errno for errp() */
    ret = snprintf(popen_cmd, popen_cmd_len, "%s -V", iocccsize);
    if (ret < 0) {
	errp(44, __FUNCTION__, "snprintf error: %d", ret);
	/*NOTREACHED*/
    }
    /* pre-flush to avoid popen() buffered stdio issues */
    clearerr(stdout);	/* pre-clear ferror() status */
    errno = 0;	/* pre-clear errno for errp() */
    ret = fflush(stdout);
    if (ret < 0) {
	errp(45, __FUNCTION__, "fflush(stdout); error code: %d", ret);
	/*NOTREACHED*/
    }
    clearerr(stderr);	/* pre-clear ferror() status */
    errno = 0;	/* pre-clear errno for errp() */
    ret = fflush(stderr);
    if (ret < 0) {
	errp(46, __FUNCTION__, "fflush(stderr); error code: %d", ret);
	/*NOTREACHED*/
    }
    dbg(DBG_MED, "reading version string from %s -V", iocccsize);
    errno = 0;	/* pre-clear errno for errp() */
    iocccsize_stream = popen(popen_cmd, "r");
    if (iocccsize_stream == NULL) {
	errp(47, __FUNCTION__, "popen for reading failed for: %s", popen_cmd);
	/*NOTREACHED*/
    }
    /* read the 1st line - should contain the iocccsize version */
    (void) readline(&linep, iocccsize_stream);
    dbg(DBG_HIGH, "version line read: %s", linep);
    /* close down pipe */
    (void) fclose(iocccsize_stream);
    ret = sscanf(linep, "%d.%d %d-%d-%d", &major_ver, &minor_ver, &year, &month, &day);
    if (ret != 5) {
	err(48, __FUNCTION__, "iocccsize -V version string is mal-formed: %s", linep);
	/*NOTREACHED*/
    }
    dbg(DBG_MED, "iocccsize version: %d.%d", major_ver, minor_ver);
    dbg(DBG_HIGH, "iocccsize release year: %d month: %d day: %d", year, month, day);
    if (major_ver != REQUIRED_IOCCCSIZE_MAJVER) {
	err(49, __FUNCTION__, "iocccsize major version: %d != required major version: %d", major_ver, REQUIRED_IOCCCSIZE_MAJVER);
	/*NOTREACHED*/
    }
    if (major_ver != REQUIRED_IOCCCSIZE_MAJVER) {
	err(50, __FUNCTION__, "iocccsize major version: %d != required major version: %d", major_ver, REQUIRED_IOCCCSIZE_MAJVER);
	/*NOTREACHED*/
    }
    if (minor_ver < MIN_IOCCCSIZE_MINVER) {
	err(51, __FUNCTION__, "iocccsize minor version: %d < minimum minor version: %d", minor_ver, MIN_IOCCCSIZE_MINVER);
	/*NOTREACHED*/
    }
    dbg(DBG_LOW, "good iocccsize version: %s", linep);

    /*
     * free storage
     */
    if (linep != NULL) {
	free(linep);
	linep = NULL;
    }
    if (popen_cmd != NULL) {
	free(popen_cmd);
	popen_cmd = NULL;
    }
    return;
}


/*
 * para - print a paragraph of lines to stdout
 *
 * Print a collection of strings with newlines added after each string.
 * The final string pointer must be a NULL.
 *
 * Example:
 *	para("line 1", "line 2", "", "prev line 3 was an empty line", NULL);
 *
 * given:
 *	line	1st paragraph line to print
 *	...	strings as paragraph lines to print
 *	NULL	end of string list
 *
 * This function does not return on error.
 */
static void
para(char *line, ...)
{
    va_list ap;		/* stdarg block */
    int ret;		/* libc function return value */
    int fd;		/* stdout as a file descriptor or -1 */
    int line_cnt;	/* number of lines in the paragraph */

    /*
     * stdarg setup
     */
    va_start(ap, line);

    /*
     * stdout sanity check
     */
    if (stdout == NULL) {
	err(52, __FUNCTION__, "stdout is NULL");
	/*NOTREACHED*/
    }
    clearerr(stdout);	/* pre-clear ferror() status */
    errno = 0;	/* pre-clear errno for errp() */
    /* this may not always catch a bogus or un-opened stdout, but try anyway */
    fd = fileno(stdout);
    if (fd < 0) {
	errp(53, __FUNCTION__, "fileno on stdout returned: %d < 0", fd);
	/*NOTREACHED*/
    }
    clearerr(stdout);	/* paranoia */

    /*
     * print paragraph strings followed by newlines
     */
    line_cnt = 0;
    while (line != NULL) {

	/*
	 * print the string
	 */
	clearerr(stdout);	/* pre-clear ferror() status */
	errno = 0;	/* pre-clear errno for errp() */
	ret = fputs(line, stdout);
	if (ret == EOF) {
	    if (ferror(stdout)) {
		errp(54, __FUNCTION__, "error writing paragraph to a stdout");
		/*NOTREACHED*/
	    } else if (feof(stdout)) {
		errp(55, __FUNCTION__, "EOF while writing paragraph to a stdout");
		/*NOTREACHED*/
	    } else {
		errp(56, __FUNCTION__, "unexpected fputs error writing paragraph to a stdout");
		/*NOTREACHED*/
	    }
	}

	/*
	 * print the newline
	 */
	clearerr(stdout);	/* pre-clear ferror() status */
	errno = 0;	/* pre-clear errno for errp() */
	ret = fputc('\n', stdout);
	if (ret == EOF) {
	    if (ferror(stdout)) {
		errp(57, __FUNCTION__, "error writing newline to a stdout");
		/*NOTREACHED*/
	    } else if (feof(stdout)) {
		errp(58, __FUNCTION__, "EOF while writing newline to a stdout");
		/*NOTREACHED*/
	    } else {
		errp(59, __FUNCTION__, "unexpected fputc error newline a stdout");
		/*NOTREACHED*/
	    }
	}
	++line_cnt;	/* count this line as printed */

	/*
	 * move to next line string
	 */
	line = va_arg(ap, char *);
    }

    /*
     * stdarg cleanup
     */
    va_end(ap);

    /*
     * flush the paragraph onto the stdout
     */
    clearerr(stdout);	/* pre-clear ferror() status */
    errno = 0;	/* pre-clear errno for errp() */
    ret = fflush(stdout);
    if (ret == EOF) {
	if (ferror(stdout)) {
	    errp(60, __FUNCTION__, "error flushing stdout");
	    /*NOTREACHED*/
	} else if (feof(stdout)) {
	    errp(61, __FUNCTION__, "EOF while flushing stdout");
	    /*NOTREACHED*/
	} else {
	    errp(62, __FUNCTION__, "unexpected fflush error while flushing stdout");
	    /*NOTREACHED*/
	}
    }
    dbg(DBG_VVHIGH, "%s() printed %d line paragraph", __FUNCTION__, line_cnt);
    return;
}


/*
 * fpara - print a paragraph of lines to an open stream
 *
 * Print a collection of strings with newlines added after each string.
 * The final string pointer must be a NULL.
 *
 * Example:
 *	fpara(stderr, "line 1", "line 2", "", "prev line 3 was an empty line", NULL);
 *
 * given:
 *	stream	open file stream to print a paragraph onto
 *	line	1st paragraph line to print
 *	...	strings as paragraph lines to print
 *	NULL	end of string list
 *
 * This function does not return on error.
 */
static void
fpara(FILE *stream, char *line, ...)
{
    va_list ap;		/* stdarg block */
    int ret;		/* libc function return value */
    int fd;		/* stream as a file descriptor or -1 */
    int line_cnt;	/* number of lines in the paragraph */

    /*
     * stdarg setup
     */
    va_start(ap, line);

    /*
     * stream sanity check
     */
    if (stream == NULL) {
	err(63, __FUNCTION__, "stream is NULL");
	/*NOTREACHED*/
    }
    clearerr(stream);	/* pre-clear ferror() status */
    errno = 0;	/* pre-clear errno for errp() */
    /* this may not always catch a bogus or un-opened stream, but try anyway */
    fd = fileno(stream);
    if (fd < 0) {
	errp(64, __FUNCTION__, "fileno on stream returned: %d < 0", fd);
	/*NOTREACHED*/
    }
    clearerr(stream);	/* paranoia */

    /*
     * print paragraph strings followed by newlines
     */
    line_cnt = 0;
    while (line != NULL) {

	/*
	 * print the string
	 */
	clearerr(stream);	/* pre-clear ferror() status */
	errno = 0;	/* pre-clear errno for errp() */
	ret = fputs(line, stream);
	if (ret == EOF) {
	    if (ferror(stream)) {
		errp(65, __FUNCTION__, "error writing paragraph to a stream");
		/*NOTREACHED*/
	    } else if (feof(stream)) {
		errp(66, __FUNCTION__, "EOF while writing paragraph to a stream");
		/*NOTREACHED*/
	    } else {
		errp(67, __FUNCTION__, "unexpected fputs error writing paragraph to a stream");
		/*NOTREACHED*/
	    }
	}

	/*
	 * print the newline
	 */
	clearerr(stream);	/* pre-clear ferror() status */
	errno = 0;	/* pre-clear errno for errp() */
	ret = fputc('\n', stream);
	if (ret == EOF) {
	    if (ferror(stream)) {
		errp(68, __FUNCTION__, "error writing newline to a stream");
		/*NOTREACHED*/
	    } else if (feof(stream)) {
		errp(69, __FUNCTION__, "EOF while writing newline to a stream");
		/*NOTREACHED*/
	    } else {
		errp(70, __FUNCTION__, "unexpected fputc error newline a stream");
		/*NOTREACHED*/
	    }
	}
	++line_cnt;	/* count this line as printed */

	/*
	 * move to next line string
	 */
	line = va_arg(ap, char *);
    }

    /*
     * stdarg cleanup
     */
    va_end(ap);

    /*
     * flush the paragraph onto the stream
     */
    clearerr(stream);	/* pre-clear ferror() status */
    errno = 0;	/* pre-clear errno for errp() */
    ret = fflush(stream);
    if (ret == EOF) {
	if (ferror(stream)) {
	    errp(71, __FUNCTION__, "error flushing stream");
	    /*NOTREACHED*/
	} else if (feof(stream)) {
	    errp(72, __FUNCTION__, "EOF while flushing stream");
	    /*NOTREACHED*/
	} else {
	    errp(73, __FUNCTION__, "unexpected fflush error while flushing stream");
	    /*NOTREACHED*/
	}
    }
    dbg(DBG_VVHIGH, "%s() printed %d line paragraph", __FUNCTION__, line_cnt);
    return;
}


/*
 * prompt - prompt for a string
 *
 * Prompt a string, followed by :<space> on stdout and then read a line from stdin.
 * The line is stripped of the trailing newline and then of all trailing whitespace.
 *
 * given:
 *	str	- string to string followed by :<space>
 *	lenp	- pointer to where to put the length of the response,
 *		  NULL ==> do not save length
 *
 *
 * returns:
 *	malloced input string with newline and trailing whitespace removed
 *
 * NOTE: This function will NOT return NULL.
 *
 * This function does not return on error.
 */
static char *
prompt(char *str, size_t *lenp)
{
    char *linep = NULL;	/* readline_dup line buffer */
    int ret;		/* libc function return value */
    size_t len;		/* length of input */
    char *buf;		/* malloced input string */

    /*
     * firewall
     */
    if (str == NULL) {
	err(74, __FUNCTION__, "str is NULL");
	/*NOTREACHED*/
    }

    /*
     * prompt + :<space>
     */
    clearerr(stdout);	/* pre-clear ferror() status */
    errno = 0;	/* pre-clear errno for errp() */
    ret = fputs(str, stdout);
    if (ret == EOF) {
	if (ferror(stdout)) {
	    errp(75, __FUNCTION__, "error printing prompt string");
	    /*NOTREACHED*/
	} else if (feof(stdout)) {
	    errp(76, __FUNCTION__, "EOF while printing prompt string");
	    /*NOTREACHED*/
	} else {
	    errp(77, __FUNCTION__, "unexpected fputs error printing prompt string");
	    /*NOTREACHED*/
	}
    }
    clearerr(stdout);	/* pre-clear ferror() status */
    errno = 0;	/* pre-clear errno for errp() */
    ret = fputs(": ", stdout);
    if (ret == EOF) {
	if (ferror(stdout)) {
	    errp(78, __FUNCTION__, "error printing :<space>");
	    /*NOTREACHED*/
	} else if (feof(stdout)) {
	    errp(79, __FUNCTION__, "EOF while writing :<space>");
	    /*NOTREACHED*/
	} else {
	    errp(80, __FUNCTION__, "unexpected fputs error printing :<space>");
	    /*NOTREACHED*/
	}
    }
    clearerr(stdout);	/* pre-clear ferror() status */
    errno = 0;	/* pre-clear errno for errp() */
    ret = fflush(stdout);
    if (ret == EOF) {
	if (ferror(stdout)) {
	    errp(81, __FUNCTION__, "error flushing prompt to stdout");
	    /*NOTREACHED*/
	} else if (feof(stdout)) {
	    errp(82, __FUNCTION__, "EOF while flushing prompt to stdout");
	    /*NOTREACHED*/
	} else {
	    errp(83, __FUNCTION__, "unexpected fflush error while flushing prompt to stdout");
	    /*NOTREACHED*/
	}
    }

    /*
     * read user input - return input length
     */
    buf = readline_dup(&linep, true, &len, stdin);
    if (buf == NULL) {
	errp(84, __FUNCTION__, "readline_dup returned NULL");
	/*NOTREACHED*/
    }
    dbg(DBG_VHIGH, "received a %d byte response", len);

    /*
     * save length if requested
     */
    if (lenp != NULL) {
	*lenp = len;
    }

    /*
     * free storage
     */
    if (linep != NULL) {
	free(linep);
	linep = NULL;
    }

    /*
     * return malloced input buffer
     */
    return buf;
}



/*
 * get content ID or test
 *
 * This function will prompt the user for a contest ID, validate it and return it
 * as a malloced string.  If the contest ID is the special value "test", then
 * *testp will be set to true, otherwise it will be set to false.
 *
 * given:
 *	testp - pointer to boolean for test mode
 *
 * returns:
 *	malloced contest ID string
 *	*testp == true ==> contest ID is "test", else contest ID is a UUID.
 *
 * This function does not return on error or if the contest ID is malformed.
 */
static char *
get_contest_id(bool *testp)
{
    char *malloc_ret;		/* malloced return string */
    size_t len;			/* input string length */
    int ret;			/* libc function return */
    unsigned int a,b,c,d,e,f;	/* parts of the UUID string */
    unsigned int version = 0;	/* UUID version hex character */
    unsigned int variant = 0;	/* UUID variant hex character */
    int i;

    /*
     * firewall
     */
    if (testp == NULL) {
	err(85, __FUNCTION__, "testp is NULL");
	/*NOTREACHED*/
    }

    /*
     * explain contest ID
     */
    para("To submit entries to the IOCCC, you must a registered contestant and have received a",
	 "IOCCC contest ID (via email) shortly after you have been successfully registered.",
	 "If the IOCCC is open, you may register as a contestant. See:",
	 "",
	 "    file:///Users/chongo/bench/ioccc/ioccc-src/winner/index.html#enter",
	 "",
	 "If you do not have an IOCCC contest ID and you with to test this program,",
	 "you may use the special contest ID:",
	 "",
	 "    test",
	 "",
	 "Note you will not be able to submit the resulting compressed tarball when using test.",
	 "",
	 NULL);


    /*
     * keep asking for an entry number until we get a valid reply
     */
    do {

	/*
	 * prompt for the contest ID
	 */
	malloc_ret = prompt("Enter IOCCC contest ID or test", &len);
	dbg(DBG_HIGH, "the IOCCC contest ID as entered is: %s", malloc_ret);
	ret = 0; /* initialize paranoia */

	/*
	 * case: IOCCC contest ID is test, quick return
	 */
	if (strcmp(malloc_ret, "test") == 0) {

	    /*
	     * report test mode
	     */
	    para("",
		 "IOCCC contest ID is test, entering test mode.",
		 NULL);
	    *testp = true;
	    return malloc_ret;
	}

	/*
	 * validate format of non-test contest ID
	 *
	 * The contest ID, if not "test" must be a UUID.  The UUID has the 36 character format:
	 *
	 *		xxxxxxxx-xxxx-4xxx-axxx-xxxxxxxxxxxx
	 *
	 * where 'x' is a hex character.  The 4 is the UUID version and a the variant 1.
	 */
	if (len != UUID_LEN) {

	    /*
	     * reject improper input length
	     */
	    (void) fprintf(stderr, "\nIOCCC contest ID are %d characters in length, you entered %ld\n\n", UUID_LEN, len);
	    (void) fprintf(stderr, "IOCCC contest IDs are in the form:\n\n");
	    (void) fprintf(stderr, "    xxxxxxxx-xxxx-4xxx-axxx-xxxxxxxxxxxx\n\n");
	    (void) fprintf(stderr, "where 'x' is a hex character, 4 is the UUID version and a the variant 1.\n\n");

	    /*
	     * free storage
	     */
	    if (malloc_ret != NULL) {
		free(malloc_ret);
		malloc_ret = NULL;
	    }
	    continue;
	}
	/* convert to lower case */
	for (i=0; i < len; ++i) {
	    malloc_ret[i] = tolower(malloc_ret[i]);
	}
	dbg(DBG_VHIGH, "converted the IOCCC contest ID to: %s", malloc_ret);
	ret = sscanf(malloc_ret, "%8x-%4x-%1x%3x-%1x%3x-%8x%4x", &a, &b, &version, &c, &variant, &d, &e, &f);
	dbg(DBG_HIGH, "UUID version hex char: %1x", version);
	dbg(DBG_HIGH, "UUID variant hex char: %1x", variant);
	if (ret != 8) {
	    fpara(stderr,
		  "",
		  "IOCCC contest IDs are version 4, variant 1 UUID as defined by RFC4122:",
		  "",
		  "    https://datatracker.ietf.org/doc/html/rfc4122#section-4.1.1",
		  "",
		  "Your IOCCC contest ID is not a valid UUID.  Please check your the email you received",
		  "when you registered as an IOCCC contestant for the correct IOCCC contest ID.",
		  "",
		  NULL);
	}
    } while (ret != 8);
    dbg(DBG_MED, "IOCCC contest ID is a UUID: %s", malloc_ret);

    /*
     * report contest ID format is valid
     */
    para("",
	 "The format of the non-test IOCCC contest ID appears to be valid.",
	 "",
	 NULL);
    *testp = false;	/* IOCCC contest ID is not test */

    /*
     * return IOCCC contest ID
     */
    return malloc_ret;
}


/*
 * get_entry_num - obtain the entry number
 *
 * returns:
 *	entry number >= 0 <= MAX_ENTRY_NUM
 */
static int
get_entry_num(void)
{
    int entry_num;		/* entry number */
    char *entry_str;		/* entry number string */
    int ret;			/* libc function return */

    /*
     * keep asking for an entry number until we get a valid reply
     */
    do {

	/*
	 * explain entry numbers
	 */
	errno = 0;	/* pre-clear errno for errp() */
	ret = printf("\nYou are allowed to submit up to %d entries to a given IOCCC.\n", MAX_ENTRY_NUM+1);
	if (ret < 0) {
	    errp(86, __FUNCTION__, "printf error printing number of entries allowed");
	    /*NOTREACHED*/
	}
	para("",
	     "As in C, Entry numbers start with 0.  If you are updated a previous entry, PLEASE",
	     "use the same entry number that you previously uploaded so we know which entry we",
	     "should replace. If this is your 1st entry to this given IOCCC, enter 0.",
	     "",
	     NULL);

	/*
	 * ask for the entry number
	 */
	entry_str = prompt("Enter the entry number", NULL);

	/*
	 * check the entry number
	 */
	ret = sscanf(entry_str, "%d", &entry_num);
	if (ret != 1 || entry_num < 0 || entry_num > MAX_ENTRY_NUM) {
	    (void) fprintf(stderr, "\nThe entry number must be a number from 0 thru %d, please re-enter.\n", MAX_ENTRY_NUM);
	}

	/*
	 * free storage
	 */
	if (entry_str != NULL) {
	    free(entry_str);
	    entry_str = NULL;
	}

    } while (entry_num < 0 || entry_num > MAX_ENTRY_NUM);

    /*
     * return the entry number
     */
    return entry_num;
}


/*
 * mk_entry_dir - make the entry directory
 *
 * Make a directory, under work_dir, from which the compressed tarball
 * will be formed.
 *
 * given:
 *	work_dir - working directory under which the entry directory is formed
 *	ioccc_id - IOCCC entry ID (or test)
 *	entry_num - entry number
 *
 * returns:
 *	the path of the working directory
 *
 * This function does not return on error or if the entry directory cannot be formed.
 */
static char *
mk_entry_dir(char *work_dir, char *ioccc_id, int entry_num)
{
    int len;			/* length of entry directory */
    char *entry_dir = NULL;	/* malloced entry directory path */
    int ret;			/* libc function return */

    /*
     * firewall
     */
    if (work_dir == NULL || ioccc_id == NULL) {
	err(87, __FUNCTION__, "work_dir and/or ioccc_id is NULL");
	/*NOTREACHED*/
    }
    if (entry_num < 0 || entry_num > MAX_ENTRY_NUM) {
	err(88, __FUNCTION__, "entry number: %d must >= 0 and <= %d", MAX_ENTRY_NUM);
	/*NOTREACHED*/
    }

    /*
     * determine length of entry directory path
     */
    /* work_dir/ioccc_id-entry */
    len = strlen(work_dir) + 1 + strlen(ioccc_id) + 1 + MAX_ENTRY_CHARS + 1;
    errno = 0;	/* pre-clear errno for errp() */
    entry_dir = malloc(len + 1 + 1);
    if (entry_dir == NULL) {
	errp(89, __FUNCTION__, "cannot malloc %d characters", len + 1);
	/*NOTREACHED*/
    }
    errno = 0;	/* pre-clear errno for errp() */
    ret = snprintf(entry_dir, len + 1, "%s/%s-%d", work_dir, ioccc_id, entry_num);
    if (ret < 0) {
	errp(90, __FUNCTION__, "snprintf to form entry directory failed");
	/*NOTREACHED*/
    }
    dbg(DBG_HIGH, "entry directory path: %s", entry_dir);

    /*
     * verify that the entry directory does not exist
     */
    if (exists(entry_dir)) {
	(void) fprintf(stderr, "\nentry directory already exists: %s\n", entry_dir);
	fpara(stderr,
	      "",
	      "You need to move that directory, or remove it, or use a different work_dir.",
	      "",
	      NULL);
	err(91, __FUNCTION__, "entry directory exists: %s", entry_dir);
	/*NOTREACHED*/
    }
    dbg(DBG_HIGH, "entry directory path: %s", entry_dir);

    /*
     * make the entry directory
     */
    errno = 0;	/* pre-clear errno for errp() */
    ret = mkdir(entry_dir, 0755);
    if (ret < 0) {
	errp(92, __FUNCTION__, "cannot mkdir %s with mode 0755", entry_dir);
	/*NOTREACHED*/
    }

    /*
     * return entry directory
     */
    return entry_dir;
}


/*
 * lookup_location_name - convert a ISO 3166-1 Alpha-2 into a location name
 *
 * given:
 *	upper_code	- ISO 3166-1 Alpha-2 in UPPER CASE
 *
 * return:
 *	location name or NULL ==> unlisted code
 *
 * This function does not return on error.
 */
static char *
lookup_location_name(char *upper_code)
{
    struct location *p;		/* entry in the location table */

    /*
     * firewall
     */
    if (upper_code == NULL) {
	err(93, __FUNCTION__, "upper_code is NULL");
	/*NOTREACHED*/
    }

    /*
     * search location table for the code
     */
    for (p=&loc[0]; p->code != NULL && p->name != NULL; ++p) {
	if (strcmp(upper_code, p->code) == 0) {
	    dbg(DBG_VHIGH, "code %s name found: %s", p->name);
	    break;
	}
    }

    /*
     * return name or NULL
     */
    return p->name;
}


/*
 * yes_or_no - determine if input is yes or no
 *
 * given:
 *	question	string to prompt for a question
 *
 * returns:
 *	true ==> input is yes in some form,
 *	false ==> input is not yes
 */
static bool
yes_or_no(char *question)
{
    char *response;	/* response to the question */
    char *p;

    /*
     * firewall
     */
    if (question == NULL) {
	err(94, __FUNCTION__, "question is NULL");
	/*NOTREACHED*/
    }

    /*
     * ask the question and obtain the response
     */
    do{
	response = prompt(question, NULL);

	/*
	 * convert response to lower case
	 */
	for (p=response; *p != '\0'; ++p) {
	    if (isascii(*p) && isalpha(*p)) {
		*p = tolower(*p);
	    }
	}

	/*
	 * check for a valid reply
	 */
	if (strcmp(response, "y") == 0 || strcmp(response, "yes") == 0) {

	    /*
	     * free storage
	     */
	    if (response != NULL) {
		free(response);
		response = NULL;
	    }

	    /*
	     * return yes
	     */
	    return true;

	} else if (strcmp(response, "n") == 0 || strcmp(response, "no") == 0) {

	    /*
	     * free storage
	     */
	    if (response != NULL) {
		free(response);
		response = NULL;
	    }

	    /*
	     * return no
	     */
	    return false;
	}

	/*
	 * reject response and ask again
	 */
	(void) fprintf(stderr, "Please enter either y (yes) or n (no)\n");

	/*
	 * free storage
	 */
	if (response != NULL) {
	    free(response);
	    response = NULL;
	}

    } while (response == NULL);
    /*NOTREACHED*/

    /*
     * free storage
     */
    if (response != NULL) {
	free(response);
	response = NULL;
    }

    /*
     * should not get here - but assume no if we do
     */
    return false;
}


/*
 * get_author_info - obtain information on entry authors
 *
 * given:
 *	ioccc_id	IOCCC entry ID or test
 *	entry_num	entry number
 *	author_set	pointer to array of authors
 *
 * returns:
 *	number of authors
 *
 * This function does not return on error.
 */
static int
get_author_info(char *ioccc_id, int entry_num, struct author **author_set_p)
{
    struct author *author_set = NULL;	/* allocated author set */
    int author_count = -1;		/* number of authors or -1 */
    char *author_count_str = NULL;	/* author count string */
    char *location_name = NULL;		/* location name of a given location/country code */
    bool yorn = false;			/* response to a question */
    size_t len;				/* length of reply */
    int ret;				/* libc function return */
    char *p;
    int i;

    /*
     * firewall
     */
    if (ioccc_id == NULL || author_set_p == NULL) {
	err(95, __FUNCTION__, "ioccc_id and/or author_set_p is NULL");
	/*NOTREACHED*/
    }

    /*
     * keep asking for an entry number until we get a valid reply
     */
    do {

        /*
	 * ask for author count
	 */
	author_count_str = prompt("\nEnter the number of authors of this entry", NULL);

	/*
	 * convert author_count_str to number
	 */
	ret = sscanf(author_count_str, "%d", &author_count);
	if (ret != 1 || author_count < 0 || author_count > MAX_AUTHORS) {
	    (void) fprintf(stderr, "\nThe number of authors must a number from 1 thru %d, please re-enter.\n", MAX_AUTHORS);
	    (void) fprintf(stderr, "If you happen to have more than %d authors. we ask that you pick the\n", MAX_AUTHORS);
	    (void) fprintf(stderr, "just %d authors and mention the remainder of the authors in the remarks file.\n", MAX_AUTHORS);
	}

	/*
	 * free storage
	 */
	if (author_count_str != NULL) {
	    free(author_count_str);
	    author_count_str = NULL;
	}

    } while (author_count < 1 || author_count > MAX_AUTHORS);
    dbg(DBG_HIGH, "will request information on %d authors", author_count);

    /*
     * allocate the author array
     */
    errno = 0;	/* pre-clear errno for errp() */
    author_set = (struct author *)malloc(sizeof(struct author) * author_count);
    if (author_set == NULL) {
	errp(96, __FUNCTION__, "unable to malloc a struct author array of length: %d", author_count);
	/*NOTREACHED*/
    }
    /* pre-zeroize the author array */
    memset(author_set, 0, sizeof(struct author) * author_count);

    /*
     * inform the user about the author information we need to collect
     */
    para("",
	 "We will now ask for information about the author(s) of this entry.",
	 "",
	 "Information that you supply, if your entry is selected as a winner,",
	 "will be published with your entry.",
	 "",
	 "Except for your name and location/country code, you can opt out of providing it,",
	 "(or if you don't have the thing we are asking for), by just pressing return.",
	 "",
	 "A name is required. If an author wishes to be anonymous, use a pseudo-name.",
	 "Keep in mind that if an author wins multiple years, or has won before, you might",
	 "want to be consistent and provide the same name or pseudo-name each time.",
	 "",
	 "We will ask for the location/country as a 2 character ISO 3166-1 Alpha-2 code.",
	 "",
	 "    See the following URLs for information on ISO 3166-1 Alpha-2 codes:",
	 NULL);
    ret = puts(ISO_3166_1_CODE_URL0);
    if (ret < 0) {
	errp(97, __FUNCTION__, "puts error printing ISO 3166-1 URL");
	/*NOTREACHED*/
    }
    ret = puts(ISO_3166_1_CODE_URL1);
    if (ret < 0) {
	errp(98, __FUNCTION__, "puts error printing ISO 3166-1 URL");
	/*NOTREACHED*/
    }
    ret = puts(ISO_3166_1_CODE_URL2);
    if (ret < 0) {
	errp(99, __FUNCTION__, "puts error printing ISO 3166-1 URL2");
	/*NOTREACHED*/
    }
    ret = puts(ISO_3166_1_CODE_URL3);
    if (ret < 0) {
	errp(100, __FUNCTION__, "puts error printing ISO 3166-1 URL2");
	/*NOTREACHED*/
    }
    para("",
	 "We will ask for the author(s) Email address. Press return if you don't want to provide it, or if don't have one.",
	 "",
	 "We will ask for a home URL (starting with http:// or https://), or press return to skip, or if don't have one.",
	 "",
	 "We will ask a twitter handle (must start with @), or press return to skip, or if don't have one.",
	 "",
	 "We will ask a GitHub account (must start with @), or press return to skip, or if don't have one.",
	 "",
	 "We will ask for an affiliation (company, school, org) of the author, or press return to skip, or if don't have one.",
	 NULL);

    /*
     * collect information on authors
     */
    for (i=0; i < author_count; ++i) {

	/*
	 * announce author number
	 */
	errno = 0;	/* pre-clear errno for errp() */
	ret = printf("\nEnter information for author #%d\n\n", i);
	if (ret < 0) {
	    errp(101, __FUNCTION__, "printf error printing author number");
	    /*NOTREACHED*/
	}

	/*
	 * obtain author name
	 */
	do {

	    /*
	     * prompt for the author name
	     */
	    author_set[i].name = NULL;
	    author_set[i].name = prompt("Enter author name", &len);

	    /*
	     * reject empty author name
	     */
	    if (len <= 0) {

		/*
		 * issue rejection message
		 */
		fpara(stderr,
		     "",
		     "The author name cannot be empty, try again.  If they want to be anonymous, give a pseudo-name.",
		     "",
		     NULL);

		/*
		 * free storage
		 */
		if (author_set[i].name != NULL) {
		    free(author_set[i].name);
		    author_set[i].name = NULL;
		}

	    /*
	     * reject if name is too long
	     */
	    } else if (len > MAX_NAME_LEN) {

		/*
		 * issue rejection message
		 */
		(void) fprintf(stderr, "\nSorry ( tm Canada :-) ), we limit names to %d characters\n\n", MAX_NAME_LEN);

		/*
		 * free storage
		 */
		if (author_set[i].name != NULL) {
		    free(author_set[i].name);
		    author_set[i].name = NULL;
		}
	    }
	} while (author_set[i].name == NULL);

	/*
	 * obtain author location/country code
	 */
	do {

	    /*
	     * request location/country code
	     */
	    author_set[i].location_code = NULL;
	    author_set[i].location_code = prompt("Enter author 2 character location/country code", &len);
	    dbg(DBG_VHIGH, "location/country code as entered: %s", author_set[i].location_code);

	    /*
	     * inspect code input
	     */
	    if (len != 2 ||
	        ! isascii(author_set[i].location_code[0]) || ! isalpha(author_set[i].location_code[0]) ||
	        ! isascii(author_set[i].location_code[1]) || ! isalpha(author_set[i].location_code[1])) {

		/*
		 * provide more help on location/country codes
		 */
		(void) fprintf(stderr, "\nLocation/country codes are two letters\n\n");
		(void) fprintf(stderr, "For ISO 3166-1 2 character codes, see: the Alpha-2 code column of:\n");
		(void) fprintf(stderr, "\n%s\n\n", ISO_3166_1_CODE_URL0);
		(void) fprintf(stderr, "or from these Wikipedia / ISO web pages:\n");
		(void) fprintf(stderr, "\n%s\n", ISO_3166_1_CODE_URL1);
		(void) fprintf(stderr, "%s\n", ISO_3166_1_CODE_URL2);
		(void) fprintf(stderr, "%s\n\n", ISO_3166_1_CODE_URL3);

		/*
		 * free storage
		 */
		if (author_set[i].location_code != NULL) {
		    free(author_set[i].location_code);
		    author_set[i].location_code = NULL;
		}

		/*
		 * discard this invalid location/country code input
		 */
		location_name = NULL;
		yorn = false;
		continue;

	    } else {

		/*
		 * valid location/country code - convert to upper case
		 */
		author_set[i].location_code[0] = toupper(author_set[i].location_code[0]);
		author_set[i].location_code[1] = toupper(author_set[i].location_code[1]);

		/*
		 * determine if code is known
		 */
		location_name = lookup_location_name(author_set[i].location_code);
		if (location_name == NULL) {

		    /*
		     * provide more help on location/country codes
		     */
		    (void) fprintf(stderr, "\nThat is not a known location/country code.\n\n");
		    (void) fprintf(stderr, "For ISO 3166-1 2 character codes, see: the Alpha-2 code column of:\n");
		    (void) fprintf(stderr, "\n%s\n\n", ISO_3166_1_CODE_URL0);
		    (void) fprintf(stderr, "or from these Wikipedia / ISO web pages:\n");
		    (void) fprintf(stderr, "\n%s\n", ISO_3166_1_CODE_URL1);
		    (void) fprintf(stderr, "%s\n", ISO_3166_1_CODE_URL2);
		    (void) fprintf(stderr, "%s\n\n", ISO_3166_1_CODE_URL3);

		    /*
		     * free storage
		     */
		    if (author_set[i].location_code != NULL) {
			free(author_set[i].location_code);
			author_set[i].location_code = NULL;
		    }

		    /*
		     * discard this invalid location/country code input
		     */
		    location_name = NULL;
		    yorn = false;
		    continue;
		}

		/*
		 * verify the known location/country code
		 */
		ret = printf("The location/country code you entered is assigned to: %s\n", location_name);
		yorn = yes_or_no("Is that location/country code correct? [yn]");

		/*
		 * free storage if no (reenter location/country code)
		 */
		if (yorn == false) {
		    if (author_set[i].location_code != NULL) {
			free(author_set[i].location_code);
			author_set[i].location_code = NULL;
		    }
		}
	    }
	} while (author_set[i].location_code == NULL || location_name == NULL || yorn == false);
	dbg(DBG_HIGH, "author location/country: %s (%s)", author_set[i].location_code, location_name);

	/*
	 * ask for Email address
	 */
	do {

	    /*
	     * request Email address
	     */
	    author_set[i].email = NULL;
	    author_set[i].email = prompt("Enter author email address, or press return to skip", &len);
	    if (len == 0) {
		dbg(DBG_VHIGH, "Email address withheld");
	    } else {
		dbg(DBG_VHIGH, "Email address: %s", author_set[i].email);
	    }

	    /*
	     * reject if too long
	     */
	    if (len > MAX_EMAIL_LEN) {

		/*
		 * issue rejection message
		 */
		(void) fprintf(stderr, "\nSorry ( tm Canada :-) ), we limit Email address to %d characters\n\n", MAX_EMAIL_LEN);

		/*
		 * free storage
		 */
		if (author_set[i].email != NULL) {
		    free(author_set[i].email);
		    author_set[i].email = NULL;
		}
		continue;
	    }

	    /*
	     * reject if no @ in the address
	     */
	    if (len > 0) {
		p = strchr(author_set[i].email, '@');
		if (p == NULL || author_set[i].email[0] == '@' || author_set[i].email[len-1] == '@' ||
		    p != strrchr(author_set[i].email, '@')) {

		    /*
		     * issue rejection message
		     */
		    fpara(stderr,
			  "",
			  "Email addresses must have only a single @ somewhere inside the string."
			  "",
			  "",
			  NULL);

		    /*
		     * free storage
		     */
		    if (author_set[i].email != NULL) {
			free(author_set[i].email);
			author_set[i].email = NULL;
		    }
		    continue;
		}

	    /*
	     * just in case we have a bogus length
	     */
	    } else if (len < 0) {
		errp(102, __FUNCTION__, "Bogus Email length: %d < 0", len);
		/*NOTREACHED*/
	    }
	} while (author_set[i].email == NULL);

	/*
	 * ask for home URL
	 */
	do {

	    /*
	     * request URL
	     */
	    author_set[i].url = NULL;
	    author_set[i].url = prompt("Enter author home page URL (starting with http:// or https://), or press return to skip", &len);
	    if (len == 0) {
		dbg(DBG_VHIGH, "URL withheld");
	    } else {
		dbg(DBG_VHIGH, "URL: %s", author_set[i].url);
	    }

	    /*
	     * reject if too long
	     */
	    if (len > MAX_URL_LEN) {

		/*
		 * issue rejection message
		 */
		(void) fprintf(stderr, "\nSorry ( tm Canada :-) ), we URLs to %d characters\n\n", MAX_URL_LEN);

		/*
		 * free storage
		 */
		if (author_set[i].url != NULL) {
		    free(author_set[i].url);
		    author_set[i].url = NULL;
		}
		continue;
	    }

	    /*
	     * if it starts with http:// or https:// and has more characters, it is OK
	     */
	    if (len > 0) {
		if (((strncmp(author_set[i].url, "http://", sizeof("http://")-1) == 0) &&
		     (author_set[i].url[sizeof("http://")-1] != '\0')) ||
		    ((strncmp(author_set[i].url, "https://", sizeof("https://")-1) == 0) &&
		     (author_set[i].url[sizeof("https://")-1] != '\0'))) {

		    /* URL appears to in valid form */
		    break;

	    /*
	     * reject if it does not start with http:// or https://
	     */
	    } else if (len > 0) {

		    /*
		     * issue rejection message
		     */
		    fpara(stderr,
			  "",
			  "url addresses must begin with http:// or https:// followed by the rest of the homepage URL",
			  "",
			  NULL);

		    /*
		     * free storage
		     */
		    if (author_set[i].url != NULL) {
			free(author_set[i].url);
			author_set[i].url = NULL;
		    }
		    continue;
		}

	    /*
	     * just in case we have a bogus length
	     */
	    } else if (len < 0) {
		errp(103, __FUNCTION__, "Bogus url length: %d < 0", len);
		/*NOTREACHED*/
	    }
	} while (author_set[i].url == NULL);

	/*
	 * ask for twitter handle
	 */
	do {

	    /*
	     * request twitter handle
	     */
	    author_set[i].twitter = NULL;
	    author_set[i].twitter = prompt("Enter author twitter handle, starting with @, or press return to skip", &len);
	    if (len == 0) {
		dbg(DBG_VHIGH, "Twitter handle not given");
	    } else {
		dbg(DBG_VHIGH, "Twitter handle: %s", author_set[i].twitter);
	    }

	    /*
	     * reject if too long
	     */
	    if (len > MAX_TWITTER_LEN) {

		/*
		 * issue rejection message
		 */
		(void) fprintf(stderr, "\nSorry ( tm Canada :-) ), we limit twitter handles, starting with the @, to %d characters\n\n", MAX_TWITTER_LEN);

		/*
		 * free storage
		 */
		if (author_set[i].twitter != NULL) {
		    free(author_set[i].twitter);
		    author_set[i].twitter = NULL;
		}
		continue;
	    }

	    /*
	     * reject if no leading @, or if more than one @
	     */
	    if (len > 0) {
		p = strchr(author_set[i].twitter, '@');
		if (p == NULL || author_set[i].twitter[0] != '@' || p != strrchr(author_set[i].twitter, '@') ||
		    author_set[i].twitter[1] == '\0') {

		    /*
		     * issue rejection message
		     */
		    fpara(stderr,
			  "",
			  "Twitter handles must start with a @ and have no other @-signs."
			  "",
			  "",
			  NULL);

		    /*
		     * free storage
		     */
		    if (author_set[i].twitter != NULL) {
			free(author_set[i].twitter);
			author_set[i].twitter = NULL;
		    }
		    continue;
		}

	    /*
	     * just in case we have a bogus length
	     */
	    } else if (len < 0) {
		errp(104, __FUNCTION__, "Bogus twitter handle length: %d < 0", len);
		/*NOTREACHED*/
	    }
	} while (author_set[i].twitter == NULL);

	/*
	 * ask for GitHub account
	 */
	do {

	    /*
	     * request GitHub account
	     */
	    author_set[i].github_user = NULL;
	    author_set[i].github_user = prompt("Enter author GitHub account, starting with @, or press return to skip", &len);
	    if (len == 0) {
		dbg(DBG_VHIGH, "GitHub account not given");
	    } else {
		dbg(DBG_VHIGH, "GitHub account: %s", author_set[i].github_user);
	    }

	    /*
	     * reject if too long
	     */
	    if (len > MAX_GITHUB_LEN) {

		/*
		 * issue rejection message
		 */
		(void) fprintf(stderr, "\nSorry ( tm Canada :-) ), we limit GitHub account names, starting with the @, to %d characters\n\n", MAX_GITHUB_LEN);

		/*
		 * free storage
		 */
		if (author_set[i].github_user != NULL) {
		    free(author_set[i].github_user);
		    author_set[i].github_user = NULL;
		}
		continue;
	    }

	    /*
	     * reject if no leading @, or if more than one @
	     */
	    if (len > 0) {
		p = strchr(author_set[i].github_user, '@');
		if (p == NULL || author_set[i].github_user[0] != '@' || p != strrchr(author_set[i].github_user, '@') || author_set[i].github_user[1] == '\0') {

		    /*
		     * issue rejection message
		     */
		    fpara(stderr,
			  "",
			  "GitHuv accounts must start with a @ and have no other @-signs."
			  "",
			  "",
			  NULL);

		    /*
		     * free storage
		     */
		    if (author_set[i].github_user != NULL) {
			free(author_set[i].github_user);
			author_set[i].github_user = NULL;
		    }
		    continue;
		}

	    /*
	     * just in case we have a bogus length
	     */
	    } else if (len < 0) {
		errp(105, __FUNCTION__, "Bogus GitHub account length: %d < 0", len);
		/*NOTREACHED*/
	    }
	} while (author_set[i].github_user == NULL);

	/*
	 * ask for affiliation
	 */
	do {

	    /*
	     * request affiliation
	     */
	    author_set[i].affiliation = NULL;
	    author_set[i].affiliation = prompt("Enter author affiliation, or press return to skip", &len);
	    if (len == 0) {
		dbg(DBG_VHIGH, "Affiliation not given");
	    } else {
		dbg(DBG_VHIGH, "Affiliation: %s", author_set[i].affiliation);
	    }

	    /*
	     * reject if too long
	     */
	    if (len > MAX_AFFILIATION_LEN) {

		/*
		 * issue rejection message
		 */
		(void) fprintf(stderr, "\nSorry ( tm Canada :-) ), we limit affiliation names to %d characters\n\n", MAX_AFFILIATION_LEN);

		/*
		 * free storage
		 */
		if (author_set[i].affiliation != NULL) {
		    free(author_set[i].affiliation);
		    author_set[i].affiliation = NULL;
		}
		continue;
	    }

	    /*
	     * just in case we have a bogus length
	     */
	    if (len < 0) {
		errp(106, __FUNCTION__, "Bogus affiliation length: %d < 0", len);
		/*NOTREACHED*/
	    }
	} while (author_set[i].affiliation == NULL);

	/* XXX - more code here - XXX */
    }

    /*
     * store author set
     */
    *author_set_p = author_set;

    /*
     * return the author count
     */
    return author_count;
}
