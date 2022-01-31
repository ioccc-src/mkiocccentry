/*
 * mkiocccentry - make an ioccc entry
 *
 * Make an IOCCC compressed tarball for an IOCCC entry.
 *
 * We will form the IOCCC entry compressed tarball "by hand" in C.
 * Not in some high level language, but standard vanilla C.
 * Why?  Because this is a obfuscated C contest.  But then why isn't
 * this code obfuscated?  Because the IOCCC judges prefer to write
 * in robust unobfuscated code.  Besides, the IOCCC was started
 * as an ironic commentary on the Bourne shell source and finger daemon
 * source.  Moreover, irony is well baked-in to the IOCCC.  :-)
 *
 * If you do find a problem with this code, let the judges know.
 * To contact the judges please see:
 *
 *      https://www.ioccc.org/judges.html
 *
 * "Because even printf has a return value worth paying attention to." :-)
 *
 * Many thanks are due to a number of people who provided important
 * and valuable testing, suggestions, issue reports and GitHub pull
 * requests to this code.  Without their time and effort, this tool
 * would not work very well!
 *
 * Among the GitHub users we wish to thank include these fine developers
 * in alphabetical GitHub @user order:
 *
 *	@ilyakurdyukov		Ilya Kurdyukov
 *	@SirWumpus		Anthony Howe
 *	@vog			Volker Diels-Grabsch
 *	@xexyl			Cody Boone Ferguson
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


/* special comments for the seqcexit tool */
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
#include <time.h>
#include <sys/time.h>


/*
 * IOCCC size and rule related limitations
 */
#include "limit_ioccc.h"


/*
 * dbg - debug, warning and error reporting facility
 */
#include "dbg.h"


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


/*
 * mkiocccentry version
 */
#define MKIOCCCENTRY_VERSION "0.32 2022-01-24"	/* use format: major.minor YYYY-MM-DD */
#define IOCCC_CONTEST "IOCCC28"			/* use format: IOCCC99 */
#define IOCCC_YEAR (2022)			/* Year IOCCC_CONTEST closes */


/*
 * Version of info for JSON the .info.json file.
 *
 * The following is NOT the version of this mkiocccentry tool!
 */
#define INFO_VERSION "1.6 2022-01-21"	/* version of the .info.json file to produce */


/*
 * Version of info for JSON the .author.json file.
 *
 * The following is NOT the version of this mkiocccentry tool!
 */
#define AUTHOR_VERSION "1.7 2022-01-21"	/* version of the .author.json file to produce */


/*
 * definitions
 */
#define LITLEN(x) (sizeof(x)-1)	/* length of a literal string w/o the NUL byte */
#define REQUIRED_ARGS (4)	/* number of required arguments on the command line */
#define UUID_LEN (36)		/* characters in a UUID string - as per RFC4122 */
#define UUID_VERSION (4)	/* version 4 - random UUID */
#define UUID_VARIANT (0xa)	/* variant 1 - encoded as 0xa */
#define ISO_3166_1_CODE_URL0 "https://en.wikipedia.org/wiki/ISO_3166-1#Officially_assigned_code_elements"
#define ISO_3166_1_CODE_URL1 "https://en.wikipedia.org/wiki/ISO_3166-1_alpha-2"
#define ISO_3166_1_CODE_URL2 "https://www.iso.org/obp/ui/#iso:pub:PUB500001:en"
#define ISO_3166_1_CODE_URL3 "https://www.iso.org/obp/ui/#search"
#define ISO_3166_1_CODE_URL4 "https://www.iso.org/glossary-for-iso-3166.html"
#define TAIL_TITLE_CHARS "abcdefghijklmnopqrstuvwxyz0123456789_+-"	/* [a-z0-9_+-] */
#define TIMESTAMP_EPOCH "Thr Jan  1 00:00:00 1970 UTC"	/* gettimeofday epoch */
#define TAR_PATH_0 "/usr/bin/tar"			/* historic path for tar */
#define TAR_PATH_1 "/bin/tar"			/* alternate tar path for some systems where /usr/bin/tar != /bin/tar */
#define CP_PATH_0 "/bin/cp"			/* historic path for cp */
#define CP_PATH_1 "/usr/bin/cp"			/* alternate cp path for some systems where /bin/cp != /usr/bin/cp */
#define LS_PATH_0 "/bin/ls"			/* historic path for ls */
#define LS_PATH_1 "/usr/bin/ls"			/* alternate ls path for some systems where /bin/ls != /usr/bin/ls */
#define IOCCC_REGISTER_URL "https://register.ioccc.org/NOT/a/real/URL"	/* XXX - change to real URL when ready */
#define IOCCC_SUBMIT_URL "https://submit.ioccc.org/NOT/a/real/URL"	/* XXX - change to real URL when ready */


/*
 * usage message
 *
 * Use the usage() function to print the these usage_msgX strings.
 */
static const char * const usage_msg0 =
    "usage: %s [-h] [-v level] [-V] [-t tar] [-c cp] [-l ls] [{-a|-i} answers] work_dir prog.c Makefile remarks.md [file ...]\n"
    "\n"
    "\t-h\t\t\tprint help message and exit 0\n"
    "\t-v level\t\tset verbosity level: (def level: %d)\n"
    "\t-V\t\t\tprint version string and exit\n"
    "\n"
    "\t-t /path/to/tar\t\tpath to tar executable that supports the -J (xz) option (def: %s)\n"
    "\t-c /path/to/cp\t\tpath to cp executable (def: %s)\n"
    "\t-l /path/to/ls\t\tpath to ls executable (def: %s)\n"
    "\t-a answers\t\twrite answers to a file for easier updating of an entry\n"
    "\t-i answers\t\tread answers from file previously written by -a answers\n";
static const char * const usage_msg1 =
    "\t    NOTE: One cannot use both -a answers and -i answers at the same time.\n"
    "\n"
    "\twork_dir\tdirectory where the entry directory and tarball are formed\n"
    "\tprog.c\t\tpath to the C source for your entry\n";
static const char * const usage_msg2 =
    "\n"
    "\tMakefile\tMakefile to build (make all) and cleanup (make clean & make clobber)\n"
    "\n"
    "\tremarks.md\tRemarks about your entry in markdown format\n"
    "\t\t\tNOTE: The following is a guide to markdown:\n"
    "\n"
    "\t\t\t    https://www.markdownguide.org/basic-syntax\n"
    "\n"
    "\t[file ...]\textra data files to include with your entry\n" "\n" "mkiocccentry version: %s\n";


/*
 * author info
 */
struct author {
    char *name;			/* name of the author */
    char *location_code;	/* author location/country code */
    char const *location_name;	/* name of author location/country */
    char *email;		/* Email address of author or or empty string ==> not provided */
    char *url;			/* home URL of author or or empty string ==> not provided */
    char *twitter;		/* author twitter handle or or empty string ==> not provided */
    char *github;		/* author GitHub username or or empty string ==> not provided */
    char *affiliation;		/* author affiliation or or empty string ==> not provided */
    int author_num;		/* author number */
};


/*
 * info for JSON
 *
 * Information we will collect in order to form the .info json file.
 */
struct info {
    /*
     * version
     */
    char *mkiocccentry_ver;	/* mkiocccentry version (MKIOCCCENTRY_VERSION) */
    char const *iocccsize_ver;	/* iocccsize version (compiled in, same as iocccsize -V) */
    /*
     * entry
     */
    char *ioccc_id;		/* IOCCC contest ID */
    int entry_num;		/* IOCCC entry number */
    char *title;		/* entry title */
    char *abstract;		/* entry abstract */
    ssize_t rule_2a_size;	/* Rule 2a size of prog.c */
    size_t rule_2b_size;	/* Rule 2b size of prog.c */
    bool empty_override;	/* true ==> empty prog.c override requested */
    bool rule_2a_override;	/* true ==> Rule 2a override requested */
    bool rule_2a_mismatch;	/* true ==> file size != rule_count function size */
    bool rule_2b_override;	/* true ==> Rule 2b override requested */
    bool highbit_warning;	/* true ==> high bit character(s) detected */
    bool nul_warning;		/* true ==> NUL character(s) detected */
    bool trigraph_warning;	/* true ==> unknown or invalid tri-graph(s) detected */
    bool wordbuf_warning;	/* true ==> word buffer overflow detected */
    bool ungetc_warning;	/* true ==> ungetc warning detected */
    bool Makefile_override;	/* true ==> Makefile rule override requested */
    bool warnings_ignored;	/* true ==> warnings were ignored */
    /*
     * filenames
     */
    char *prog_c;		/* prog.c filename */
    char *Makefile;		/* Makefile filename */
    char *remarks_md;		/* remarks.md filename */
    int extra_count;		/* number of extra files */
    char **extra_file;		/* list of extra filenames followed by NULL */
    /*
     * time
     */
    time_t tstamp;		/* seconds since epoch when .info json was formed (see gettimeofday(2)) */
    int usec;			/* microseconds since the tstamp second */
    char *epoch;		/* epoch of tstamp, currently: Thr Jan 1 00:00:00 1970 UTC */
    char *gmtime;		/* UTC converted string for tstamp (see asctime(3)) */
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
 *      https://en.wikipedia.org/wiki/ISO_3166-1#Officially_assigned_code_elements
 *      https://en.wikipedia.org/wiki/ISO_3166-1_alpha-2
 *      https://www.iso.org/obp/ui/#iso:pub:PUB500001:en
 *      https://www.iso.org/obp/ui/#search
 *
 * See also:
 *
 *      https://www.iso.org/glossary-for-iso-3166.html
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
    const char * const code;		/* ISO 3166-1 Alpha-2 Code */
    const char * const name;		/* name (short name lower case) */
} loc[] = {
    {"AA", "User-assigned code AA"},	/* User-assigned code */
    {"AC", "Ascension Island"},	/* Exceptionally reserved code */
    {"AD", "Andorra"},
    {"AE", "United Arab Emirates"},
    {"AF", "Afghanistan"},
    {"AG", "Antigua and Barbuda"},
    {"AI", "Anguilla"},
    {"AL", "Albania"},
    {"AM", "Armenia"},
    {"AN", "Netherlands Antilles"},	/* Transitionally reserved code */
    {"AO", "Angola"},
/*  {"AP", "African Regional Industrial Property Organization"}, */ /* WIPO Indeterminately reserved code */
    {"AQ", "Antarctica"},
    {"AR", "Argentina"},
    {"AS", "American Samoa"},
    {"AT", "Austria"},
    {"AU", "Australia"},
    {"AW", "Aruba"},
    {"AX", "\xc3\x85land Islands"},
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
    {"BL", "Saint Barth\xc3\xa9lemy"},
    {"BM", "Bermuda"},
    {"BN", "Brunei Darussalam"},
    {"BO", "Bolivia (Plurinational State of)"},
    {"BQ", "Bonaire, Sint Eustatius and Saba"},
    {"BR", "Brazil"},
    {"BS", "Bahamas"},
    {"BT", "Bhutan"},
    {"BU", "Burma"},		/* Transitionally reserved code */
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
    {"CI", "C\xc3\xb4te d'Ivoire"},
    {"CK", "Cook Islands"},
    {"CL", "Chile"},
    {"CM", "Cameroon"},
    {"CN", "China"},
    {"CO", "Colombia"},
    {"CP", "Clipperton Island"},	/* Exceptionally reserved code */
    {"CQ", "island of Sark"},	/* Exceptionally reserved code */
    {"CR", "Costa Rica"},
    {"CS", "Serbia and Montenegro"},	/* Transitionally reserved code */
    {"CT", "Canton and Enderbury Island"},	/* Formerly assigned code */
    {"CU", "Cuba"},
    {"CV", "Cabo Verde"},
    {"CW", "Cura\xc3\xa7" "ao"},
    {"CX", "Christmas Island"},
    {"CY", "Cyprus"},
    {"CZ", "Czechia"},
    {"DD", "German Democratic Republic"},	/* Formerly assigned code */
    {"DE", "Germany"},
    {"DG", "Diego Garcia"},	/* Exceptionally reserved code */
    {"DJ", "Djibouti"},
    {"DK", "Denmark"},
    {"DM", "Dominica"},
    {"DO", "Dominican Republic"},
    {"DY", "Benin"},		/* Indeterminately reserved code */
    {"DZ", "Algeria"},
    {"EA", "Ceuta, Melilla"},	/* Exceptionally reserved code */
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
    {"EU", "European Union"},	/* Exceptionally reserved code */
/*  {"EV", "Eurasian Patent Organization"}, */ /* WIPO Indeterminately reserved code */
    {"EW", "Estonia"},		/* Indeterminately reserved code */
    {"EZ", "European OTC derivatives"},	/* Exceptionally reserved code */
    {"FI", "Finland"},
    {"FJ", "Fiji"},
    {"FK", "Falkland Islands (the) [Malvinas]"},
    {"FL", "Liechtenstein"},	/* Indeterminately reserved code */
    {"FM", "Micronesia (Federated States of)"},
    {"FO", "Faroe Islands"},
    {"FQ", "French Southern and Antarctic Territories"},	/* Formerly assigned code */
    {"FR", "France"},
    {"FX", "France, Metropolitan"},	/* Exceptionally reserved code */
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
    {"HV", "Upper Volta"},	/* Formerly assigned code */
/*  {"IB", "International Bureau of WIPO"}, */ /* WIPO Indeterminately reserved code */
    {"IC", "Canary Islands"},	/* Exceptionally reserved code */
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
    {"JA", "Jamaica"},		/* Indeterminately reserved code */
    {"JE", "Jersey"},
    {"JM", "Jamaica"},
    {"JO", "Jordan"},
    {"JP", "Japan"},
    {"JT", "Johnston Island"},	/* Formerly assigned code */
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
    {"LF", "Libya Fezzan"},	/* Indeterminately reserved code */
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
    {"MI", "Midway Islands"},	/* Formerly assigned code */
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
    {"NH", "New Hebrides"},	/* Formerly assigned code */
    {"NI", "Nicaragua"},
    {"NL", "Netherlands"},
    {"NO", "Norway"},
    {"NP", "Nepal"},
    {"NQ", "Dronning Maud Land"},	/* Formerly assigned code */
    {"NR", "Nauru"},
    {"NT", "Neutral Zone"},	/* Transitionally reserved code */
    {"NU", "Niue"},
    {"NZ", "New Zealand"},
/*  {"OA", "African Intellectual Property Organization"}, */ /* WIPO Indeterminately reserved code */
    {"OO", "user-assigned escape code"},	/* User-assigned code */
    {"OM", "Oman"},
    {"PA", "Panama"},
    {"PC", "Pacific Islands Trust Territory"},	/* Formerly assigned code */
    {"PE", "Peru"},
    {"PF", "French Polynesia"},
    {"PG", "Papua New Guinea"},
    {"PH", "Philippines"},
    {"PI", "Philippines"},	/* Indeterminately reserved code */
    {"PK", "Pakistan"},
    {"PL", "Poland"},
    {"PM", "Saint Pierre and Miquelon"},
    {"PN", "Pitcairn"},
    {"PR", "Puerto Rico"},
    {"PS", "Palestine, State of"},
    {"PT", "Portugal"},
    {"PU", "United States Miscellaneous Pacific Islands"},	/* Formerly assigned code */
    {"PW", "Palau"},
    {"PY", "Paraguay"},
    {"PZ", "Panama Canal Zone"},	/* Formerly assigned code */
    {"QA", "Qatar"},
    {"QM", "User-assigned code QM"},	/* User-assigned code */
    {"QN", "User-assigned code QN"},	/* User-assigned code */
    {"QO", "User-assigned code QO"},	/* User-assigned code */
    {"QP", "User-assigned code QP"},	/* User-assigned code */
    {"QQ", "User-assigned code QQ"},	/* User-assigned code */
    {"QR", "User-assigned code QR"},	/* User-assigned code */
    {"QS", "User-assigned code QS"},	/* User-assigned code */
    {"QT", "User-assigned code QT"},	/* User-assigned code */
    {"QU", "User-assigned code QU"},	/* User-assigned code */
    {"QV", "User-assigned code QV"},	/* User-assigned code */
    {"QW", "User-assigned code QW"},	/* User-assigned code */
    {"QX", "User-assigned code QX"},	/* User-assigned code */
    {"QY", "User-assigned code QY"},	/* User-assigned code */
    {"QZ", "User-assigned code QZ"},	/* User-assigned code */
    {"RA", "Argentina"},	/* Indeterminately reserved code */
    {"RB", "Bolivia or Botswana"},	/* Indeterminately reserved code */
    {"RC", "China"},		/* Indeterminately reserved code */
    {"RE", "R\xc3\xa9union"},
    {"RH", "Haiti"},		/* Indeterminately reserved code */
    {"RI", "Indonesia"},	/* Indeterminately reserved code */
    {"RL", "Lebanon"},		/* Indeterminately reserved code */
    {"RM", "Madagascar"},	/* Indeterminately reserved code */
    {"RN", "Niger"},		/* Indeterminately reserved code */
    {"RO", "Romania"},
    {"RP", "Philippines"},	/* Indeterminately reserved code */
    {"RS", "Serbia"},
    {"RU", "Russian Federation"},
    {"RW", "Rwanda"},
    {"SA", "Saudi Arabia"},
    {"SB", "Solomon Islands"},
    {"SC", "Seychelles"},
    {"SD", "Sudan"},
    {"SE", "Sweden"},
    {"SF", "Finland"},		/* Indeterminately reserved code */
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
    {"SU", "USSR"},		/* Exceptionally reserved code */
    {"SV", "El Salvador"},
    {"SX", "Sint Maarten (Dutch part)"},
    {"SY", "Syrian Arab Republic"},
    {"SZ", "Eswatini"},
    {"TA", "Tristan da Cunha"},	/* Exceptionally reserved code */
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
    {"TP", "Turkey"},		/* Transitionally reserved code */
    {"TR", "Turkey"},
    {"TT", "Trinidad and Tobago"},
    {"TV", "Tuvalu"},
    {"TW", "Taiwan"},
    {"TZ", "Tanzania, United Republic of"},
    {"UA", "Ukraine"},
    {"UG", "Uganda"},
    {"UK", "United Kingdom"},	/* Exceptionally reserved code */
    {"UM", "United States Minor Outlying Islands"},
    {"UN", "United Nations"},	/* Exceptionally reserved code */
    {"US", "United States of America"},
    {"UY", "Uruguay"},
    {"UZ", "Uzbekistan"},
    {"VA", "Holy See"},
    {"VC", "Saint Vincent and the Grenadines"},
    {"VD", "Democratic Republic of Viet-Nam"},	/* Formerly assigned code */
    {"VE", "Venezuela (Bolivarian Republic of)"},
    {"VG", "Virgin Islands (British)"},
    {"VI", "Virgin Islands (U.S.)"},
    {"VN", "Viet Nam"},
    {"VU", "Vanuatu"},
    {"WF", "Wallis and Futuna"},
    {"WG", "Grenada"},		/* Indeterminately reserved code */
    {"WK", "Wake Island"},	/* Formerly assigned code */
    {"WL", "Saint Lucia"},	/* Indeterminately reserved code */
/*  {"WO", "World Intellectual Property Organization"}, */ /* WIPO Indeterminately reserved code */
    {"WS", "Samoa"},
    {"WV", "Saint Vincent"},	/* Indeterminately reserved code */
    {"XA", "User-assigned code XA"},	/* User-assigned code */
    {"XB", "User-assigned code XB"},	/* User-assigned code */
    {"XC", "User-assigned code XC"},	/* User-assigned code */
    {"XD", "User-assigned code XD"},	/* User-assigned code */
    {"XE", "User-assigned code XE"},	/* User-assigned code */
    {"XF", "User-assigned code XF"},	/* User-assigned code */
    {"XG", "User-assigned code XG"},	/* User-assigned code */
    {"XH", "User-assigned code XH"},	/* User-assigned code */
    {"XI", "User-assigned code XI"},	/* User-assigned code */
    {"XJ", "User-assigned code XJ"},	/* User-assigned code */
    {"XK", "User-assigned code XK"},	/* User-assigned code */
    {"XL", "User-assigned code XL"},	/* User-assigned code */
    {"XM", "User-assigned code XM"},	/* User-assigned code */
    {"XN", "User-assigned code XN"},	/* User-assigned code */
    {"XO", "User-assigned code XO"},	/* User-assigned code */
    {"XP", "User-assigned code XP"},	/* User-assigned code */
    {"XR", "User-assigned code XR"},	/* User-assigned code */
    {"XS", "User-assigned code XS"},	/* User-assigned code */
    {"XT", "User-assigned code XT"},	/* User-assigned code */
    {"XU", "User-assigned code XU"},	/* User-assigned code */
    {"XV", "User-assigned code XV"},	/* User-assigned code */
    {"XW", "User-assigned code XW"},	/* User-assigned code */
    {"XX", "User-assigned code XX"},	/* User-assigned code */
    {"XY", "User-assigned code XY"},	/* User-assigned code */
    {"XZ", "User-assigned code XZ"},	/* User-assigned code */
    {"YD", "Democratic Yemen"},	/* Formerly assigned code */
    {"YE", "Yemen"},
    {"YT", "Mayotte"},
    {"YU", "Yugoslavia"},	/* Transitionally reserved code */
    {"YV", "Venezuela"},	/* Indeterminately reserved code */
    {"ZA", "South Africa"},
    {"ZM", "Zambia"},
    {"ZR", "Zaire"},		/* Transitionally reserved code */
    {"ZW", "Zimbabwe"},
    {"ZZ", "User-assigned code ZZ"},	/* User-assigned code */
    {NULL, NULL},		/* MUST BE LAST */
};


/*
 * globals
 */
int verbosity_level = DBG_DEFAULT;	/* debug level set by -v */

/*
 * forward declarations
 */
static void usage(int exitcode, char const *name, char const *str, char const *program,
		  char const *tar, char const *cp, char const *ls) __attribute__((noreturn));
static void free_info(struct info *infop);
static void free_author_array(struct author *authorp, int author_count);
static bool exists(char const *path);
static bool is_file(char const *path);
static bool is_exec(char const *path);
static bool is_dir(char const *path);
static bool is_read(char const *path);
static bool is_write(char const *path);
static ssize_t file_size(char const *path);
static void check_prog_c(struct info *infop, char const *entry_dir, char const *cp, char const *prog_c, FILE *answerp,
			 bool i_flag_used, unsigned *answers_errors);
static ssize_t readline(char **linep, FILE * stream);
static char *readline_dup(char **linep, bool strip, size_t *lenp, FILE * stream);
static void sanity_chk(struct info *infop, char const *work_dir, char const *tar,
		       char const *cp, char const *ls, char const *answers, bool i_flag_used);
static void para(char const *line, ...);
static void fpara(FILE * stream, char const *line, ...);
static char *prompt(char const *str, size_t *lenp);
static char *get_contest_id(struct info *infop, bool *testp);
static int get_entry_num(struct info *infop);
static char *mk_entry_dir(char const *work_dir, char const *ioccc_id, int entry_num, char **tarball_path, time_t tstamp);
static bool inspect_Makefile(char const *Makefile);
static void check_Makefile(struct info *infop, char const *entry_dir, char const *cp, char const *Makefile, FILE *answerp,
			   bool i_flag_used, unsigned *answers_errors);
static void check_remarks_md(struct info *infop, char const *entry_dir, char const *cp, char const *remarks_md);
static char *base_name(char const *path);
static void check_extra_data_files(struct info *infop, char const *entry_dir, char const *cp, int count, char **args);
static char const *lookup_location_name(char const *upper_code);
static bool yes_or_no(char const *question);
static char *get_title(struct info *infop);
static char *get_abstract(struct info *infop);
static int get_author_info(struct info *infop, char *ioccc_id, struct author **author_set, FILE *answerp, bool i_flag_used, 
	unsigned *answers_errors);
static void verify_entry_dir(char const *entry_dir, char const *ls);
static bool json_putc(int const c, FILE *stream);
static bool json_fprintf_str(FILE *stream, char const *str);
static bool json_fprintf_value_string(FILE *stream, char const *lead, char const *name, char const *middle, char const *value,
				      char const *tail);
static bool json_fprintf_value_long(FILE *stream, char const *lead, char const *name, char const *middle, long value,
				    char const *tail);
static bool json_fprintf_value_bool(FILE *stream, char const *lead, char const *name, char const *middle, bool value,
				    char const *tail);
static char const * strnull(char const * const str);
static void write_info(struct info *infop, char const *entry_dir, bool test_mode);
static void write_author(struct info *infop, int author_count, struct author *authorp, char const *entry_dir);
static void form_tarball(char const *work_dir, char const *entry_dir, char const *tarball_path, char const *tar, char const *ls);
static void remind_user(char const *work_dir, char const *entry_dir, char const *tar, char const *tarball_path, bool test_mode,
			char const *answers, struct info *infop, unsigned answers_errors);
static char *cmdprintf(char const *format, ...);


int
main(int argc, char *argv[])
{
    char *program = NULL;	/* our name */
    extern char *optarg;	/* option argument */
    extern int optind;		/* argv index of the next arg */
    struct timeval tp;		/* gettimeofday time value */
    char const *work_dir = NULL;	/* where the entry directory and tarball are formed */
    char const *prog_c = NULL;		/* path to prog.c */
    char const *Makefile = NULL;	/* path to Makefile */
    char const *remarks_md = NULL;	/* path to remarks.md */
    char const *tar = TAR_PATH_0;	/* path to tar executable that supports the -J (xz) option */
    char const *cp = CP_PATH_0;		/* path to cp executable */
    char const *ls = LS_PATH_0;		/* path to ls executable */
    char const *answers = NULL;		/* path to the answers file (recording input given on stdin) */
    FILE *answerp = NULL;		/* file pointer to the answers file */
    bool test_mode = false;		/* true ==> contest ID is test */
    char *entry_dir = NULL;	/* entry directory from which to form a compressed tarball */
    char *tarball_path = NULL;	/* path of the compressed tarball to form */
    int extra_count = 0;	/* number of extra files */
    char **extra_list = NULL;	/* list of extra files (if any) */
    struct info info;		/* data to form .info.json */
    int author_count = 0;	/* number of authors */
    struct author *author_set = NULL;	/* list of authors */
    bool t_flag_used = false;	/* true ==> -t /path/to/tar was given */
    bool c_flag_used = false;	/* true ==> -c /path/to/cp was given */
    bool l_flag_used = false;	/* true ==> -l /path/to/ls was given */
    bool a_flag_used = false;	/* true ==> -a write answers to answers file */
    bool i_flag_used = false;	/* true ==> -i read answers from answers file */
    unsigned answers_errors = 0; /* > 0 ==> flushing or closing answers file failed */
    int ret;			/* libc return code */
    int i;

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, "hv:Vt:c:l:a:i:")) != -1) {
	switch (i) {
	case 'h':		/* -h - print help to stderr and exit 0 */
	    usage(0, __func__, "-h help mode", program, tar, cp, ls);
	    not_reached();
	    break;
	case 'v':		/* -v verbosity */
	    /*
	     * parse verbosity
	     */
	    errno = 0;		/* pre-clear errno for errp() */
	    verbosity_level = (int)strtol(optarg, NULL, 0);
	    if (errno != 0) {
		errp(1, __func__, "cannot parse -v arg: %s error: %s", optarg, strerror(errno));
		not_reached();
	    }
	    break;
	case 'V':		/* -V - print version and exit */
	    errno = 0;		/* pre-clear errno for errp() */
	    ret = printf("%s\n", MKIOCCCENTRY_VERSION);
	    if (ret <= 0) {
		warnp(__func__, "printf error printing version string: %s", MKIOCCCENTRY_VERSION);
	    }
	    exit(0); /*ooo*/
	    not_reached();
	    break;
	case 't':		/* -t /path/to/tar */
	    tar = optarg;
	    t_flag_used = true;
	    break;
	case 'c':		/* -c /path/to/cp */
	    cp = optarg;
	    c_flag_used = true;
	    break;
	case 'l':		/* -l /path/to/ls */
	    ls = optarg;
	    l_flag_used = true;
	    break;
	case 'a':		/* -a record_answers */
	    answers = optarg;
	    a_flag_used = true;
	    break;
	case 'i':		/* -i input_recorded_answers */
	    answers = optarg;
	    i_flag_used = true;
	    break;
	default:
	    usage(1, __func__, "invalid -flag", program, tar, cp, ls); /*ooo*/
	    not_reached();
	 }
    }
    /* must have at least the required number of args */
    if (argc - optind < REQUIRED_ARGS) {
	usage(1, __func__, "wrong number of arguments", program, tar, cp, ls); /*ooo*/
	not_reached();
    }
    /*
     * guess where tar, cp and ls utilities are located
     *
     * If the user did not give a -t, -c and/or -l /path/to/x path, then look at
     * the historic location for the utility.  If the historic location of the utility
     * isn't executable, look for an executable in the alternate location.
     *
     * On some systems where /usr/bin != /bin, the distribution made the mistake of
     * moving historic critical applications, look to see if the alternate path works instead.
     */
    if (t_flag_used == false && !is_exec(TAR_PATH_0) && is_exec(TAR_PATH_1)) {
	tar = TAR_PATH_1;
	dbg(DBG_MED, "tar is not in historic location: %s : will try alternate location: %s", TAR_PATH_0, tar);
    }
    if (c_flag_used == false && !is_exec(CP_PATH_0) && is_exec(CP_PATH_1)) {
	cp = CP_PATH_1;
	dbg(DBG_MED, "cp is not in historic location: %s : will try alternate location: %s", CP_PATH_0, cp);
    }
    if (l_flag_used == false && !is_exec(LS_PATH_0) && is_exec(LS_PATH_1)) {
	ls = LS_PATH_1;
	dbg(DBG_MED, "ls is not in historic location: %s : will try alternate location: %s", LS_PATH_0, ls);
    }
    if (a_flag_used == true && i_flag_used == true) {
	err(2, __func__, "-a answers and -i answers cannot be used together");
	not_reached();
    }
    /* collect required the required args */
    extra_count = (argc - optind > REQUIRED_ARGS) ? argc - optind - REQUIRED_ARGS : 0;
    extra_list = argv + optind + REQUIRED_ARGS;
    dbg(DBG_LOW, "tar: %s", tar);
    dbg(DBG_LOW, "cp: %s", cp);
    dbg(DBG_LOW, "ls: %s", ls);
    work_dir = argv[optind];
    dbg(DBG_LOW, "work_dir: %s", work_dir);
    prog_c = argv[optind + 1];
    dbg(DBG_LOW, "prog.c: %s", prog_c);
    Makefile = argv[optind + 2];
    dbg(DBG_LOW, "Makefile: %s", Makefile);
    remarks_md = argv[optind + 3];
    dbg(DBG_LOW, "remarks: %s", remarks_md);
    dbg(DBG_LOW, "number of extra data file args: %d", extra_count);
    dbg(DBG_LOW, "answers file: %s", answers);

    /*
     * zerosize info
     */
    memset(&info, 0, sizeof(info));

    /*
     * record the time
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = gettimeofday(&tp, NULL);
    if (ret < 0) {
	errp(3, __func__, "gettimeofday failed");
	not_reached();
    }
    info.tstamp = tp.tv_sec;
    dbg(DBG_HIGH, "info.tstamp: %ld", info.tstamp);
    info.usec = tp.tv_usec;
    dbg(DBG_HIGH, "infop->usec: %ld", (long)info.usec);

    /*
     * Welcome
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = printf("Welcome to mkiocccentry version: %s\n", MKIOCCCENTRY_VERSION);
    if (ret <= 0) {
	errp(4, __func__, "printf error printing the welcome string");
	not_reached();
    }

    /*
     * save our version
     */
    errno = 0;			/* pre-clear errno for errp() */
    info.mkiocccentry_ver = strdup(MKIOCCCENTRY_VERSION);
    if (info.mkiocccentry_ver == NULL) {
	errp(5, __func__, "cannot strdup version: %s", MKIOCCCENTRY_VERSION);
	not_reached();
    }

    /*
     * environment sanity checks
     */
    para("", "Performing sanity checks on your environment ...", NULL);
    sanity_chk(&info, work_dir, tar, cp, ls, answers, i_flag_used);
    para("... environment looks OK", "", NULL);

    /*
     * check if we should read input from answers file
     */
    if (i_flag_used == true) {
	if (is_read(answers) == false) {
	    errp(6, __func__, "cannot read answers file");
	    not_reached();
	}
	errno = 0;
	answerp = freopen(answers, "r", stdin);
	if (answerp == NULL) {
	    errp(7, __func__, "cannot open answers file");
	    not_reached();
	}
    }
    /*
     * obtain the IOCCC contest ID
     */
    info.ioccc_id = get_contest_id(&info, &test_mode);
    dbg(DBG_MED, "IOCCC contest ID: %s", info.ioccc_id);
    /*
     * obtain entry number
     */
    info.entry_num = get_entry_num(&info);
    dbg(DBG_MED, "entry number: %d", info.entry_num);

    /*
     * create entry directory
     */
    entry_dir = mk_entry_dir(work_dir, info.ioccc_id, info.entry_num, &tarball_path, info.tstamp);
    dbg(DBG_LOW, "formed entry directory: %s", entry_dir);


    /*
     * if -a, open the answers file. We only do it after verifying that we can
     * make the entry directory because if we get input before and find out the
     * directory already exists then the answers file will have invalid data.
     */
    if (a_flag_used && answers != NULL && strlen(answers) > 0) {
	errno = 0;			/* pre-clear errno for errp() */
	answerp = fopen(answers, "w");
	if (answerp == NULL) {
	    errp(8, __func__, "cannot create answers file: %s", answers);
	    not_reached();
	}
    }

    /*
     * write the IOCCC id and entry number to the answers file
     */
    if (answerp != NULL) {
	errno = 0;			/* pre-clear errno for errp() */
        ret = fprintf(answerp, "%s\n", info.ioccc_id);
	if (ret <= 0) {
	    warnp(__func__, "fprintf error printing IOCCC contest id to the answers file");
	}
	errno = 0;			/* pre-clear errno for errp() */
	ret = fprintf(answerp, "%d\n", info.entry_num);
	if (ret <= 0) {
	    warnp(__func__, "fprintf error printing entry number to the answers file");
	}
    }

    /*
     * check prog.c
     */
    para("", "Checking prog.c ...", NULL);
    check_prog_c(&info, entry_dir, cp, prog_c, answerp, i_flag_used, &answers_errors);
    para("... completed prog.c check.", "", NULL);

    /*
     * check Makefile
     */
    para("Checking Makefile ...", NULL);
    check_Makefile(&info, entry_dir, cp, Makefile, answerp, i_flag_used, &answers_errors);
    para("... completed Makefile check.", "", NULL);

    /*
     * check remarks.md
     */
    para("Checking remarks.md ...", NULL);
    check_remarks_md(&info, entry_dir, cp, remarks_md);
    para("... completed remarks.md check.", "", NULL);

    /*
     * check, if needed, extra data files
     */
    para("Checking extra data files ...", NULL);
    check_extra_data_files(&info, entry_dir, cp, extra_count, extra_list);
    para("... completed extra data files check.", "", NULL);

    /*
     * obtain the title
     */
    info.title = get_title(&info);
    dbg(DBG_LOW, "entry title: %s", info.title);
    if (answerp != NULL) {
	errno = 0;			/* pre-clear errno for errp() */
	ret = fprintf(answerp, "%s\n", info.title);
	if (ret <= 0) {
	    warnp(__func__, "fprintf error printing title to the answers file");
	}

    }

    /*
     * obtain the abstract
     */
    info.abstract = get_abstract(&info);
    dbg(DBG_LOW, "entry abstract: %s", info.abstract);
    if (answerp != NULL) {
	errno = 0;			/* pre-clear errno for errp() */
	ret = fprintf(answerp, "%s\n", info.abstract);
	if (ret <= 0) {
	    warnp(__func__, "fprintf error printing abstract to the answers file");
	}
    }


    /*
     * obtain author information
     */
    author_count = get_author_info(&info, info.ioccc_id, &author_set, answerp, i_flag_used, &answers_errors);
    dbg(DBG_LOW, "collected information on %d authors", author_count);


    /*
     * write the .info.json file
     */
    para("", "Forming the .info.json file ...", NULL);
    write_info(&info, entry_dir, test_mode);
    para("... completed the .info.json file.", "", NULL);

    /*
     * write the .author.json file
     */
    para("", "Forming the .author.json file ...", NULL);
    write_author(&info, author_count, author_set, entry_dir);
    para("... completed .author.json file.", "", NULL);

    /*
     * form the .txz file
     */
    form_tarball(work_dir, entry_dir, tarball_path, tar, ls);

    /*
     * finalize the answers file, writing final answers (if writing answers) and
     * then closing the stream.
     */
    if (answerp != NULL) {
	if (a_flag_used == true) {
	    errno = 0;			/* pre-clear errno for errp() */
	    ret = fprintf(answerp, "y\n");
	    if (ret <= 0) {
		++answers_errors;
		warnp(__func__, "unable to write confirming y to the answers file");
	    }
	    errno = 0;			/* pre-clear errno for errp() */
	    ret = fflush(answerp);
	    if (ret != 0) {
		warnp(__func__, "unable to flush data to the answers file");
		++answers_errors;
	    }
	}
	ret = fclose(answerp);
	if (ret != 0) {
	    warnp(__func__, "error in fclose to the answers file");
	    ++answers_errors;
	}
	answerp = NULL;
    }


    /*
     * remind user various things e.g., to upload (unless in test mode)
     */
    remind_user(work_dir, entry_dir, tar, tarball_path, test_mode, answers, &info, answers_errors);

    /*
     * free storage
     */
    if (entry_dir != NULL) {
	free(entry_dir);
	entry_dir = NULL;
    }
    if (tarball_path != NULL) {
	free(tarball_path);
	tarball_path = NULL;
    }
    free_info(&info);
    memset(&info, 0, sizeof(info));
    if (author_set != NULL) {
	free_author_array(author_set, author_count);
	free(author_set);
	author_set = NULL;
    }


    /*
     * All Done!!! - Jessica Noll, age 2
     */
    return 0; /*ooo*/
}


/*
 * usage - print usage to stderr
 *
 * Example:
 *      usage(3, __func__, "missing required argument(s)", "/usr/bin/tar", "/bin/cp", "/bin/ls");
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *       Normally one should NOT include newlines in warn messages.
 */
static void
usage(int exitcode, char const *name, char const *str, char const *program, char const *tar, char const *cp, char const *ls)
{
    /*
     * firewall
     */
    if (name == NULL) {
	name = "((NULL name))";
	warn(__func__, "\nin usage(): program was NULL, forcing it to be: %s\n", program);
    }
    if (str == NULL) {
	str = "((NULL str))";
	warn(__func__, "\nin usage(): str was NULL, forcing it to be: %s\n", str);
    }
    if (program == NULL) {
	program = "((NULL program))";
	warn(__func__, "\nin usage(): program was NULL, forcing it to be: %s\n", program);
    }
    if (tar == NULL) {
	tar = "((NULL tar))";
	warn(__func__, "\nin usage(): tar was NULL, forcing it to be: %s\n", tar);
    }
    if (cp == NULL) {
	cp = "((NULL cp))";
	warn(__func__, "\nin usage(): cp was NULL, forcing it to be: %s\n", cp);
    }
    if (ls == NULL) {
	ls = "((NULL ls))";
	warn(__func__, "\nin usage(): ls was NULL, forcing it to be: %s\n", ls);
    }

    /*
     * print the formatted usage stream
     */
    vfprintf_usage(DO_NOT_EXIT, stderr, "%s\n", str);
    vfprintf_usage(DO_NOT_EXIT, stderr, usage_msg0, program, DBG_DEFAULT, tar, cp, ls);
    vfprintf_usage(DO_NOT_EXIT, stderr, "%s", usage_msg1);
    vfprintf_usage(exitcode, stderr, usage_msg2, MKIOCCCENTRY_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}


/*
 * free_info - free info and related sub-elements
 *
 * given:
 *      infop   - pointer to info structure to free
 *
 * This function does not return.
 */
static void
free_info(struct info *infop)
{
    int i;

    /*
     * firewall
     */
    if (infop == NULL) {
	err(11, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * free version values
     */
    if (infop->mkiocccentry_ver != NULL) {
	free(infop->mkiocccentry_ver);
	infop->mkiocccentry_ver = NULL;
    }

    /*
     * free entry values
     */
    if (infop->ioccc_id != NULL) {
	free(infop->ioccc_id);
	infop->ioccc_id = NULL;
    }
    if (infop->title != NULL) {
	free(infop->title);
	infop->title = NULL;
    }
    if (infop->abstract != NULL) {
	free(infop->abstract);
	infop->abstract = NULL;
    }

    /*
     * free filenames
     */
    if (infop->prog_c != NULL) {
	free(infop->prog_c);
	infop->prog_c = NULL;
    }
    if (infop->Makefile != NULL) {
	free(infop->Makefile);
	infop->Makefile = NULL;
    }
    if (infop->remarks_md != NULL) {
	free(infop->remarks_md);
	infop->remarks_md = NULL;
    }
    if (infop->extra_file != NULL) {
	for (i = 0; i < infop->extra_count; ++i) {
	    if (infop->extra_file[i] != NULL) {
		free(infop->extra_file[i]);
		infop->extra_file[i] = NULL;
	    }
	}
	free(infop->extra_file);
	infop->extra_file = NULL;
    }

    /*
     * free time values
     */
    if (infop->epoch != NULL) {
	free(infop->epoch);
	infop->epoch = NULL;
    }
    if (infop->gmtime != NULL) {
	free(infop->gmtime);
	infop->gmtime = NULL;
    }
    return;
}


/*
 * free_author_array - free storage related to a struct author
 *
 * given:
 *      author_set      - pointer to a struct author array
 *      author_count    - length of author array
 */
static void
free_author_array(struct author *author_set, int author_count)
{
    int i;

    /*
     * firewall
     */
    if (author_set == NULL) {
	err(12, __func__, "called with NULL arg(s)");
	not_reached();
    }
    if (author_count < 0) {
	err(13, __func__, "author_count: %d < 0", author_count);
	not_reached();
    }

    /*
     * free elements of each array member
     */
    for (i = 0; i < author_count; ++i) {
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
	if (author_set[i].github != NULL) {
	    free(author_set[i].github);
	    author_set[i].github = NULL;
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
 *      path    - the path to test
 *
 * returns:
 *      true ==> path exists,
 *      false ==> path does not exist
 *
 * This function does not return.
 */
static bool
exists(char const *path)
{
    int ret;			/* return code holder */
    struct stat buf;		/* path status */

    /*
     * firewall
     */
    if (path == NULL) {
	err(14, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * test for existence of path
     */
    ret = stat(path, &buf);
    if (ret < 0) {
	dbg(DBG_HIGH, "path %s does not exist, stat returned: %d", path, ret);
	return false;
    }
    dbg(DBG_VHIGH, "path %s size: %ld", path, (long)buf.st_size);
    return true;
}


/*
 * is_file - if a path is a file
 *
 * This function tests if a path exists and is a regular file.
 *
 * given:
 *
 *      path    - the path to test
 *
 * returns:
 *      true ==> path exists and is a regular file,
 *      false ==> path does not exist OR is not a regular file
 */
static bool
is_file(char const *path)
{
    int ret;			/* return code holder */
    struct stat buf;		/* path status */

    /*
     * firewall
     */
    if (path == NULL) {
	err(15, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * test for existence of path
     */
    ret = stat(path, &buf);
    if (ret < 0) {
	dbg(DBG_HIGH, "path %s does not exist, stat returned: %d", path, ret);
	return false;
    }
    dbg(DBG_VHIGH, "path %s size: %ld", path, (long)buf.st_size);

    /*
     * test if path is a regular file
     */
    if (!S_ISREG(buf.st_mode)) {
	dbg(DBG_HIGH, "path %s is not a regular file", path);
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
 *      path    - the path to test
 *
 * returns:
 *      true ==> path exists and we have executable access,
 *      false ==> path does not exist OR is not executable OR
 *                we don't have permission to execute it
 */
static bool
is_exec(char const *path)
{
    int ret;			/* return code holder */
    struct stat buf;		/* path status */

    /*
     * firewall
     */
    if (path == NULL) {
	err(16, __func__, "called with NULL arg(s)");
	not_reached();
     }

    /*
     * test for existence of path
     */
    ret = stat(path, &buf);
    if (ret < 0) {
	dbg(DBG_HIGH, "path %s does not exist, stat returned: %d", path, ret);
	return false;
    }
    dbg(DBG_VHIGH, "path %s size: %ld", path, (long)buf.st_size);

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
 *      path    - the path to test
 *
 * returns:
 *      true ==> path exists and is a directory,
 *      false ==> path does not exist OR is not a directory
 */
static bool
is_dir(char const *path)
{
    int ret;			/* return code holder */
    struct stat buf;		/* path status */

    /*
     * firewall
     */
    if (path == NULL) {
	err(17, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * test for existence of path
     */
    ret = stat(path, &buf);
    if (ret < 0) {
	dbg(DBG_HIGH, "path %s does not exist, stat returned: %d", path, ret);
	return false;
    }
    dbg(DBG_VHIGH, "path %s size: %ld", path, (long)buf.st_size);

    /*
     * test if path is a regular directory
     */
    if (!S_ISDIR(buf.st_mode)) {
	dbg(DBG_HIGH, "path %s is not a directory", path);
	return false;
    }
    dbg(DBG_VHIGH, "path %s is a directory", path);
    return true;
}


/*
 * is_read - if a path is readable
 *
 * This function tests if a path exists and we have permissions to read it.
 *
 * given:
 *
 *      path    - the path to test
 *
 * returns:
 *      true ==> path exists and we have read access,
 *      false ==> path does not exist OR is not read OR
 *                we don't have permission to read it
 */
static bool
is_read(char const *path)
{
    int ret;			/* return code holder */
    struct stat buf;		/* path status */

    /*
     * firewall
     */
    if (path == NULL) {
	err(18, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * test for existence of path
     */
    ret = stat(path, &buf);
    if (ret < 0) {
	dbg(DBG_HIGH, "path %s does not exist, stat returned: %d", path, ret);
	return false;
    }
    dbg(DBG_VHIGH, "path %s size: %ld", path, (long)buf.st_size);

    /*
     * test if we are allowed to execute it
     */
    ret = access(path, R_OK);
    if (ret < 0) {
	dbg(DBG_HIGH, "path %s is not readable", path);
	return false;
    }
    dbg(DBG_VHIGH, "path %s is readable", path);
    return true;
}


/*
 * is_write - if a path is writable
 *
 * This function tests if a path exists and we have permissions to write it.
 *
 * given:
 *
 *      path    - the path to test
 *
 * returns:
 *      true ==> path exists and we have write access,
 *      false ==> path does not exist OR is not writable OR
 *                we don't have permission to write it
 */
static bool
is_write(char const *path)
{
    int ret;			/* return code holder */
    struct stat buf;		/* path status */

    /*
     * firewall
     */
    if (path == NULL) {
	err(19, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * test for existence of path
     */
    ret = stat(path, &buf);
    if (ret < 0) {
	dbg(DBG_HIGH, "path %s does not exist, stat returned: %d", path, ret);
	return false;
    }
    dbg(DBG_VHIGH, "path %s size: %ld", path, (long)buf.st_size);

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
 * file_size - determine the file size
 *
 * Return the file size, or -1 if the file does not exist.
 *
 * given:
 *
 *      path    - the path to test
 *
 * returns:
 *      >= 0 ==> file size,
 *      <0 ==> file does not exist
 */
static ssize_t
file_size(char const *path)
{
    int ret;			/* return code holder */
    struct stat buf;		/* path status */

    /*
     * firewall
     */
    if (path == NULL) {
	err(20, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * test for existence of path
     */
    ret = stat(path, &buf);
    if (ret < 0) {
	dbg(DBG_HIGH, "path %s does not exist, stat returned: %d", path, ret);
	return -1;
    }

    /*
     * return file size
     */
    dbg(DBG_VHIGH, "path %s size: %ld", path, (long)buf.st_size);
    return (ssize_t)buf.st_size;
}


/*
 * readline - read a line from a stream
 *
 * Read a line from an open stream.  Malloc or realloc the line
 * buffer as needed.  Remove the trailing newline that was read.
 *
 * given:
 *      linep   - malloced line buffer (may be realloced) or ptr to NULL
 *                NULL ==> getline() will malloc() the linep buffer
 *                else ==> getline() might realloc() the linep buffer
 *      stream - file stream to read from
 *
 * returns:
 *      number of characters in the line with newline removed,
 *      or -1 for EOF
 *
 * This function does not return on error.
 */
static ssize_t
readline(char **linep, FILE * stream)
{
    size_t linecap = 0;		/* allocated capacity of linep buffer */
    ssize_t ret;		/* getline return and our modified size return */
    char *p;			/* printer to NUL */

    /*
     * firewall
     */
    if (linep == NULL || stream == NULL) {
	err(21, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * read the line
     */
    clearerr(stream);
    errno = 0;			/* pre-clear errno for errp() */
    ret = getline(linep, &linecap, stream);
    if (ret < 0) {
	if (feof(stream)) {
	    dbg(DBG_VVHIGH, "EOF detected on readline");
	    return -1; /* EOF found */
	} else if (ferror(stream)) {
	    errp(22, __func__, "getline() error");
	    not_reached();
	} else {
	    errp(23, __func__, "unexpected getline() error");
	    not_reached();
	}
    }
    /*
     * paranoia
     */
    if (*linep == NULL) {
	err(24, __func__, "*linep is NULL after getline()");
	not_reached();
    }

    /*
     * scan for embedded NUL bytes (before end of line)
     *
     * We could use memchr() but
     */
    errno = 0;			/* pre-clear errno for errp() */
    p = (char *)memchr(*linep, 0, (size_t)ret);
    if (p != NULL) {
	errp(25, __func__, "found NUL before end of line");
	not_reached();
    }

    /*
     * process trailing newline or lack there of
     */
    if ((*linep)[ret - 1] != '\n') {
	warn(__func__, "line does not end in newline: %s", *linep);
    } else {
	(*linep)[ret - 1] = '\0';	/* clear newline */
	--ret;
    }
    dbg(DBG_VVHIGH, "read %ld bytes + newline into %lu byte buffer", (long)ret, (unsigned long)linecap);

    /*
     * return length of line without the trailing newline
     */
    return ret;
}


/*
 * readline_dup - read a line from a stream and duplicate to a malloced buffer.
 *
 * given:
 *      linep   - malloced line buffer (may be realloced) or ptr to NULL
 *                NULL ==> getline() will malloc() the linep buffer
 *                else ==> getline() might realloc() the linep buffer
 *      strip   - true ==> remove trailing whitespace,
 *                false ==> only remove the trailing newline
 *      lenp    - != NULL ==> pointer to length of final length of line malloced,
 *                NULL ==> do not return length of line
 *      stream - file stream to read from
 *
 * returns:
 *      malloced buffer containing the input without a trailing newline,
 *      and if strip == true, without trailing whitespace,
 *      or NULL ==> EOF
 *
 * NOTE: This function will NOT return NULL.
 *
 * This function does not return on error.
 */
static char *
readline_dup(char **linep, bool strip, size_t *lenp, FILE *stream)
{
    ssize_t len;		/* getline return and our modified size return */
    char *ret;			/* malloced input */
    ssize_t i;

    /*
     * firewall
     */
    if (linep == NULL || stream == NULL) {
	err(26, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * read the line
     */
    len = readline(linep, stream);
    if (len < 0) {
	/*
	 * EOF found
	 */
	return NULL;
    }
    dbg(DBG_VVHIGH, "readline returned %ld bytes", (long)len);

    /*
     * duplicate the line
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = strdup(*linep);
    if (ret == NULL) {
	errp(27, __func__, "strdup of read line of %ld bytes failed", (long)len);
	not_reached();
    }

    /*
     * strip trailing whitespace if requested
     */
    if (strip == true) {
	if (len > 0) {
	    for (i = len - 1; i >= 0; --i) {
		if (isascii(ret[i]) && isspace(ret[i])) {
		    /*
		     * strip trailing ASCII whitespace
		     */
		    --len;
		    ret[i] = '\0';
		} else {
		    break;
		}
	    }
	}
	dbg(DBG_VVHIGH, "readline, after trailing whitespace strip is %ld bytes", (long)len);
    }
    if (lenp != NULL) {
	*lenp = (size_t)len;
    }

    /*
     * return the malloced string
     */
    return ret;
}


/*
 * sanity_chk - perform basic sanity checks
 *
 * We perform basic sanity checks on paths and the IOCCC contest ID.
 *
 * given:
 *
 *      infop           - pointer to info structure
 *      work_dir        - where the entry directory and tarball are formed
 *      tar             - path to tar that supports the -J (xz) option
 *	cp		- path to the cp utility
 *	ls		- path to the ls utility
 *	answers		- path to the answers file (or NULL if not specified)
 *	i_flag_used	- if -i was specified
 *
 * NOTE: This function does not return on error or if things are not sane.
 */
static void
sanity_chk(struct info *infop, char const *work_dir, char const *tar, char const *cp, char const *ls, char const *answers,
	   bool i_flag_used)
{
    /*
     * firewall
     */
    if (infop == NULL || work_dir == NULL || tar == NULL) {
	err(28, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * tar must be executable
     */
    if (!exists(tar)) {
	fpara(stderr,
	      "",
	      "We cannot find a tar program.",
	      "",
	      "A tar program that supports the -J (xz) option is required to build an compressed tarball.",
	      "Perhaps you need to use:",
	      "",
	      "    mkiocccentry -t /path/to/tar ...",
	      "",
	      "and/or install a tar program?  You can find the source for tar:",
	      "",
	      "    https://www.gnu.org/software/tar/",
	      "",
	      NULL);
	err(29, __func__, "tar does not exist: %s", tar);
	not_reached();
    }
    if (!is_file(tar)) {
	fpara(stderr,
	      "",
	      "The tar, while it exists, is not a file.",
	      "",
	      "Perhaps you need to use another path:",
	      "",
	      "    mkiocccentry -t /path/to/tar ...",
	      "",
	      "and/or install a tar program?  You can find the source for tar:",
	      "",
	      "    https://www.gnu.org/software/tar/",
	      "",
	      NULL);
	err(30, __func__, "tar is not a file: %s", tar);
	not_reached();
    }
    if (!is_exec(tar)) {
	fpara(stderr,
	      "",
	      "The tar, while it is a file, is not executable.",
	      "",
	      "We suggest you check the permissions on the tar program, or use another path:",
	      "",
	      "    mkiocccentry -t /path/to/tar ...",
	      "",
	      "and/or install a tar program?  You can find the source for tar:",
	      "",
	      "    https://www.gnu.org/software/tar/",
	      "",
	      NULL);
	err(31, __func__, "tar is not executable program: %s", tar);
	not_reached();
    }

    /*
     * cp must be executable
     */
    if (!exists(cp)) {
	fpara(stderr,
	      "",
	      "We cannot find a cp program.",
	      "",
	      "A cp program is required to copy files into a directory under work_dir.",
	      "Perhaps you need to use:",
	      "",
	      "    mkiocccentry -c /path/to/cp ...",
	      "",
	      "and/or install a cp program?  You can find the source for cp in core utilities:",
	      "",
	      "    https://www.gnu.org/software/coreutils/",
	      "",
	      NULL);
	err(32, __func__, "cp does not exist: %s", cp);
	not_reached();
    }
    if (!is_file(cp)) {
	fpara(stderr,
	      "",
	      "The cp, while it exists, is not a file.",
	      "",
	      "Perhaps you need to use another path:",
	      "",
	      "    mkiocccentry -c /path/to/cp ...",
	      "",
	      "and/or install a cp program?  You can find the source for cp in core utilities:",
	      "",
	      "    https://www.gnu.org/software/coreutils/",
	      "",
	      NULL);
	err(33, __func__, "cp is not a file: %s", cp);
	not_reached();
    }
    if (!is_exec(cp)) {
	fpara(stderr,
	      "",
	      "The cp, while it is a file, is not executable.",
	      "",
	      "We suggest you check the permissions on the cp program, or use another path:",
	      "",
	      "    mkiocccentry -c /path/to/cp ...",
	      "",
	      "and/or install a cp program?  You can find the source for cp in core utilities:",
	      "",
	      "    https://www.gnu.org/software/coreutils/",
	      "",
	      NULL);
	err(34, __func__, "cp is not executable program: %s", cp);
	not_reached();
    }

    /*
     * ls must be executable
     */
    if (!exists(ls)) {
	fpara(stderr,
	      "",
	      "We cannot find a ls program.",
	      "",
	      "A ls program is required to copy files into a directory under work_dir.",
	      "Perhaps you need to use:",
	      "",
	      "    mkiocccentry -l /path/to/ls ...",
	      "",
	      "and/or install a ls program?  You can find the source for ls in core utilities:",
	      "",
	      "    https://www.gnu.org/software/coreutils/",
	      "",
	      NULL);
	err(35, __func__, "ls does not exist: %s", ls);
	not_reached();
    }
    if (!is_file(ls)) {
	fpara(stderr,
	      "",
	      "The ls, while it exists, is not a file.",
	      "",
	      "Perhaps you need to use another path:",
	      "",
	      "    mkiocccentry -l /path/to/ls ...",
	      "",
	      "and/or install a ls program?  You can find the source for ls in core utilities:",
	      "",
	      "    https://www.gnu.org/software/coreutils/",
	      "",
	      NULL);
	err(36, __func__, "ls is not a file: %s", ls);
	not_reached();
    }
    if (!is_exec(ls)) {
	fpara(stderr,
	      "",
	      "The ls, while it is a file, is not executable.",
	      "",
	      "We suggest you check the permissions on the ls program, or use another path:",
	      "",
	      "    mkiocccentry -l /path/to/ls ...",
	      "",
	      "and/or install a ls program?  You can find the source for ls in core utilities:",
	      "",
	      "    https://www.gnu.org/software/coreutils/",
	      "",
	      NULL);
	err(37, __func__, "ls is not executable program: %s", ls);
	not_reached();
    }

    /*
     * work_dir must be a writable directory
     */
    if (!exists(work_dir)) {
	fpara(stderr,
	      "",
	      "The work_dir does not exist.",
	      "",
	      "You should either create work_dir, or use a different work_dir directory path on the command line.",
	      "",
	      NULL);
	err(38, __func__, "work_dir does not exist: %s", work_dir);
	not_reached();
    }
    if (!is_dir(work_dir)) {
	fpara(stderr,
	      "",
	      "While work_dir exists, it is not a directory.",
	      "",
	      "You should move or remove work_dir and them make a new work_dir directory, or use a different",
	      "work_dir directory path on the command line.",
	      "",
	      NULL);
	err(39, __func__, "work_dir is not a directory: %s", work_dir);
	not_reached();
    }
    if (!is_write(work_dir)) {
	fpara(stderr,
	      "",
	      "While the directory work_dir exists, it is not a writable directory.",
	      "",
	      "You should change the permission to make work_dir writable, or you move or remove work_dir and then",
	      "create a new writable directory, or use a different work_dir directory path on the command line.",
	      "",
	      NULL);
	err(40, __func__, "work_dir is not a writable directory: %s", work_dir);
	not_reached();
    }

    /*
     * if answers was specified, then it must not exist
     */
    if (answers && strlen(answers) > 0 && exists(answers) && i_flag_used == false) {
	fprintf(stderr,
		"The answers file already exists.\n\n"
		"You might consider using the file to automate the process of answering the prompts e.g. by.\n"
		"\t./mkiocccentry -i %s ...\n\n"
		"If you've ignored warnings and you've fixed these (or you ignore different warnings)\n"
		"we STRONGLY recommend you re-enter your answers!\n\n", answers);
	err(41, __func__, "answers file already exists: %s", answers);
	not_reached();
    }


    /*
     * obtain version string from iocccsize_version
     */
    infop->iocccsize_ver = iocccsize_version;
    return;
}


/*
 * para - print a paragraph of lines to stdout
 *
 * Print a collection of strings with newlines added after each string.
 * The final string pointer must be a NULL.
 *
 * Example:
 *      para("line 1", "line 2", "", "prev line 3 was an empty line", NULL);
 *
 * given:
 *      line    - 1st paragraph line to print
 *      ...     - strings as paragraph lines to print
 *      NULL    - end of string list
 *
 * This function does not return on error.
 */
static void
para(char const *line, ...)
{
    va_list ap;			/* stdarg block */
    int ret;			/* libc function return value */
    int fd;			/* stdout as a file descriptor or -1 */
    int line_cnt;		/* number of lines in the paragraph */

    /*
     * stdarg setup
     */
    va_start(ap, line);

    /*
     * firewall
     */
    if (stdout == NULL) {
	err(42, __func__, "stdout is NULL");
	not_reached();
    }
    clearerr(stdout);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    /*
     * this may not always catch a bogus or un-opened stdout, but try anyway
     */
    fd = fileno(stdout);
    if (fd < 0) {
	errp(43, __func__, "fileno on stdout returned: %d < 0", fd);
	not_reached();
    }
    clearerr(stdout);		/* paranoia */

    /*
     * print paragraph strings followed by newlines
     */
    line_cnt = 0;
    while (line != NULL) {

	/*
	 * print the string
	 */
	clearerr(stdout);	/* pre-clear ferror() status */
	errno = 0;		/* pre-clear errno for errp() */
	ret = fputs(line, stdout);
	if (ret == EOF) {
	    if (ferror(stdout)) {
		errp(44, __func__, "error writing paragraph to a stdout");
		not_reached();
	    } else if (feof(stdout)) {
		err(45, __func__, "EOF while writing paragraph to a stdout");
		not_reached();
	    } else {
		errp(46, __func__, "unexpected fputs error writing paragraph to a stdout");
		not_reached();
	    }
	}

	/*
	 * print the newline
	 */
	clearerr(stdout);	/* pre-clear ferror() status */
	errno = 0;		/* pre-clear errno for errp() */
	ret = fputc('\n', stdout);
	if (ret == EOF) {
	    if (ferror(stdout)) {
		errp(47, __func__, "error writing newline to a stdout");
		not_reached();
	    } else if (feof(stdout)) {
		err(48, __func__, "EOF while writing newline to a stdout");
		not_reached();
	    } else {
		errp(49, __func__, "unexpected fputc error newline a stdout");
		not_reached();
	    }
	}
	++line_cnt;		/* count this line as printed */

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
    clearerr(stdout);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fflush(stdout);
    if (ret == EOF) {
	if (ferror(stdout)) {
	    errp(50, __func__, "error flushing stdout");
	    not_reached();
	} else if (feof(stdout)) {
	    err(51, __func__, "EOF while flushing stdout");
	    not_reached();
	} else {
	    errp(52, __func__, "unexpected fflush error while flushing stdout");
	    not_reached();
	}
    }
    dbg(DBG_VVHIGH, "%s() printed %d line paragraph", __func__, line_cnt);
    return;
}


/*
 * fpara - print a paragraph of lines to an open stream
 *
 * Print a collection of strings with newlines added after each string.
 * The final string pointer must be a NULL.
 *
 * Example:
 *      fpara(stderr, "line 1", "line 2", "", "prev line 3 was an empty line", NULL);
 *
 * given:
 *      stream  - open file stream to print a paragraph onto
 *      line    - 1st paragraph line to print
 *      ...     - strings as paragraph lines to print
 *      NULL    - end of string list
 *
 * This function does not return on error.
 */
static void
fpara(FILE * stream, char const *line, ...)
{
    va_list ap;			/* stdarg block */
    int ret;			/* libc function return value */
    int fd;			/* stream as a file descriptor or -1 */
    int line_cnt;		/* number of lines in the paragraph */

    /*
     * stdarg setup
     */
    va_start(ap, line);

    /*
     * firewall
     */
    if (stream == NULL) {
	err(53, __func__, "stream is NULL");
	not_reached();
    }
    clearerr(stream);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */

    /*
     * this may not always catch a bogus or un-opened stream, but try anyway
     */
    fd = fileno(stream);
    if (fd < 0) {
	errp(54, __func__, "fileno on stream returned: %d < 0", fd);
	not_reached();
    }
    clearerr(stream);		/* paranoia */

    /*
     * print paragraph strings followed by newlines
     */
    line_cnt = 0;
    while (line != NULL) {

	/*
	 * print the string
	 */
	clearerr(stream);	/* pre-clear ferror() status */
	errno = 0;		/* pre-clear errno for errp() */
	ret = fputs(line, stream);
	if (ret == EOF) {
	    if (ferror(stream)) {
		errp(55, __func__, "error writing paragraph to a stream");
		not_reached();
	    } else if (feof(stream)) {
		err(56, __func__, "EOF while writing paragraph to a stream");
		not_reached();
	    } else {
		errp(57, __func__, "unexpected fputs error writing paragraph to a stream");
		not_reached();
	    }
	}

	/*
	 * print the newline
	 */
	clearerr(stream);	/* pre-clear ferror() status */
	errno = 0;		/* pre-clear errno for errp() */
	ret = fputc('\n', stream);
	if (ret == EOF) {
	    if (ferror(stream)) {
		errp(58, __func__, "error writing newline to a stream");
		not_reached();
	    } else if (feof(stream)) {
		err(59, __func__, "EOF while writing newline to a stream");
		not_reached();
	    } else {
		errp(60, __func__, "unexpected fputc error newline a stream");
		not_reached();
	    }
	}
	++line_cnt;		/* count this line as printed */

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
    clearerr(stream);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fflush(stream);
    if (ret == EOF) {
	if (ferror(stream)) {
	    errp(61, __func__, "error flushing stream");
	    not_reached();
	} else if (feof(stream)) {
	    err(62, __func__, "EOF while flushing stream");
	    not_reached();
	} else {
	    errp(63, __func__, "unexpected fflush error while flushing stream");
	    not_reached();
	}
    }
    dbg(DBG_VVHIGH, "%s() printed %d line paragraph", __func__, line_cnt);
    return;
}


/*
 * prompt - prompt for a string
 *
 * Prompt a string, followed by :<space> on stdout and then read a line from stdin.
 * The line is stripped of the trailing newline and then of all trailing whitespace.
 *
 * given:
 *      str     - string to string followed by :<space>
 *      lenp    - pointer to where to put the length of the response,
 *                NULL ==> do not save length
 *
 *
 * returns:
 *      malloced input string with newline and trailing whitespace removed
 *	if lenp != NULL, *lenp will be set to the length of the response
 *
 * NOTE: This function will NOT return NULL.
 *
 * This function does not return on error.
 */
static char *
prompt(char const *str, size_t *lenp)
{
    char *linep = NULL;		/* readline_dup line buffer */
    int ret;			/* libc function return value */
    size_t len;			/* length of input */
    char *buf;			/* malloced input string */

    /*
     * firewall
     */
    if (str == NULL) {
	err(64, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * prompt + :<space>
     */
    clearerr(stdout);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fputs(str, stdout);
    if (ret == EOF) {
	if (ferror(stdout)) {
	    errp(65, __func__, "error printing prompt string");
	    not_reached();
	} else if (feof(stdout)) {
	    err(66, __func__, "EOF while printing prompt string");
	    not_reached();
	} else {
	    errp(67, __func__, "unexpected fputs error printing prompt string");
	    not_reached();
	}
    }
    clearerr(stdout);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fputs(": ", stdout);
    if (ret == EOF) {
	if (ferror(stdout)) {
	    errp(68, __func__, "error printing :<space>");
	    not_reached();
	} else if (feof(stdout)) {
	    err(69, __func__, "EOF while writing :<space>");
	    not_reached();
	} else {
	    errp(70, __func__, "unexpected fputs error printing :<space>");
	    not_reached();
	}
    }
    clearerr(stdout);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fflush(stdout);
    if (ret == EOF) {
	if (ferror(stdout)) {
	    errp(71, __func__, "error flushing prompt to stdout");
	    not_reached();
	} else if (feof(stdout)) {
	    err(72, __func__, "EOF while flushing prompt to stdout");
	    not_reached();
	} else {
	    errp(73, __func__, "unexpected fflush error while flushing prompt to stdout");
	    not_reached();
	}
    }

    /*
     * read user input - return input length
     */
    buf = readline_dup(&linep, true, &len, stdin);
    if (buf == NULL) {
	err(74, __func__, "EOF while reading prompt input");
	not_reached();
    }
    dbg(DBG_VHIGH, "received a %lu byte response", (unsigned long)len);

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
 * get IOCCC ID or test
 *
 * This function will prompt the user for a contest ID, validate it and return it
 * as a malloced string.  If the contest ID is the special value "test", then
 * *testp will be set to true, otherwise it will be set to false.
 *
 * given:
 *      infop   - pointer to info structure
 *      testp   - pointer to boolean for test mode
 *
 * returns:
 *      malloced contest ID string
 *      *testp == true ==> contest ID is "test", else contest ID is a UUID.
 *
 * This function does not return on error or if the contest ID is malformed.
 */
static char *
get_contest_id(struct info *infop, bool *testp)
{
    char *malloc_ret;		/* malloced return string */
    size_t len;			/* input string length */
    int ret;			/* libc function return */
    unsigned int a, b, c, d, e, f;	/* parts of the UUID string */
    unsigned int version = 0;	/* UUID version hex character */
    unsigned int variant = 0;	/* UUID variant hex character */
    char guard;			/* scanf guard to catch excess amount of input */
    size_t i;

    /*
     * firewall
     */
    if (infop == NULL || testp == NULL) {
	err(75, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * explain contest ID
     */
    para("To submit entries to the IOCCC, you must a registered contestant and have received a",
	 "IOCCC contest ID (via email) shortly after you have been successfully registered.",
	 "If the IOCCC is open, you may register as a contestant. See:",
	 "",
	 NULL);
    errno = 0;		/* pre-clear errno for errp() */
    ret = fprintf(stderr, "    %s\n", IOCCC_REGISTER_URL);
    if (ret <= 0) {
	warnp(__func__, "fprintf error printing IOCCC_REGISTER_URL");
    }
    para("",
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
	ret = 0;		/* initialize paranoia */

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
	 *             xxxxxxxx-xxxx-4xxx-axxx-xxxxxxxxxxxx
	 *
	 * where 'x' is a hex character.  The 4 is the UUID version and a the variant 1.
	 */
	if (len != UUID_LEN) {

	    /*
	     * reject improper input length
	     */
	    errno = 0;		/* pre-clear errno for errp() */
	    ret = fprintf(stderr, "\nIOCCC contest ID are %d characters in length, you entered %lu\n\n",
				  UUID_LEN, (unsigned long)len);
	    if (ret <= 0) {
		warnp(__func__, "fprintf error while improper input length");
	    }
	    fpara(stderr,
		  "IOCCC contest IDs are of the form:",
		  "",
		  "    xxxxxxxx-xxxx-4xxx-axxx-xxxxxxxxxxxx",
		  "",
		  "where 'x' is a hex character, 4 is the UUID version and a the variant 1.",
		  "",
		  NULL);

	    /*
	     * free storage
	     */
	    if (malloc_ret != NULL) {
		free(malloc_ret);
		malloc_ret = NULL;
	    }
	    continue;
	}
	/*
	 * convert to lower case
	 */
	for (i = 0; i < len; ++i) {
	    malloc_ret[i] = (char)tolower(malloc_ret[i]);
	}
	dbg(DBG_VHIGH, "converted the IOCCC contest ID to: %s", malloc_ret);
	ret = sscanf(malloc_ret, "%8x-%4x-%1x%3x-%1x%3x-%8x%4x%c", &a, &b, &version, &c, &variant, &d, &e, &f, &guard);
	dbg(DBG_HIGH, "UUID version hex char: %1x", version);
	dbg(DBG_HIGH, "UUID variant hex char: %1x", variant);
	if (ret != 8 || version != UUID_VERSION || variant != UUID_VARIANT) {
	    fpara(stderr,
		  "",
		  "IOCCC contest IDs are version 4, variant 1 UUID as defined by RFC4122:",
		  "",
		  "    https://datatracker.ietf.org/doc/html/rfc4122#section-4.1.1",
		  "",
		  "They are of the form:",
		  "",
		  "    xxxxxxxx-xxxx-4xxx-axxx-xxxxxxxxxxxx",
		  "",
		  "where 'x' is a hex character, 4 is the UUID version and a the variant 1.",
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
	 NULL);
    *testp = false;		/* IOCCC contest ID is not test */

    /*
     * return IOCCC contest ID
     */
    return malloc_ret;
}


/*
 * get_entry_num - obtain the entry number
 *
 * given:
 *      infop   - pointer to info structure
 *
 * returns:
 *      entry number >= 0 <= MAX_ENTRY_NUM
 */
static int
get_entry_num(struct info *infop)
{
    int entry_num;		/* entry number */
    char *entry_str;		/* entry number string */
    int ret;			/* libc function return */
    char guard;			/* scanf guard to catch excess amount of input */

    /*
     * firewall
     */
    if (infop == NULL) {
	err(76, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * keep asking for an entry number until we get a valid reply
     */
    do {

	/*
	 * explain entry numbers
	 */
	errno = 0;		/* pre-clear errno for errp() */
	ret = printf("\nYou are allowed to submit up to %d entries to a given IOCCC.\n", MAX_ENTRY_NUM + 1);
	if (ret <= 0) {
	    errp(77, __func__, "printf error printing number of entries allowed");
	    not_reached();
	}
	para("",
	     "As in C, Entry numbers start with 0.  If you are updating a previous entry, PLEASE",
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
	errno = 0;		/* pre-clear errno for errp() */
	ret = sscanf(entry_str, "%d%c", &entry_num, &guard);
	if (ret != 1 || entry_num < 0 || entry_num > MAX_ENTRY_NUM) {
	    errno = 0;		/* pre-clear errno for errp() */
	    ret = fprintf(stderr, "\nThe entry number must be a number from 0 thru %d, please re-enter.\n", MAX_ENTRY_NUM);
	    if (ret <= 0) {
		warnp(__func__, "fprintf error while informing about the valid entry number range");
	    }
	    entry_num = -1;	/* invalidate input */
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
 *      work_dir        - working directory under which the entry directory is formed
 *      ioccc_id        - IOCCC entry ID (or test)
 *      entry_num       - entry number
 *      tarball_path    - pointer to the malloced path to where the compressed tarball will be formed
 *      tstamp          - now as a timestamp
 *
 * returns:
 *      the path of the working directory
 *
 * This function does not return on error or if the entry directory cannot be formed.
 */
static char *
mk_entry_dir(char const *work_dir, char const *ioccc_id, int entry_num, char **tarball_path, time_t tstamp)
{
    size_t entry_dir_len;	/* length of entry directory */
    size_t tarball_len;		/* length of the compressed tarball path */
    char *entry_dir = NULL;	/* malloced entry directory path */
    int ret;			/* libc function return */

    /*
     * firewall
     */
    if (work_dir == NULL || ioccc_id == NULL || tarball_path == NULL) {
	err(78, __func__, "called with NULL arg(s)");
	not_reached();
    }
    if (entry_num < 0 || entry_num > MAX_ENTRY_NUM) {
	err(79, __func__, "entry number: %d must >= 0 and <= %d", entry_num, MAX_ENTRY_NUM);
	not_reached();
    }

    /*
     * determine length of entry directory path
     */
    /*
     * work_dir/ioccc_id-entry
     */
    entry_dir_len = strlen(work_dir) + 1 + strlen(ioccc_id) + 1 + MAX_ENTRY_CHARS + 1 + 1;
    errno = 0;			/* pre-clear errno for errp() */
    entry_dir = (char *)malloc(entry_dir_len + 1);
    if (entry_dir == NULL) {
	errp(80, __func__, "malloc #0 of %lu bytes failed", (unsigned long)(entry_dir_len + 1));
	not_reached();
    }
    errno = 0;			/* pre-clear errno for errp() */
    ret = snprintf(entry_dir, entry_dir_len + 1, "%s/%s-%d", work_dir, ioccc_id, entry_num);
    if (ret <= 0) {
	errp(81, __func__, "snprintf to form entry directory failed");
	not_reached();
    }
    dbg(DBG_HIGH, "entry directory path: %s", entry_dir);

    /*
     * verify that the entry directory does not exist
     */
    if (exists(entry_dir)) {
	errno = 0;		/* pre-clear errno for errp() */
	ret = fprintf(stderr, "\nentry directory already exists: %s\n", entry_dir);
	if (ret <= 0) {
	    warnp(__func__, "fprintf error while informing that the entry directory already exists");
	}
	fpara(stderr,
	      "",
	      "You need to move that directory, or remove it, or use a different work_dir.",
	      "",
	      NULL);
	err(82, __func__, "entry directory exists: %s", entry_dir);
	not_reached();
    }
    dbg(DBG_HIGH, "entry directory path: %s", entry_dir);

    /*
     * make the entry directory
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = mkdir(entry_dir, 0755);
    if (ret < 0) {
	errp(83, __func__, "cannot mkdir %s with mode 0755", entry_dir);
	not_reached();
    }

    /*
     * form the compressed tarball path
     *
     * We assume timestamps will be values of 12 decimal digits or less in the future. :-)
     */
    tarball_len = LITLEN("entry.") + strlen(ioccc_id) + 1 + MAX_ENTRY_CHARS + LITLEN(".123456789012.txz") + 1;
    errno = 0;			/* pre-clear errno for errp() */
    *tarball_path = (char *)malloc(tarball_len + 1);
    if (*tarball_path == NULL) {
	errp(84, __func__, "malloc #1 of %lu bytes failed", (unsigned long)(tarball_len + 1));
	not_reached();
    }
    errno = 0;			/* pre-clear errno for errp() */
    ret = snprintf(*tarball_path, tarball_len + 1, "entry.%s.%d.%ld.txz", ioccc_id, entry_num, tstamp);
    if (ret <= 0) {
	errp(85, __func__, "snprintf to form compressed tarball path failed");
	not_reached();
    }
    dbg(DBG_HIGH, "compressed tarball path: %s", *tarball_path);

    /*
     * return entry directory
     */
    return entry_dir;
}


/*
 * check_prog_c - check prog_c arg and if OK, copy into entry_dir/prog.c
 *
 * Check if the prog_c argument is a readable file, and
 * if it is within the guidelines of iocccsize (or if the author overrides),
 * and if all is OK or overridden,
 * use cp to copy into entry_dir/prog.c.
 *
 * given:
 *      infop           - pointer to info structure
 *      entry_dir       - newly created entry directory (by mk_entry_dir()) under work_dir
 *      cp              - cp utility path
 *      prog_c          - prog_c arg: given path to prog.c
 *      answerp		- answer file
 *      i_flag_used	- if -i was specified
 *      answers_errors  - > 0 I/O errors on answers file
 *
 * This function does not return on error.
 */
static void
check_prog_c(struct info *infop, char const *entry_dir, char const *cp, char const *prog_c, FILE *answerp, bool i_flag_used,
	unsigned *answers_errors)
{
    FILE *prog_stream;		/* prog.c open file stream */
    struct iocccsize size;	/* rule_count() processing results */
    bool yorn = false;		/* response to a question */
    size_t prog_c_len;		/* length of the prog_c path */
    size_t entry_dir_len;	/* length of the entry_dir path */
    char *cmd = NULL;		/* cp prog_c entry_dir/prog.c */
    int exit_code;		/* exit code from system(cmd) */
    int ret;			/* libc function return */

    /*
     * firewall
     */
    if (infop == NULL || entry_dir == NULL || cp == NULL || prog_c == NULL) {
	err(86, __func__, "called with NULL arg(s)");
	not_reached();
    }
    entry_dir_len = strlen(entry_dir);
    if (entry_dir_len <= 0) {
	err(87, __func__, "entry_dir arg is an empty string");
	not_reached();
    }
    prog_c_len = strlen(prog_c);
    if (prog_c_len <= 0) {
	err(88, __func__, "prog_c arg is an empty string");
	not_reached();
    }

    /*
     * prog.c must be a readable file
     */
    if (!exists(prog_c)) {
	fpara(stderr,
	      "",
	      "We cannot find the prog.c file.",
	      "",
	      NULL);
	err(89, __func__, "prog.c does not exist: %s", prog_c);
	not_reached();
    }
    if (!is_file(prog_c)) {
	fpara(stderr,
	      "",
	      "The prog.c path, while it exists, is not a file.",
	      "",
	      NULL);
	err(90, __func__, "prog.c is not a file: %s", prog_c);
	not_reached();
    }
    if (!is_read(prog_c)) {
	fpara(stderr,
	      "",
	      "The prog.c, while it is a file, is not readable.",
	      "",
	      NULL);
	err(91, __func__, "prog.c is not a readable file: %s", prog_c);
	not_reached();
    }

    /*
     * warn if prog.c is empty
     */
    infop->rule_2a_size = file_size(prog_c);
    dbg(DBG_MED, "Rule 2a size: %ld", (long)infop->rule_2a_size);
    if (infop->rule_2a_size < 0) {
	err(92, __func__, "file_size error: %ld on prog_c: %s", (long)infop->rule_2a_size, prog_c);
	not_reached();
    } else if (infop->rule_2a_size == 0) {
	dbg(DBG_MED, "prog.c: %s is empty", prog_c);
	fpara(stderr,
	      "WARNING: prog.c is empty.  An empty prog.c has been submitted before:",
	      "",
	      "    https://www.ioccc.org/years.html#1994_smr",
	      "",
	      "The guidelines indicate that we tend to dislike programs that:",
	      "",
	      "    * are rather similar to previous winners  :-(",
	      "",
	      "Perhaps you have a different twist on an empty prog.c than yet another",
	      "smallest self-replicating program.  If so, the you may proceed, although",
	      "we strongly suggest that you put into your remarks.md file, why your",
	      "entry prog.c is not another smallest self-replicating program.",
	      "",
	      NULL);
	yorn = yes_or_no("Are you sure you want to submit an empty prog.c file? [yn]");
	if (yorn == false) {
	    err(93, __func__, "please fix your prog.c file: %s", prog_c);
	    not_reached();
	}
	dbg(DBG_LOW, "user says that their empty prog.c: %s is OK", prog_c);
	infop->empty_override = true;
	infop->rule_2a_override = false;
	infop->warnings_ignored = true;
	if (answerp != NULL && i_flag_used == false) {
	    errno = 0;			/* pre-clear errno for errp() */
	    ret = fprintf(answerp, "y\n");
	    if (ret <= 0) {
		warnp(__func__, "fprintf error writing user confirmation that empty prog.c is OK to answers file");
		(*answers_errors)++;
	    }
	}


    /*
     * warn if prog.c is too large under Rule 2a
     */
    } else if (infop->rule_2a_size > RULE_2A_SIZE) {
	dbg(DBG_MED, "prog.c: %s size: %ld > Rule 2a size: %ld", prog_c,
		     (long)infop->rule_2a_size, (long)RULE_2A_SIZE);
	errno = 0;		/* pre-clear errno for errp() */
	ret = fprintf(stderr, "\nWARNING: The prog.c %s size: %ld > Rule 2a maximum: %ld\n", prog_c,
		      (long)infop->rule_2a_size, (long)RULE_2A_SIZE);
	if (ret <= 0) {
	    warnp(__func__, "fprintf error when printing prog.c Rule 2a warning");
	}
	fpara(stderr,
	      "If you are attempting some clever rule abuse, then we strongly suggest that you",
	      "tell us about your rule abuse in your remarks.md file.  Be sure you have read the",
	      "\"ABUSING THE RULES\" section of the guidelines.  And more importantly, read rule 12!",
	      "",
	      NULL);
	yorn = yes_or_no("Are you sure you want to submit such a large prog.c file? [yn]");
	if (yorn == false) {
	    err(94, __func__, "please fix your prog.c file: %s", prog_c);
	    not_reached();
	}
	dbg(DBG_LOW, "user says that their prog.c %s size: %ld > Rule 2a max size: %ld is OK", prog_c,
	    (long)infop->rule_2a_size, (long)RULE_2A_SIZE);
	infop->empty_override = false;
	infop->rule_2a_override = true;
	infop->warnings_ignored = true;
	if (answerp != NULL && i_flag_used == false) {
	    errno = 0;			/* pre-clear errno for errp() */
	    ret = fprintf(answerp, "y\n");
	    if (ret <= 0) {
		warnp(__func__, "fprintf error writing user confirmation that prog.c size > Rule 2a max size is OK to answers file");
		(*answers_errors)++;
	    }
	}
    } else {
	infop->empty_override = false;
	infop->rule_2a_override = false;
    }

    /*
     * determine Rule 2b for prog.c
     *
     * If the size returned is outside of the allowed range, the user will
     * be asked if they want to proceed.  We will allow it if the user insists
     * on proceeding, or of iocccsize says the size is allowed under Rule 2a.
     */
    errno = 0;			/* pre-clear errno for errp() */
    prog_stream = fopen(prog_c, "r");
    if (prog_stream == NULL) {
	errp(95, __func__, "failed to fopen: %s", prog_c);
	not_reached();
    }
    size = rule_count(prog_stream, 0);
    infop->rule_2b_size = size.rule_2b_size;
    dbg(DBG_MED, "prog.c: %s Rule 2b size: %lu", prog_c, (unsigned long)infop->rule_2b_size);
    errno = 0;			/* pre-clear errno for errp() */
    ret = fclose(prog_stream);
    if (ret != 0) {
	errp(96, __func__, "failed to fclose: %s", prog_c);
	not_reached();
    }

    /*
     * sanity check on file size vs rule_count function size for Rule 2a
     */
    if (infop->rule_2a_size != (ssize_t)size.rule_2a_size) {
	errno = 0;		/* pre-clear errno for errp() */
	ret = fprintf(stderr, "\nInteresting: prog.c: %s file size: %ld != rule_count function size: %ld\n"
			      "In order to avoid a possible Rule 2a violation, BE SURE TO CLEARLY MENTION THIS IN\n"
			      "YOUR remarks.md FILE!\n\n",
			      prog_c, (long)infop->rule_2a_size, (long)size.rule_2a_size);
	if (ret <= 0) {
	    warnp(__func__, "fprintf error when printing prog.c file size and Rule 2a mismatch");
	}
	yorn = yes_or_no("Are you sure you want to proceed? [yn]");
	if (yorn == false) {
	    err(97, __func__, "please fix your prog.c file: %s", prog_c);
	    not_reached();
	}
	dbg(DBG_LOW, "user says that prog.c %s size: %ld != rule_count function size: %ld is OK", prog_c,
	    (long)infop->rule_2a_size, (long)size.rule_2a_size);
	infop->rule_2a_mismatch = true;
	infop->warnings_ignored = true;
	if (answerp != NULL && i_flag_used == false) {
	    errno = 0;			/* pre-clear errno for errp() */
	    ret = fprintf(answerp, "y\n");
	    if (ret <= 0) {
		warnp(__func__, "fprintf error writing user confirmation that prog.c size != rule_count function size is OK to answers file");
		(*answers_errors)++;
	    }
	}
    } else {
	infop->rule_2a_mismatch = false;
    }

    /*
     * sanity check on high bit character(s)
     */
    if (size.char_warning) {
	errno = 0;		/* pre-clear errno for errp() */
	ret = fprintf(stderr, "\nprog_c: %s has character(s) with high bit set!\n"
			      "Be careful you don't violate rule 13!\n\n", prog_c);
	if (ret <= 0) {
	    warnp(__func__, "fprintf error when printing prog.c char_warning");
	}
	yorn = yes_or_no("Are you sure you want to proceed? [yn]");
	if (yorn == false) {
	    err(98, __func__, "please fix your prog.c file: %s", prog_c);
	    not_reached();
	}
	dbg(DBG_LOW, "user says that prog.c %s having character(s) with high bit is OK", prog_c);
	infop->highbit_warning = true;
	infop->warnings_ignored = true;
	if (answerp != NULL && i_flag_used == false) {
	    errno = 0;			/* pre-clear errno for errp() */
	    ret = fprintf(answerp, "y\n");
	    if (ret <= 0) {
		warnp(__func__, "fprintf error writing user confirmation that prog.c having character(s) with high bit is OK to answers file");
		(*answers_errors)++;
	    }
	}
    } else {
	infop->highbit_warning = false;
    }

    /*
     * sanity check on NUL character(s)
     */
    if (size.nul_warning) {
	errno = 0;		/* pre-clear errno for errp() */
	ret = fprintf(stderr, "\nprog_c: %s has NUL character(s)!\n"
			      "Be careful you don't violate rule 13!\n\n", prog_c);
	if (ret <= 0) {
	    warnp(__func__, "fprintf error when printing prog.c nul_warning");
	}
	yorn = yes_or_no("Are you sure you want to proceed? [yn]");
	if (yorn == false) {
	    err(99, __func__, "please fix your prog.c file: %s", prog_c);
	    not_reached();
	}
	dbg(DBG_LOW, "user says that prog.c %s having NUL character(s) is OK", prog_c);
	infop->nul_warning = true;
	infop->warnings_ignored = true;
	if (answerp != NULL && i_flag_used == false) {
	    errno = 0;			/* pre-clear errno for errp() */
	    ret = fprintf(answerp, "y\n");
	    if (ret <= 0) {
		warnp(__func__, "fprintf error writing user confirmation that prog.c having NUL character(s) is OK to answers file");
		(*answers_errors)++;
	    }
	}
    } else {
	infop->nul_warning = false;
    }

    /*
     * sanity check on unknown tri-graph(s)
     */
    if (size.trigraph_warning) {
	errno = 0;		/* pre-clear errno for errp() */
	ret = fprintf(stderr, "\nprog_c: %s has unknown or invalid trigraph(s) found!\n"
			      "Is that a bug in, or a feature of your code?\n\n", prog_c);
	if (ret <= 0) {
	    warnp(__func__, "fprintf error when printing prog.c trigraph_warning");
	}
	yorn = yes_or_no("Are you sure you want to proceed? [yn]");
	if (yorn == false) {
	    err(100, __func__, "please fix your prog.c file: %s", prog_c);
	    not_reached();
	}
	dbg(DBG_LOW, "user says that prog.c %s having unknown or invalid trigraph(s) is OK", prog_c);
	infop->trigraph_warning = true;
	infop->warnings_ignored = true;
	if (answerp != NULL && i_flag_used == false) {
	    errno = 0;			/* pre-clear errno for errp() */
	    ret = fprintf(answerp, "y\n");
	    if (ret <= 0) {
		warnp(__func__, "fprintf error writing user confirmation that prog.c having unknown or invalid trigraph(s)) is OK to answers file");
		(*answers_errors)++;
	    }
	}
    } else {
	infop->trigraph_warning = false;
    }

    /*
     * sanity check on word buffer overflow
     */
    if (size.wordbuf_warning) {
	errno = 0;		/* pre-clear errno for errp() */
	ret = fprintf(stderr, "\nprog_c: %s triggered a word buffer overflow!\n"
			      "In order to avoid a possible Rule 2b violation, BE SURE TO CLEARLY MENTION THIS IN\n"
			      "YOUR remarks.md FILE!\n\n", prog_c);
	if (ret <= 0) {
	    warnp(__func__, "fprintf error when printing prog.c wordbuf_warning");
	}
	yorn = yes_or_no("Are you sure you want to proceed? [yn]");
	if (yorn == false) {
	    err(101, __func__, "please fix your prog.c file: %s", prog_c);
	    not_reached();
	}
	dbg(DBG_LOW, "user says that prog.c %s triggered a word buffer overflow is OK", prog_c);
	infop->wordbuf_warning = true;
	infop->warnings_ignored = true;
	if (answerp != NULL && i_flag_used == false) {
	    errno = 0;			/* pre-clear errno for errp() */
	    ret = fprintf(answerp, "y\n");
	    if (ret <= 0) {
		warnp(__func__, "fprintf error writing user confirmation that it's OK that prog.c triggered a word buffer overflow to answers file");
		(*answers_errors)++;
	    }
	}
    } else {
	infop->wordbuf_warning = false;
    }

    /*
     * sanity check on ungetc warning
     */
    if (size.ungetc_warning) {
	errno = 0;		/* pre-clear errno for errp() */
	ret = fprintf(stderr, "\nprog_c: %s triggered a triggered an ungetc error: @SirWumpus goofed\n"
			      "In order to avoid a possible Rule 2b violation, BE SURE TO CLEARLY MENTION THIS IN\n"
			      "YOUR remarks.md FILE!\n\n", prog_c);
	if (ret <= 0) {
	    warnp(__func__, "fprintf error when printing prog.c ungetc_warning");
	}
	yorn = yes_or_no("Are you sure you want to proceed? [yn]");
	if (yorn == false) {
	    err(102, __func__, "please fix your prog.c file: %s", prog_c);
	    not_reached();
	}
	dbg(DBG_LOW, "user says that prog.c %s triggered an ungetc warning OK", prog_c);
	infop->ungetc_warning = true;
	infop->warnings_ignored = true;
	if (answerp != NULL && i_flag_used == false) {
	    errno = 0;			/* pre-clear errno for errp() */
	    ret = fprintf(answerp, "y\n");
	    if (ret <= 0) {
		warnp(__func__, "fprintf error writing user confirmation that it's OK that prog.c triggered an ungetc warning to answers file");
		(*answers_errors)++;
	    }
	}
    } else {
	infop->ungetc_warning = false;
    }

    /*
     * inspect the Rule 2b size
     */
    if (infop->rule_2b_size > RULE_2B_SIZE) {
	ret = fprintf(stderr, "\nWARNING: The prog.c %s size: %lu > Rule 2b maximum: %lu\n", prog_c,
		      (unsigned long)infop->rule_2b_size, (unsigned long)RULE_2B_SIZE);
	fpara(stderr,
	      "Unless you are attempting some cleaver rule abuse, then we strongly suggest that you",
	      "tell us about your rule abuse in your remarks.md file.  Be sure you have read the",
	      "\"ABUSING THE RULES\" section of the guidelines.  And more importantly, read rule 12!",
	      "",
	      NULL);
	yorn = yes_or_no("Are you sure you want to submit such a large prog.c file? [yn]");
	if (yorn == false) {
	    err(103, __func__, "please fix your prog.c file: %s", prog_c);
	    not_reached();
	}
	dbg(DBG_LOW, "user says that their prog.c %s size: %lu > Rule 2B max size: %lu is OK", prog_c,
	    (unsigned long)infop->rule_2b_size, (unsigned long)RULE_2B_SIZE);
	infop->rule_2b_override = true;
	infop->warnings_ignored = true;
	if (answerp != NULL && i_flag_used == false) {
	    errno = 0;			/* pre-clear errno for errp() */
	    ret = fprintf(answerp, "y\n");
	    if (ret <= 0) {
		warnp(__func__, "fprintf error writing user confirmation that  prog.c size > Rule 2b max size is OK to answers file");
		(*answers_errors)++;
	    }
	}
    } else {
	infop->rule_2b_override = false;
    }

    /*
     * copy prog.c under entry_dir
     */
    errno = 0;			/* pre-clear errno for errp() */
    cmd = cmdprintf("% -- % %/prog.c", cp, prog_c, entry_dir);
    if (cmd == NULL) {
	err(104, __func__, "failed to cmdprintf: cp prog_c entry_dir/prog.c");
	not_reached();
    }
    dbg(DBG_HIGH, "about to perform: system(%s)", cmd);

    /*
     * pre-flush to avoid system() buffered stdio issues
     */
    clearerr(stdout);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fflush(stdout);
    if (ret < 0) {
	errp(105, __func__, "fflush(stdout) #2: error code: %d", ret);
	not_reached();
    }
    clearerr(stderr);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fflush(stderr);
    if (ret < 0) {
	errp(106, __func__, "fflush(stderr) #1: error code: %d", ret);
	not_reached();
    }

    /*
     * execute the cp command
     */
    errno = 0;			/* pre-clear errno for errp() */
    exit_code = system(cmd);
    if (exit_code < 0) {
	errp(107, __func__, "error calling system(%s)", cmd);
	not_reached();
    } else if (exit_code == 127) {
	errp(108, __func__, "execution of the shell failed for system(%s)", cmd);
	not_reached();
    } else if (exit_code != 0) {
	err(109, __func__, "%s failed with exit code: %d", cmd, WEXITSTATUS(exit_code));
	not_reached();
    }

    /*
     * save prog.c filename
     */
    errno = 0;			/* pre-clear errno for errp() */
    infop->prog_c = strdup("prog.c");
    if (infop->prog_c == NULL) {
	errp(110, __func__, "malloc #2 of %lu bytes failed", (unsigned long)(LITLEN("prog.c") + 1));
	not_reached();
    }

    /*
     * free storage
     */
    if (cmd != NULL) {
	free(cmd);
	cmd = NULL;
    }
    return;
}


/*
 * inspect_Makefile - inspect the rule contents of Makefile
 *
 * Determine if the 1st rule contains all.  Determine if there is a clean rule.
 * Determine if there is a clobber rule.  Determine if there is a try rule.
 *
 * NOTE: This is a simplistic Makefile line parser.  It is possible to
 *       fool the line scanner and to evade rule detection due to use of
 *       Makefile variables, line continuation, conditional Gnu-make controls, etc.
 *
 * given:
 *      stream  - Makefile opened as a stream
 *
 * returns:
 *      true ==> the rule set in Makefile is OK,
 *      false ==> the Makefile has an issue
 *
 * This function does not return on error.
 */
static bool
inspect_Makefile(char const *Makefile)
{
    FILE *stream;		/* open file stream */
    int ret;			/* libc function return */
    char *linep = NULL;		/* allocated line read from iocccsize */
    char *line;			/* Makefile line to parse */
    bool first_rule_is_all = false;	/* true ==> first rule set contains the all rule */
    bool found_all_rule = false;	/* true ==> found all rule */
    bool found_clean_rule = false;	/* true ==> found clean rule */
    bool found_clobber_rule = false;	/* true ==> found clobber rule */
    bool found_try_rule = false;	/* true ==> found try rule */
    int rulenum = 0;		/* current rule number */
    char *p;

    /*
     * firewall
     */
    if (Makefile == NULL) {
	err(111, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * open Makefile
     */
    errno = 0;			/* pre-clear errno for errp() */
    stream = fopen(Makefile, "r");
    if (stream == NULL) {
	errp(112, __func__, "cannot open Makefile: %s", Makefile);
	not_reached();
    }

    /*
     * process lines until EOF
     */
    do {

	/*
	 * process the next line
	 */
	line = readline_dup(&linep, true, NULL, stream);
	if (line == NULL) {
	    break;
	}

	/*
	 * free storage
	 */
	if (linep != NULL) {
	    free(linep);
	    linep = NULL;
	}

	/*
	 * trim off and comments
	 */
	p = strchr(line, '#');
	if (p != NULL) {
	    /*
	     * trim off comment
	     */
	    *p = '\0';
	}

	/*
	 * skip line if there is no :
	 */
	p = strchr(line, ':');
	if (p == NULL) {

	    /*
	     * free storage
	     */
	    if (line != NULL) {
		free(line);
		line = NULL;
	    }

	    /*
	     * non-: line
	     */
	    continue;

	/*
	 * we appear to have a make rule line
	 */
	} else {
	    /* trim off : and later dependency text */
	    *p = 0;
	}
	++rulenum;

	/*
	 * split the line into whitespace separated tokens
	 */
	for (p = strtok(line, " \t"); p != NULL; p = strtok(NULL, ":")) {

	    /*
	     * detect all rule
	     */
	    dbg(DBG_VHIGH, "rulenum[%d]: token: %s", rulenum, p);
	    if (found_all_rule == false && strcmp(p, "all") == 0) {
		/*
		 * first all rule found
		 */
		dbg(DBG_HIGH, "rulenum[%d]: all token found", rulenum);
		found_all_rule = true;
		if (rulenum == 1) {
		    /*
		     * all rule is in 1st rule line
		     */
		    first_rule_is_all = true;
		    break;
		}

	    /*
	     * detect clean rule
	     */
	    } else if (found_clean_rule == false && strcmp(p, "clean") == 0) {
		/*
		 * first clean rule found
		 */
		dbg(DBG_HIGH, "rulenum[%d]: clean token found", rulenum);
		found_clean_rule = true;

	    /*
	     * detect clobber rule
	     */
	    } else if (found_clobber_rule == false && strcmp(p, "clobber") == 0) {
		/*
		 * first clobber rule found
		 */
		dbg(DBG_HIGH, "rulenum[%d]: clobber token found", rulenum);
		found_clobber_rule = true;

	    /*
	     * detect try rule
	     */
	    } else if (found_try_rule == false && strcmp(p, "try") == 0) {
		/*
		 * first try rule found
		 */
		dbg(DBG_HIGH, "rulenum[%d]: try token found", rulenum);
		found_try_rule = true;
	    }
	}

	/*
	 * free storage
	 */
	if (line != NULL) {
	    free(line);
	    line = NULL;
	}

    } while (first_rule_is_all == false || found_all_rule == false || found_clean_rule == false ||
	     found_clobber_rule == false || found_try_rule == false);

    /*
     * close Makefile
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fclose(stream);
    if (ret < 0) {
	errp(113, __func__, "fclose error");
	not_reached();
    }

    /*
     * free storage
     */
    if (line != NULL) {
	free(line);
	line = NULL;
    }

    /*
     * if our parse of Makefile was successful
     */
    if (first_rule_is_all == true && found_all_rule == true && found_clean_rule == true &&
	found_clobber_rule == true && found_try_rule == true) {
	dbg(DBG_MED, "Makefile appears to pass");
	return true;
    }

    /*
     * report problem with Makefile
     */
    fpara(stderr,
	  "",
	  "There are problems with the Makefile provided:",
	  "",
	  NULL);
    if (first_rule_is_all == false) {
	fpara(stderr, "The all rule appears to not be the first (default) rule.",
	      "",
	      NULL);
    }
    if (found_all_rule == false) {
	fpara(stderr,
	      "  The Makefile appears to not have an all rule.",
	      "    The all rule should make your compiled/built program.",
	      "",
	      NULL);
    }
    if (found_clean_rule == false) {
	fpara(stderr,
	      "  The Makefile appears to not have a clean rule.",
	      "    The clean rule should remove any intermediate build files.",
	      "    For example, remove .o files and other intermediate build files .",
	      "    The clean rule should NOT remove compiled/built program built by the all rule.",
	      "",
	      NULL);
    }
    if (found_clobber_rule == false) {
	fpara(stderr,
	      "  The Makefile appears to not have a clobber rule.",
	      "    The clobber rule should restore the directory to the original submission state.",
	      "    The clobber role should depend on the clean rule, it could remove the entry's program,",
	      "    clean up after program execution (if needed), and restore the entire directory back",
	      "    to the original submission state.",
	      "",
	      NULL);
    }
    if (found_try_rule == false) {
	fpara(stderr,
	      "  The Makefile appears to not have a try rule.",
	      "    The try rule should execute the program with suggested arguments (if any needed).",
	      "    The program may be executed more than once if such examples are informative.",
	      "	   The try rule should depend on the all rule.",
	      "",
	      NULL);
    }
    return false;
}


/*
 * check_Makefile - check Makefile arg and if OK, copy into entry_dir/Makefile
 *
 * Check if the Makefile argument is a readable file, and
 * if it has the proper rules (starting with all:),
 * use cp to copy into entry_dir/Makefile.
 *
 * given:
 *      infop           - pointer to info structure
 *      entry_dir       - newly created entry directory (by mk_entry_dir()) under work_dir
 *      cp              - cp utility path
 *      Makefile        - Makefile arg: given path to Makefile
 *      answerp		- answer file
 *      i_flag_used	- if -i was used
 *      answers_errors  - > 0 I/O errors on answers file
 *
 * This function does not return on error.
 */
static void
check_Makefile(struct info *infop, char const *entry_dir, char const *cp, char const *Makefile, FILE *answerp, bool i_flag_used,
	unsigned *answers_errors)
{
    ssize_t filesize = 0;	/* size of Makefile */
    int ret;			/* libc function return */
    char *cmd = NULL;		/* cp prog_c entry_dir/prog.c */
    int exit_code;		/* exit code from system(cmd) */
    bool yorn = false;		/* response to a question */

    /*
     * firewall
     */
    if (infop == NULL || entry_dir == NULL || cp == NULL || Makefile == NULL) {
	err(114, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * Makefile must be a non-empty readable file
     */
    if (!exists(Makefile)) {
	fpara(stderr,
	      "",
	      "We cannot find the prog.c file.",
	      "",
	      NULL);
	err(115, __func__, "Makefile does not exist: %s", Makefile);
	not_reached();
    }
    if (!is_file(Makefile)) {
	fpara(stderr,
	       "",
	       "The Makefile path, while it exists, is not a file.",
	       "",
	       NULL);
	err(116, __func__, "Makefile is not a file: %s", Makefile);
	not_reached();
    }
    if (!is_read(Makefile)) {
	fpara(stderr,
	      "",
	      "The Makefile, while it is a file, is not readable.",
	      "",
	      NULL);
	err(117, __func__, "Makefile is not readable file: %s", Makefile);
	not_reached();
    }
    filesize = file_size(Makefile);
    if (filesize < 0) {
	err(118, __func__, "file_size error: %ld on Makefile  %s", (long)filesize, Makefile);
	not_reached();
    } else if (filesize == 0) {
	err(119, __func__, "Makefile cannot be empty: %s", Makefile);
	not_reached();
    }

    /*
     * scan Makefile for critical rules
     */
    if (inspect_Makefile(Makefile) == false) {

	/*
	 * Explain again what is needed in a Makefile
	 */
	fpara(stderr,
	      "Makefiles must have the following Makefile rules:",
	      "",
	      "    all - compile the entry, must be the 1st entry",
	      "    clean - remove intermediate compilation files",
	      "    clobber - clean, remove compiled entry, restore to the original entry state",
	      "    try - invoke the entry at least once",
	      "",
	      "While this program's parser may have missed finding those Makefile rules,",
	      "chances are this file is not a proper Makefile under the IOCCC rules:",
	      "",
	      NULL);
	errno = 0;		/* pre-clear errno for errp() */
	ret = fprintf(stderr, "    %s\n\n", Makefile);
	if (ret <= 0) {
	    warnp(__func__, "fprintf for Makefile error: %d", ret);
	}

	/*
	 * Ask if they want to submit it anyway
	 */
	yorn = yes_or_no("Do you still want to submit this Makefile in the hopes that it is OK? [yn]");
	if (yorn == false) {
	    err(120, __func__, "Use a different Makefile or modify this file: %s", Makefile);
	    not_reached();
	}
	infop->Makefile_override = true;
	infop->warnings_ignored = true;
	if (answerp != NULL && i_flag_used == false) {
	    errno = 0;			/* pre-clear errno for errp() */
	    ret = fprintf(answerp, "y\n");
	    if (ret <= 0) {
		warnp(__func__, "fprintf error writing user confirmation that Makefile is OK to answers file");
		(*answers_errors)++;
	    }
	}
    } else {
	infop->Makefile_override = false;
    }

    /*
     * copy Makefile under entry_dir
     */
    errno = 0;			/* pre-clear errno for errp() */
    cmd = cmdprintf("% -- % %/Makefile", cp, Makefile, entry_dir);
    if (cmd == NULL) {
	err(121, __func__, "failed to cmdprintf: cp Makefile entry_dir/Makefile");
	not_reached();
    }
    dbg(DBG_HIGH, "about to perform: system(%s)", cmd);

    /*
     * pre-flush to avoid system() buffered stdio issues
     */
    clearerr(stdout);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fflush(stdout);
    if (ret < 0) {
	errp(122, __func__, "fflush(stdout) error code: %d", ret);
	not_reached();
    }
    clearerr(stderr);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fflush(stderr);
    if (ret < 0) {
	errp(123, __func__, "fflush(stderr) #1: error code: %d", ret);
	not_reached();
    }

    /*
     * execute the cp command
     */
    errno = 0;			/* pre-clear errno for errp() */
    exit_code = system(cmd);
    if (exit_code < 0) {
	errp(124, __func__, "error calling system(%s)", cmd);
	not_reached();
    } else if (exit_code == 127) {
	errp(125, __func__, "execution of the shell failed for system(%s)", cmd);
	not_reached();
    } else if (exit_code != 0) {
	err(126, __func__, "%s failed with exit code: %d", cmd, WEXITSTATUS(exit_code));
	not_reached();
    }

    /*
     * save Makefile filename
     */
    errno = 0;			/* pre-clear errno for errp() */
    infop->Makefile = strdup("Makefile");
    if (infop->Makefile == NULL) {
	errp(128, __func__, "malloc #1 of %lu bytes failed", (unsigned long)(LITLEN("Makefile") + 1));
	not_reached();
    }

    /*
     * free storage
     */
    if (cmd != NULL) {
	free(cmd);
	cmd = NULL;
    }
    return;
}


/*
 * check_remarks_md - check remarks_md arg and if OK, copy into entry_dir/Makefile
 *
 * Check if the remarks_md argument is a readable file, and
 * if it is not empty,
 * use cp to copy into entry_dir/remarks.md.
 *
 * given:
 *      infop           - pointer to info structure
 *      entry_dir       - the newly created entry directory (by mk_entry_dir()) under work_dir
 *      cp              - cp utility path
 *      remarks_md      - remarks_md arg: given path to author's remarks markdown file
 *
 * This function does not return on error.
 */
static void
check_remarks_md(struct info *infop, char const *entry_dir, char const *cp, char const *remarks_md)
{
    ssize_t filesize = 0;	/* size of remarks.md */
    char *cmd = NULL;		/* cp prog_c entry_dir/prog.c */
    int exit_code;		/* exit code from system(cmd) */
    int ret;			/* libc function return */

    /*
     * firewall
     */
    if (infop == NULL || entry_dir == NULL || cp == NULL || remarks_md == NULL) {
	err(129, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * remarks_md must be a non-empty readable file
     */
    if (!exists(remarks_md)) {
	fpara(stderr,
	       "",
	       "We cannot find the remarks.md file.",
	       "",
	       NULL);
	err(130, __func__, "remarks.md does not exist: %s", remarks_md);
	not_reached();
    }
    if (!is_file(remarks_md)) {
	fpara(stderr, "",
	      "The remarks.md path, while it exists, is not a file.",
	      "",
	      NULL);
	err(131, __func__, "remarks_md is not a file: %s", remarks_md);
	not_reached();
    }
    if (!is_read(remarks_md)) {
	fpara(stderr,
	      "",
	      "The remarks.md, while it is a file, is not readable.",
	      "",
	      NULL);
	err(132, __func__, "remarks_md is not readable file: %s", remarks_md);
	not_reached();
    }
    filesize = file_size(remarks_md);
    if (filesize < 0) {
	err(133, __func__, "file_size error: %ld on remarks_md %s", (long)filesize, remarks_md);
	not_reached();
    } else if (filesize == 0) {
	err(134, __func__, "remarks.md cannot be empty: %s", remarks_md);
	not_reached();
    }

    /*
     * copy remarks.md under entry_dir
     */
    errno = 0;			/* pre-clear errno for errp() */
    cmd = cmdprintf("% -- % %/remarks.md", cp, remarks_md, entry_dir);
    if (cmd == NULL) {
	err(135, __func__, "failed to cmdprintf: cp remarks.md entry_dir/remarks.md");
	not_reached();
    }
    dbg(DBG_HIGH, "about to perform: system(%s)", cmd);

    /*
     * pre-flush to avoid system() buffered stdio issues
     */
    clearerr(stdout);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fflush(stdout);
    if (ret < 0) {
	errp(136, __func__, "fflush(stdout) error code: %d", ret);
	not_reached();
    }
    clearerr(stderr);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fflush(stderr);
    if (ret < 0) {
	errp(137, __func__, "fflush(stderr) #1: error code: %d", ret);
	not_reached();
    }

    /*
     * execute the cp command
     */
    errno = 0;			/* pre-clear errno for errp() */
    exit_code = system(cmd);
    if (exit_code < 0) {
	errp(138, __func__, "error calling system(%s)", cmd);
	not_reached();
    } else if (exit_code == 127) {
	errp(139, __func__, "execution of the shell failed for system(%s)", cmd);
	not_reached();
    } else if (exit_code != 0) {
	err(140, __func__, "%s failed with exit code: %d", cmd, WEXITSTATUS(exit_code));
	not_reached();
    }

    /*
     * save remarks_md filename
     */
    errno = 0;			/* pre-clear errno for errp() */
    infop->remarks_md = strdup("remarks.md");
    if (infop->remarks_md == NULL) {
	errp(141, __func__, "malloc #1 of %lu bytes failed", (unsigned long)(LITLEN("remarks.md") + 1));
	not_reached();
    }

    /*
     * free storage
     */
    if (cmd != NULL) {
	free(cmd);
	cmd = NULL;
    }
    return;
}


/*
 * base_name - determine the final portion of a path
 *
 * given:
 *      path    - path to form the basename from
 *
 * returns:
 *      malloced basename
 *
 * This function does not return on error.
 */
static char *
base_name(char const *path)
{
    size_t len;			/* length of path */
    char *copy;			/* copy of path to work from and maybe return */
    char *ret;			/* allocated string to return */
    char *p;
    size_t i;

    /*
     * firewall
     */
    if (path == NULL) {
	err(142, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * duplicate the path for basename processing
     */
    errno = 0;			/* pre-clear errno for errp() */
    copy = strdup(path);
    if (copy == NULL) {
	errp(143, __func__, "strdup(%s) failed", path);
	not_reached();
    }

    /*
     * case: basename of empty string is an empty string
     */
    len = strlen(copy);
    if (len == 0) {
	dbg(DBG_VVHIGH, "#0: basename of path:%s is an empty string", path);
	return copy;
    }

    /*
     * remove any multiple trailing /'s
     */
    for (i = len - 1; i > 0; --i) {
	if (copy[i] == '/') {
	    /* trim the trailing / */
	    copy[i] = '\0';
	} else {
	    /* last character (now) is not / */
	    break;
	}
    }
    /*
     * now copy has no trailing /'s, unless it is just /
     */

    /*
     * case: basename of / is /
     */
    if (strcmp(copy, "/") == 0) {
	/*
	 * path is just /, so return /
	 */
	dbg(DBG_VVHIGH, "#1: basename(%s) == %s", path, copy);
	return copy;
    }

    /*
     * look for the last /
     */
    p = strrchr(copy, '/');
    if (p == NULL) {
	/*
	 * path is just a filename, return that filename
	 */
	dbg(DBG_VVHIGH, "#2: basename(%s) == %s", path, copy);
	return copy;
    }

    /*
     * duplicate the new string to return
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = strdup(p + 1);
    if (ret == NULL) {
	errp(144, __func__, "strdup(%s) failed", p + 1);
	not_reached();
    }

    /*
     * free storage
     */
    if (copy != NULL) {
	free(copy);
	copy = NULL;
    }

    /*
     * return beyond the last /
     */
    dbg(DBG_VVHIGH, "#3: basename(%s) == %s", path, ret);
    return ret;
}


/*
 * check_extra_data_files - check extra data files args and if OK, copy into entry_dir/Makefile
 *
 * Check if the check extra data files are readable, and
 * use cp to copy into entry_dir/remarks.md.
 *
 * given:
 *      infop           - pointer to info structure
 *      entry_dir       - newly created entry directory (by mk_entry_dir()) under work_dir
 *      cp              - cp utility path
 *      count           - number of extra data files arguments
 *      args            - pointer to an array of strings starting with 1st extra data file
 *
 * This function does not return on error.
 */
static void
check_extra_data_files(struct info *infop, char const *entry_dir, char const *cp, int count, char **args)
{
    char *base;			/* basename of extra data file */
    char *dest;			/* destination path of an extra data file */
    size_t base_len;		/* length of the basename of the data file */
    size_t dest_len;		/* length of the extra data file path */
    size_t entry_dir_len;	/* length of the entry_dir path */
    char *cmd = NULL;		/* cp prog_c entry_dir/prog.c */
    int exit_code;		/* exit code from system(cmd) */
    int ret;			/* libc function return */
    int i;
    size_t j;

    /*
     * firewall
     */
    if (infop == NULL || entry_dir == NULL || cp == NULL || args == NULL) {
	err(145, __func__, "called with NULL arg(s)");
	not_reached();
    }
    if (count < 0) {
	err(146, __func__, "count :%d < 0", count);
	not_reached();
    }

    /*
     * save extra data file count
     */
    infop->extra_count = count;

    /*
     * allocate extra file arrays
     */
    errno = 0;			/* pre-clear errno for errp() */
    /* + 1 for trailing NULL */
    infop->extra_file = (char **)calloc((size_t)(count + 1), sizeof(char *));
    if (infop->extra_file == NULL) {
	errp(147, __func__, "calloc #0 of %d char * pointers failed", count + 1);
	not_reached();
    }

    /*
     * process all of the extra args
     */
    entry_dir_len = strlen(entry_dir);
    for (i = 0; i < count && args[i] != NULL; ++i) {

	/*
	 * extra data file must be a readable file
	 */
	dbg(DBG_HIGH, "processing extra data file %d: %s", i, args[i]);
	if (!exists(args[i])) {
	    fpara(stderr,
		  "",
		  "We cannot find an extra data file.",
		  "",
		  NULL);
	    err(148, __func__, "extra[%i] does not exist: %s", i, args[i]);
	    not_reached();
	}
	if (!is_file(args[i])) {
	    fpara(stderr,
		   "",
		   "The remarks.md path, while it exists, is not a file.",
		   "",
		   NULL);
	    err(149, __func__, "extra[%i] is not a file: %s", i, args[i]);
	    not_reached();
	}
	if (!is_read(args[i])) {
	    fpara(stderr,
		  "",
		  "The remarks.md, while it is a file, is not readable.",
		  "",
		  NULL);
	    err(150, __func__, "extra[%i] is not readable file: %s", i, args[i]);
	    not_reached();
	}

	/*
	 * basename cannot be too long
	 */
	base = base_name(args[i]);
	dbg(DBG_VHIGH, "basename(%s): %s", args[i], base);
	base_len = strlen(base);
	if (base_len == 0) {
	    err(151, __func__, "basename of extra data file: %s has a length of 0", args[i]);
	    not_reached();
	} else if (base_len > MAX_BASENAME_LEN) {
	    fpara(stderr,
		  "",
		  "The basename of an extra file is too long.",
		  "",
		  NULL);
	    err(152, __func__, "basename of extra data file: %s is %lu characters an is > the limit: %lu",
			       args[i], (unsigned long)base_len, (unsigned long)MAX_BASENAME_LEN);
	    not_reached();
	}

	/*
	 * basename cannot begin with . nor - nor +
	 */
	if (base[0] == '.' || base[0] == '-' || base[0] == '+') {
	    fpara(stderr,
		  "",
		  "The first character is the basename of an extra file cannot be . nor - nor +",
		  "",
		  NULL);
	    err(153, __func__, "basename of extra data file: %s start with in invalid character: %s", args[i], base);
	    not_reached();
	}

	/*
	 * basename must only use POSIX Fully portable characters: A-Z a-z 0-9 . _ - and the + character
	 */
	for (j=0; j < base_len; ++j) {
	    if (!isascii(base[j]) ||
	        (!isalnum(base[j]) && base[j] != '.' && base[j] != '_' && base[j] != '-' && base[j] != '+')) {
		fpara(stderr,
		      "",
		      "The basename of an extra file can only consist of POSIX Fully portable characters and +:",
		      "",
		      "    A-Z a-z 0-9 . _ - +",
		      "",
		      NULL);
		err(154, __func__, "basename of %s character %lu is NOT a POSIX Fully portable character nor +",
				   args[i], (unsigned long)j);
		not_reached();
	    }
	}

	/*
	 * form destination path
	 */
	infop->extra_file[i] = base;
	dest_len = entry_dir_len + 1 + base_len + 1;
	errno = 0;		/* pre-clear errno for errp() */
	dest = (char *)malloc(dest_len + 1);
	if (dest == NULL) {
	    errp(155, __func__, "malloc #0 of %lu bytes failed", (unsigned long)(dest_len + 1));
	    not_reached();
	}
	ret = snprintf(dest, dest_len, "%s/%s", entry_dir, base);
	if (ret <= 0) {
	    errp(156, __func__, "snprintf #0 error: %d", ret);
	    not_reached();
	}
	dbg(DBG_VHIGH, "destination path: %s", dest);

	/*
	 * destination cannot exist
	 */
	if (exists(dest)) {
	    fpara(stderr,
		  "",
		  "extra data files cannot overwrite other files.",
		  "",
		  NULL);
	    err(157, __func__, "for extra file: %s destination already exists: %s", args[i], dest);
	    not_reached();
	}

	/*
	 * copy remarks_md under entry_dir
	 */
	cmd = cmdprintf("% -- % %", cp, args[i], dest);
	if (cmd == NULL) {
	    err(158, __func__, "failed to cmdprintf: cp extra_file[%d]: %s dest: %s", i, args[i], dest);
	    not_reached();
	}
	dbg(DBG_HIGH, "about to perform: system(%s)", cmd);

	/*
	 * pre-flush to avoid system() buffered stdio issues
	 */
	clearerr(stdout);	/* pre-clear ferror() status */
	errno = 0;		/* pre-clear errno for errp() */
	ret = fflush(stdout);
	if (ret < 0) {
	    errp(159, __func__, "fflush(stdout) error code: %d", ret);
	    not_reached();
	}
	clearerr(stderr);		/* pre-clear ferror() status */
	errno = 0;			/* pre-clear errno for errp() */
	ret = fflush(stderr);
	if (ret < 0) {
	    errp(160, __func__, "fflush(stderr) #1: error code: %d", ret);
	    not_reached();
	}

	/*
	 * execute the cp command
	 */
	errno = 0;		/* pre-clear errno for errp() */
	exit_code = system(cmd);
	if (exit_code < 0) {
	    errp(161, __func__, "error calling system(%s)", cmd);
	    not_reached();
	} else if (exit_code == 127) {
	    errp(162, __func__, "execution of the shell failed for system(%s)", cmd);
	    not_reached();
	} else if (exit_code != 0) {
	    err(163, __func__, "%s failed with exit code: %d", cmd, WEXITSTATUS(exit_code));
	    not_reached();
	}

	/*
	 * free storage
	 */
	if (dest != NULL) {
	    free(dest);
	    dest = NULL;
	}
	if (cmd != NULL) {
	    free(cmd);
	    cmd = NULL;
	}
    }
    infop->extra_file[i] = NULL;
    return;
}


/*
 * lookup_location_name - convert a ISO 3166-1 Alpha-2 into a location name
 *
 * given:
 *      upper_code      - ISO 3166-1 Alpha-2 in UPPER CASE
 *
 * return:
 *      location name or NULL ==> unlisted code
 *
 * This function does not return on error.
 */
static char const *
lookup_location_name(char const *upper_code)
{
    struct location *p;		/* entry in the location table */

    /*
     * firewall
     */
    if (upper_code == NULL) {
	err(164, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * search location table for the code
     */
    for (p = &loc[0]; p->code != NULL && p->name != NULL; ++p) {
	if (strcmp(upper_code, p->code) == 0) {
	    dbg(DBG_VHIGH, "code %s name found: %s", p->code, p->name);
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
 *      question        - string to prompt for a question
 *
 * returns:
 *      true ==> input is yes in some form,
 *      false ==> input is not yes
 */
static bool
yes_or_no(char const *question)
{
    char *response;		/* response to the question */
    char *p;

    /*
     * firewall
     */
    if (question == NULL) {
	err(165, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * ask the question and obtain the response
     */
    do {
	response = prompt(question, NULL);

	/*
	 * convert response to lower case
	 */
	for (p = response; *p != '\0'; ++p) {
	    if (isascii(*p) && isalpha(*p)) {
		*p = (char)tolower(*p);
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
	fpara(stderr,
	      "Please enter either y (yes) or n (no)",
	      NULL);

	/*
	 * free storage
	 */
	if (response != NULL) {
	    free(response);
	    response = NULL;
	}

    } while (response == NULL);

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
 * get_title - get the title of the entry
 *
 * Ask the user for an entry title, validate the response
 * and return the malloced title.
 *
 * given:
 *      infop   - pointer to info structure
 *
 * returns:
 *      malloced and validated title
 *
 * This function does not return on error.
 */
static char *
get_title(struct info *infop)
{
    char *title = NULL;		/* entry title to return or NULL */
    size_t len;			/* length of title */
    size_t span;		/* span of valid characters in title */
    int ret;			/* libc function return */

    /*
     * firewall
     */
    if (infop == NULL) {
	err(166, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * inform the user of the title
     */
    para("An entry title is a short name using the [a-z0-9][a-z0-9_+-]* regex pattern.",
	  "",
	  "If your entry wins, the title might become the directory name of your entry.",
	  "Although the IOCCC judges might change the title for various reason.",
	  "",
	  "If you submitting more than one entry, please make your titles unique",
	  "amongst the entries that you submit to the current IOCCC.",
	  "",
	  NULL);
    errno = 0;		/* pre-clear errno for errp() */
    ret = fprintf(stderr, "You title must be between 1 and %d ASCII characters long.\n\n", MAX_TITLE_LEN);
    if (ret <= 0) {
	warnp(__func__, "fprintf #0 error: %d", ret);
    }

    /*
     * ask the question and obtain the response
     */
    do {

	/*
	 * obtain the reply
	 */
	title = prompt("Enter a title for your entry", NULL);

	/*
	 * title cannot be empty
	 */
	len = strlen(title);
	if (len == 0) {

	    /*
	     * reject empty title
	     */
	    fpara(stderr,
		  "",
		  "The title cannot be an empty string.",
		  "",
		  NULL);

	    /*
	     * free storage
	     */
	    if (title != NULL) {
		free(title);
		title = NULL;
	    }
	    continue;

	/*
	 * title cannot be too long
	 */
	} else if (len > MAX_TITLE_LEN) {

	    /*
	     * reject long title
	     */
	    fpara(stderr,
		  "",
		  "That title is too long.",
		  "",
		  NULL);
	    errno = 0;		/* pre-clear errno for errp() */
	    ret = fprintf(stderr, "You title must be between 1 and %d ASCII characters long.\n\n", MAX_TITLE_LEN);
	    if (ret <= 0) {
		warnp(__func__, "fprintf #1 error: %d", ret);
	    }

	    /*
	     * free storage
	     */
	    if (title != NULL) {
		free(title);
		title = NULL;
	    }
	    continue;
	}

	/*
	 * verify that the title starts with [a-z0-9]
	 */
	if (!isascii(title[0]) || (!islower(title[0]) && !isdigit(title[0]))) {
	    /*
	     * reject long title
	     */
	    fpara(stderr,
		  "",
		  "That title does not start with a lower case ASCII letter [a-z] or digit [0-9]:",
		  "",
		  NULL);

	    /*
	     * free storage
	     */
	    if (title != NULL) {
		free(title);
		title = NULL;
	    }
	    continue;
	}

	/*
	 * verify that the title characters are from the valid character set
	 */
	span = strspn(title, TAIL_TITLE_CHARS);
	if (span != len) {

	    /*
	     * reject long title
	     */
	    fpara(stderr,
		  "",
		  "Your title contains invalid characters.  A title must match the following regex:",
		  "",
		  "    [a-z0-9][a-z0-9-]*",
		  "",
		  "That is, it must start with a lower case letter ASCII [a-z] or digit [0-9]",
		  "followed by zero or more lower case letters ASCII [a-z], digits [0-9],",
		  "- (ASCII dash), + (ASCII plus), or _ (ASCII underscore).",
		  "",
		  NULL);

	    /*
	     * free storage
	     */
	    if (title != NULL) {
		free(title);
		title = NULL;
	    }
	    continue;
	}
    } while (title == NULL);

    /*
     * returned malloced title
     */
    return title;
}


/*
 * get_abstract - get the abstract of the entry
 *
 * Ask the user for an entry abstract, validate the response
 * and return the malloced abstract.
 *
 * given:
 *      infop           - pointer to info structure
 *
 * returns:
 *      malloced and validated abstract
 *
 * This function does not return on error.
 */
static char *
get_abstract(struct info *infop)
{
    char *abstract = NULL;	/* entry abstract to return or NULL */
    size_t len;			/* length of abstract */
    int ret;			/* libc function return */

    /*
     * firewall
     */
    if (infop == NULL) {
	err(167, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * inform the user of the abstract
     */
    para("",
	 "An entry abstract is 1-line summary of your entry.",
	 "",
	 NULL);

    /*
     * ask the question and obtain the response
     */
    do {

	/*
	 * obtain the reply
	 */
	abstract = prompt("Enter a 1-line abstract of your entry", NULL);

	/*
	 * abstract cannot be empty
	 */
	len = strlen(abstract);
	if (len == 0) {

	    /*
	     * reject empty abstract
	     */
	    fpara(stderr,
		  "",
		  "The abstract cannot be an empty string.",
		  "",
		  NULL);

	    /*
	     * free storage
	     */
	    if (abstract != NULL) {
		free(abstract);
		abstract = NULL;
	    }
	    continue;

	/*
	 * abstract cannot be too long
	 */
	} else if (len > MAX_ABSTRACT_LEN) {

	    /*
	     * reject long abstract
	     */
	    fpara(stderr,
		  "",
		  "That abstract is too long.",
		  "",
		  NULL);
	    errno = 0;		/* pre-clear errno for errp() */
	    ret = fprintf(stderr, "You abstract must be between 1 and %d characters long.\n\n", MAX_ABSTRACT_LEN);
	    if (ret <= 0) {
		warnp(__func__, "fprintf error: %d", ret);
	    }

	    /*
	     * free storage
	     */
	    if (abstract != NULL) {
		free(abstract);
		abstract = NULL;
	    }
	    continue;
	}
    } while (abstract == NULL);

    /*
     * returned malloced abstract
     */
    return abstract;
}


/*
 * get_author_info - obtain information on entry authors
 *
 * given:
 *      infop           - pointer to info structure
 *      ioccc_id        - IOCCC entry ID or test
 *      entry_num       - entry number
 *      author_set      - pointer to array of authors
 *      answerp		- answers file (for updating entries) or NULL
 *      i_flag_used	- if -i was used
 *      answers_errors	- > 0 I/O errors on answers file
 *
 * returns:
 *      number of authors
 *
 * This function does not return on error.
 */
static int
get_author_info(struct info *infop, char *ioccc_id, struct author **author_set_p, FILE *answerp, bool i_flag_used, unsigned *answers_errors)
{
    struct author *author_set = NULL;	/* allocated author set */
    int author_count = -1;		/* number of authors or -1 */
    char *author_count_str = NULL;	/* author count string */
    bool yorn = false;		/* response to a question */
    size_t len;			/* length of reply */
    int ret;			/* libc function return */
    char guard;			/* scanf guard to catch excess amount of input */
    char *p;
    int i;

    /*
     * firewall
     */
    if (infop == NULL || ioccc_id == NULL || author_set_p == NULL) {
	err(168, __func__, "called with NULL arg(s)");
	not_reached();
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
	ret = sscanf(author_count_str, "%d%c", &author_count, &guard);
	if (ret != 1 || author_count < 1 || author_count > MAX_AUTHORS) {
	    errno = 0;		/* pre-clear errno for errp() */
	    ret = fprintf(stderr, "\nThe number of authors must a number from 1 thru %d, please re-enter.\n", MAX_AUTHORS);
	    if (ret <= 0) {
		warnp(__func__, "fprintf error #0 while printing author number range");
	    }
	    errno = 0;		/* pre-clear errno for errp() */
	    ret = fprintf(stderr, "If you happen to have more than %d authors. we ask that you pick the\n", MAX_AUTHORS);
	    if (ret <= 0) {
		warnp(__func__, "fprintf error #1 while printing author number range");
	    }
	    errno = 0;		/* pre-clear errno for errp() */
	    ret = fprintf(stderr, "just %d authors and mention the remainder of the authors in the remarks file.\n", MAX_AUTHORS);
	    if (ret <= 0) {
		warnp(__func__, "fprintf error #2 while printing author number range");
	    }
	    author_count = -1;	/* invalidate input */
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
    if (answerp != NULL && i_flag_used == false) {
	errno = 0;			/* pre-clear errno for errp() */
        ret = fprintf(answerp, "%d\n", author_count);
	if (ret <= 0) {
	    warnp(__func__, "fprintf error printing IOCCC contest id to the answers file");
	    (*answers_errors)++;
	}
    }

    /*
     * allocate the author array
     */
    errno = 0;			/* pre-clear errno for errp() */
    author_set = (struct author *) malloc(sizeof(struct author) * (size_t)author_count);
    if (author_set == NULL) {
	errp(169, __func__, "malloc a struct author array of length: %d failed", author_count);
	not_reached();
    }

    /*
     * pre-zeroize the author array
     */
    memset(author_set, 0, sizeof(struct author) * (size_t)author_count);

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
	 "",
	 NULL);
    errno = 0;		/* pre-clear errno for errp() */
    ret = puts(ISO_3166_1_CODE_URL0);
    if (ret < 0) {
	warnp(__func__, "puts error printing ISO 3166-1 URL0");
    }
    errno = 0;		/* pre-clear errno for errp() */
    ret = puts(ISO_3166_1_CODE_URL1);
    if (ret < 0) {
	warnp(__func__, "puts error printing ISO 3166-1 URL1");
    }
    errno = 0;		/* pre-clear errno for errp() */
    ret = puts(ISO_3166_1_CODE_URL2);
    if (ret < 0) {
	warnp(__func__, "puts error printing ISO 3166-1 URL2");
    }
    errno = 0;		/* pre-clear errno for errp() */
    ret = puts(ISO_3166_1_CODE_URL3);
    if (ret < 0) {
	warnp(__func__, "puts error printing ISO 3166-1 URL3");
    }
    errno = 0;		/* pre-clear errno for errp() */
    ret = puts(ISO_3166_1_CODE_URL4);
    if (ret < 0) {
	warnp(__func__, "puts error printing ISO 3166-1 URL4");
    }
    para("",
	 "We will ask for the author(s) Email address. Press return if you don't want to provide it, or if don't have one.",
	 "We will ask for a home URL (starting with http:// or https://), or press return to skip, or if don't have one.",
	 "We will ask a twitter handle (must start with @), or press return to skip, or if don't have one.",
	 "We will ask a GitHub account (must start with @), or press return to skip, or if don't have one.",
	 "We will ask for an affiliation (company, school, org) of the author, or press return to skip, or if don't have one.",
	 NULL);

    /*
     * collect information on authors
     */
    for (i = 0; i < author_count; ++i) {

	/*
	 * announce author number
	 */
	errno = 0;		/* pre-clear errno for errp() */
	ret = printf("\nEnter information for author #%d\n\n", i);
	if (ret <= 0) {
	    warnp(__func__, "printf error printing author number");
	}
	author_set[i].author_num = i;

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
		errno = 0;		/* pre-clear errno for errp() */
		ret = fprintf(stderr, "\nSorry ( tm Canada :-) ), we limit names to %d characters\n\n", MAX_NAME_LEN);
		if (ret <= 0) {
		    warnp(__func__, "fprintf error while reject name that is too long");
		}

		/*
		 * free storage
		 */
		if (author_set[i].name != NULL) {
		    free(author_set[i].name);
		    author_set[i].name = NULL;
		}
	    }
	} while (author_set[i].name == NULL);
	dbg(DBG_MED, "Author #%d Name %s", i, author_set[i].name);


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
		!isascii(author_set[i].location_code[0]) || !isalpha(author_set[i].location_code[0]) ||
		!isascii(author_set[i].location_code[1]) || !isalpha(author_set[i].location_code[1])) {

		/*
		 * provide more help on location/country codes
		 */
		fpara(stderr,
		      "",
		      "Location/country codes are two letters.",
		      "",
		      "For ISO 3166-1 2 character codes, see: the Alpha-2 code column of:",
		      "",
		      NULL);
		errno = 0;		/* pre-clear errno for errp() */
		ret = fprintf(stderr, "    %s\n\n", ISO_3166_1_CODE_URL0);
		if (ret <= 0) {
		    warnp(__func__, "fprintf while printing ISO 3166-1 CODE URL #0");
		}
		fpara(stderr,
		      "or from these Wikipedia / ISO web pages:",
		      "",
		      NULL);
		errno = 0;		/* pre-clear errno for errp() */
		ret = fprintf(stderr, "    %s\n", ISO_3166_1_CODE_URL1);
		if (ret <= 0) {
		    warnp(__func__, "fprintf while printing ISO 3166-1 CODE URL #1");
		}
		errno = 0;		/* pre-clear errno for errp() */
		ret = fprintf(stderr, "    %s\n", ISO_3166_1_CODE_URL2);
		if (ret <= 0) {
		    warnp(__func__, "fprintf while printing ISO 3166-1 CODE URL #2");
		}
		errno = 0;		/* pre-clear errno for errp() */
		ret = fprintf(stderr, "    %s\n\n", ISO_3166_1_CODE_URL3);
		if (ret <= 0) {
		    warnp(__func__, "fprintf while printing ISO 3166-1 CODE URL #3");
		}

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
		author_set[i].location_name = NULL;
		yorn = false;
		continue;

	    } else {

		/*
		 * valid location/country code - convert to upper case
		 */
		author_set[i].location_code[0] = (char)toupper(author_set[i].location_code[0]);
		author_set[i].location_code[1] = (char)toupper(author_set[i].location_code[1]);

		/*
		 * determine if code is known
		 */
		author_set[i].location_name = lookup_location_name(author_set[i].location_code);
		if (author_set[i].location_name == NULL) {

		    /*
		     * provide more help on location/country codes
		     */
		    fpara(stderr,
			  "",
			  "That is not a known location/country code.",
			  "",
			  "For ISO 3166-1 2 character codes, see: the Alpha-2 code column of:",
			  "",
			  NULL);
		    errno = 0;		/* pre-clear errno for errp() */
		    ret = fprintf(stderr, "    %s\n\n", ISO_3166_1_CODE_URL0);
		    if (ret <= 0) {
			warnp(__func__, "fprintf when printing ISO 3166-1 CODE URL #0");
		    }
		    fpara(stderr,
			  "or from these Wikipedia / ISO web pages:",
			  "",
			  NULL);
		    errno = 0;		/* pre-clear errno for errp() */
		    ret = fprintf(stderr, "    %s\n", ISO_3166_1_CODE_URL1);
		    if (ret <= 0) {
			warnp(__func__, "fprintf when printing ISO 3166-1 CODE URL #1");
		    }
		    errno = 0;		/* pre-clear errno for errp() */
		    ret = fprintf(stderr, "    %s\n", ISO_3166_1_CODE_URL2);
		    if (ret <= 0) {
			warnp(__func__, "fprintf when printing ISO 3166-1 CODE URL #2");
		    }
		    errno = 0;		/* pre-clear errno for errp() */
		    ret = fprintf(stderr, "    %s\n\n", ISO_3166_1_CODE_URL3);
		    if (ret <= 0) {
			warnp(__func__, "fprintf when printing ISO 3166-1 CODE URL #3");
		    }

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
		    author_set[i].location_name = NULL;
		    yorn = false;
		    continue;
		}

		/*
		 * verify the known location/country code
		 */
		errno = 0;		/* pre-clear errno for errp() */
		ret = printf("The location/country code you entered is assigned to: %s\n", author_set[i].location_name);
		if (ret <= 0) {
		    warnp(__func__, "fprintf location/country code assignment");
		}
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
	} while (author_set[i].location_code == NULL || author_set[i].location_name == NULL || yorn == false);
	dbg(DBG_MED, "Author #%d location/country: %s (%s)", i, author_set[i].location_code, author_set[i].location_name);

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
		errno = 0;		/* pre-clear errno for errp() */
		ret = fprintf(stderr, "\nSorry ( tm Canada :-) ), we limit Email address to %d characters\n\n", MAX_EMAIL_LEN);
		if (ret <= 0) {
		    warnp(__func__, "fprintf error while printing Email address length limit");
		}

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
		if (p == NULL || author_set[i].email[0] == '@' || author_set[i].email[len - 1] == '@' ||
		    p != strrchr(author_set[i].email, '@')) {

		    /*
		     * issue rejection message
		     */
		    fpara(stderr,
			  "",
			  "Email addresses must have only a single @ somewhere inside the string.",
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
	    }
	} while (author_set[i].email == NULL);
	dbg(DBG_MED, "Author #%d Email: %s", i, author_set[i].email);

	/*
	 * ask for home URL
	 */
	do {

	    /*
	     * request URL
	     */
	    author_set[i].url = NULL;
	    author_set[i].url =
		prompt("Enter author home page URL (starting with http:// or https://), or press return to skip", &len);
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
		errno = 0;		/* pre-clear errno for errp() */
		ret = fprintf(stderr, "\nSorry ( tm Canada :-) ), we URLs to %d characters\n\n", MAX_URL_LEN);
		if (ret <= 0) {
		    warnp(__func__, "fprintf error while printing URL length limit");
		}

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
		if (((strncmp(author_set[i].url, "http://", LITLEN("http://")) == 0) &&
		     (author_set[i].url[LITLEN("http://")] != '\0')) ||
		    ((strncmp(author_set[i].url, "https://", LITLEN("https://")) == 0) &&
		     (author_set[i].url[LITLEN("https://")] != '\0'))) {

		    /*
		     * URL appears to in valid form
		     */
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
			  "url addresses must begin with http:// or https:// followed by the rest of the home page URL",
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
	    }
	} while (author_set[i].url == NULL);
	dbg(DBG_MED, "Author #%d URL: %s", i, author_set[i].url);

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
		errno = 0;		/* pre-clear errno for errp() */
		ret = fprintf(stderr, "\nSorry ( tm Canada :-) ), we limit twitter handles,"
			      "starting with the @, to %d characters\n\n", MAX_TWITTER_LEN);
		if (ret <= 0) {
		    warnp(__func__, "fprintf error while printing twitter handle length limit");
		}

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
			  "Twitter handles must start with a @ and have no other @-signs.",
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
	    }
	} while (author_set[i].twitter == NULL);
	dbg(DBG_MED, "Author #%d twitter: %s", i, author_set[i].twitter);

	/*
	 * ask for GitHub account
	 */
	do {

	    /*
	     * request GitHub account
	     */
	    author_set[i].github = NULL;
	    author_set[i].github = prompt("Enter author GitHub account, starting with @, or press return to skip", &len);
	    if (len == 0) {
		dbg(DBG_VHIGH, "GitHub account not given");
	    } else {
		dbg(DBG_VHIGH, "GitHub account: %s", author_set[i].github);
	    }

	    /*
	     * reject if too long
	     */
	    if (len > MAX_GITHUB_LEN) {

		/*
		 * issue rejection message
		 */
		errno = 0;		/* pre-clear errno for errp() */
		ret =
		    fprintf(stderr,
			    "\nSorry ( tm Canada :-) ), we limit GitHub account names, starting with the @, to %d characters\n\n",
			    MAX_GITHUB_LEN);
		if (ret <= 0) {
		    warnp(__func__, "fprintf error while printing GitHub user length limit");
		}

		/*
		 * free storage
		 */
		if (author_set[i].github != NULL) {
		    free(author_set[i].github);
		    author_set[i].github = NULL;
		}
		continue;
	    }

	    /*
	     * reject if no leading @, or if more than one @
	     */
	    if (len > 0) {
		p = strchr(author_set[i].github, '@');
		if (p == NULL || author_set[i].github[0] != '@' || p != strrchr(author_set[i].github, '@')
		    || author_set[i].github[1] == '\0') {

		    /*
		     * issue rejection message
		     */
		    fpara(stderr,
			  "",
			  "GitHub accounts must start with a @ and have no other @-signs.",
			  "",
			  NULL);

		    /*
		     * free storage
		     */
		    if (author_set[i].github != NULL) {
			free(author_set[i].github);
			author_set[i].github = NULL;
		    }
		    continue;
		}
	    }
	} while (author_set[i].github == NULL);
	dbg(DBG_MED, "Author #%d GitHub: %s", i, author_set[i].github);

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
		errno = 0;		/* pre-clear errno for errp() */
		ret =
		    fprintf(stderr, "\nSorry ( tm Canada :-) ), we limit affiliation names to %d characters\n\n",
			    MAX_AFFILIATION_LEN);
		if (ret <= 0) {
		    warnp(__func__, "fprintf error while printing affiliation length limit");
		}

		/*
		 * free storage
		 */
		if (author_set[i].affiliation != NULL) {
		    free(author_set[i].affiliation);
		    author_set[i].affiliation = NULL;
		}
		continue;
	    }
	} while (author_set[i].affiliation == NULL);
	dbg(DBG_MED, "Author #%d affiliation: %s", i, author_set[i].affiliation);



	/*
	 * verify the information for this author
	 */
	errno = 0;	/* pre-clear errno for errp() */
	if (printf("\nPlease verify the information about author #%d\n\n", i) <= 0 ||
	    printf("Name: %s\n", author_set[i].name) <= 0 ||
	    printf("Location/country code: %s (%s)\n", author_set[i].location_code, author_set[i].location_name) <= 0 ||
	    ((author_set[i].email[0] == '\0') ? printf("Email not given\n") :
						printf("Email: %s\n", author_set[i].email)) <= 0 ||
	    ((author_set[i].url[0] == '\0') ? printf("Url not given\n") :
					      printf("Url: %s\n", author_set[i].url)) <= 0 ||
	    ((author_set[i].twitter[0] == '\0') ? printf("Twitter handle not given\n") :
						  printf("Twitter handle: %s\n", author_set[i].twitter)) <= 0 ||
	    ((author_set[i].github[0] == '\0') ? printf("GitHub username not given\n") :
						 printf("GitHub username: %s\n", author_set[i].github)) <= 0 ||
	    ((author_set[i].affiliation[0] == '\0') ? printf("Affiliation not given\n\n") :
						      printf("Affiliation: %s\n\n", author_set[i].affiliation)) <= 0) {
	    errp(170, __func__, "error while printing author #%d information\n", i);
	    not_reached();
	}
	yorn = yes_or_no("Is that author information correct? [yn]");
	if (yorn == false) {
	    /*
	     * reenter author information
	     */
	    --i;
	    continue;
	}

	/*
	 * if we have an answers file, record the verified author information
	 */
	if (answerp != NULL && i_flag_used == false) {
	    errno = 0;			/* pre-clear errno for errp() */
	    ret = fprintf(answerp, "%s\n", author_set[i].name);
	    if (ret <= 0) {
		warnp(__func__, "fprintf error printing author name to the answers file");
		(*answers_errors)++;
	    }
	    errno = 0;			/* pre-clear errno for errp() */
	    ret = fprintf(answerp, "%s\ny\n", author_set[i].location_code);
	    if (ret <= 0) {
		warnp(__func__, "fprintf error printing author location to the answers file");
		(*answers_errors)++;
	    }
	    errno = 0;			/* pre-clear errno for errp() */
	    ret = fprintf(answerp, "%s\n", author_set[i].email);
	    if (ret <= 0) {
		warnp(__func__, "fprintf error printing author email to the answers file");
		(*answers_errors)++;
	    }
	    errno = 0;			/* pre-clear errno for errp() */
	    ret = fprintf(answerp, "%s\n", author_set[i].url);
	    if (ret <= 0) {
		warnp(__func__, "fprintf error printing author url to the answers file");
		(*answers_errors)++;
	    }
	    errno = 0;			/* pre-clear errno for errp() */
	    ret = fprintf(answerp, "%s\n", author_set[i].twitter);
	    if (ret <= 0) {
		warnp(__func__, "fprintf error printing author twitter handle to the answers file");
		(*answers_errors)++;
	    }
	    errno = 0;			/* pre-clear errno for errp() */
	    ret = fprintf(answerp, "%s\n", author_set[i].github);
	    if (ret <= 0) {
		warnp(__func__, "fprintf error printing author GitHub account to the answers file");
		(*answers_errors)++;
	    }
	    errno = 0;			/* pre-clear errno for errp() */
	    ret = fprintf(answerp, "%s\n", author_set[i].affiliation);
	    if (ret <= 0) {
		warnp(__func__, "fprintf error printing author affiliation to the answers file");
		(*answers_errors)++;
	    }
	    errno = 0;			/* pre-clear errno for errp() */
	    ret = fprintf(answerp, "y\n");
	    if (ret <= 0) {
		warnp(__func__, "fprintf error writing 'y' to confirm author information");
		(*answers_errors)++;
	    }
	}
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


/*
 * verify_entry_dir - ask user to verify the contents of the entry directory
 *
 * given:
 *      entry_dir       - path to entry directory
 *      ls              - path to ls utility
 *
 * This function does not return on error.
 */
static void
verify_entry_dir(char const *entry_dir, char const *ls)
{
    int exit_code;		/* exit code from system(cmd) */
    bool yorn = false;		/* response to a question */
    char *cmd = NULL;		/* cd entry_dir && ls -l . */
    FILE *ls_stream;		/* pipe from iocccsize -V */
    char *linep = NULL;		/* allocated line read from iocccsize */
    ssize_t readline_len;	/* readline return length */
    int kdirsize;		/* number of kibibyte (2^10) blocks in entry directory */
    char guard;			/* scanf guard to catch excess amount of input */
    int ret;			/* libc function return */

    /*
     * firewall
     */
    if (entry_dir == NULL || ls == NULL) {
	err(171, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * list the contents of the entry_dir
     */
    para("The following is a listing of the entry directory:",
	 "",
	 NULL);
    errno = 0;		/* pre-clear errno for errp() */
    ret = printf("    %s\n", entry_dir);
    if (ret <= 0) {
	warnp(__func__, "printf error code: %d", ret);
    }
    para("",
	 "form which the xz tarball will be formed:",
	 "",
	 NULL);
    errno = 0;			/* pre-clear errno for errp() */
    cmd = cmdprintf("cd -- % && % -lak .", entry_dir, ls);
    if (cmd == NULL) {
	err(172, __func__, "failed to cmdprintf: cd entry_dir && ls -lak .");
	not_reached();
    }
    dbg(DBG_HIGH, "about to perform: system(%s)", cmd);

    /*
     * pre-flush to avoid system() buffered stdio issues
     */
    clearerr(stdout);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fflush(stdout);
    if (ret < 0) {
	errp(173, __func__, "fflush(stdout) error code: %d", ret);
	not_reached();
    }
    errno = 0;			/* pre-clear errno for errp() */
    clearerr(stderr);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fflush(stderr);
    if (ret < 0) {
	errp(174, __func__, "fflush(stderr) #1: error code: %d", ret);
	not_reached();
    }

    /*
     * execute the ls command
     */
    errno = 0;			/* pre-clear errno for errp() */
    exit_code = system(cmd);
    if (exit_code < 0) {
	errp(175, __func__, "error calling system(%s)", cmd);
	not_reached();
    } else if (exit_code == 127) {
	errp(176, __func__, "execution of the shell failed for system(%s)", cmd);
	not_reached();
    } else if (exit_code != 0) {
	err(177, __func__, "%s failed with exit code: %d", cmd, WEXITSTATUS(exit_code));
	not_reached();
    }

    /*
     * pre-flush to avoid popen() buffered stdio issues
     */
    dbg(DBG_HIGH, "about to perform: popen(%s, r)", cmd);
    clearerr(stdout);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fflush(stdout);
    if (ret < 0) {
	errp(178, __func__, "fflush(stdout) #0: error code: %d", ret);
	not_reached();
    }
    clearerr(stderr);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fflush(stderr);
    if (ret < 0) {
	errp(179, __func__, "fflush(stderr) #1: error code: %d", ret);
	not_reached();
    }

    /*
     * form pipe to the ls command
     */
    errno = 0;			/* pre-clear errno for errp() */
    ls_stream = popen(cmd, "r");
    if (ls_stream == NULL) {
	errp(180, __func__, "popen for reading failed for: %s", cmd);
	not_reached();
    }
    setlinebuf(ls_stream);

    /*
     * read the 1st line - contains the total kibibyte (2^10) block line
     */
    dbg(DBG_HIGH, "reading 1st line from popen(%s, r)", cmd);
    readline_len = readline(&linep, ls_stream);
    if (readline_len < 0) {
	err(181, __func__, "EOF while reading 1st line from ls: %s", ls);
	not_reached();
    } else {
	dbg(DBG_HIGH, "ls 1st line read length: %ld buffer: %s", (long)readline_len, linep);
    }

    /*
     * parse k-block line from ls
     */
    ret = sscanf(linep, "total %d%c", &kdirsize, &guard);
    if (ret != 1) {
	err(182, __func__, "failed to parse block line from ls: %s", linep);
	not_reached();
    }
    if (kdirsize <= 0) {
	err(183, __func__, "ls k block value: %d <= 0", kdirsize);
	not_reached();
    }
    if (kdirsize > MAX_DIR_KSIZE) {
	fpara(stderr,
	      "",
	      "The entry is too large.",
	      "",
	      NULL);
	errno = 0;			/* pre-clear errno for errp() */
	ret = fprintf(stderr, "The entry directory %s is %d kibibyte (1024 byte blocks) in length.\n"
			      "It must be <= %d kibibyte (1024 byte blocks).\n\n", entry_dir, kdirsize, MAX_DIR_KSIZE);
	if (ret <= 0) {
	    errp(184, __func__, "fprintf error while printing entry directory kibibyte sizes");
	    not_reached();
	}
	err(185, __func__, "The entry directory is too large: %s", entry_dir);
	not_reached();
    }
    dbg(DBG_LOW, "entry directory %s size in kibibyte (1024 byte blocks): %d", entry_dir, kdirsize);

    /*
     * close down pipe
     */
    errno = 0;		/* pre-clear errno for errp() */
    ret = pclose(ls_stream);
    if (ret < 0) {
	warnp(__func__, "pclose error on ls stream");
    }
    ls_stream = NULL;

    /*
     * ask the user to verify the list
     */
    yorn = yes_or_no("\nIs the above list a correct list of files in your entry? [yn]");
    if (yorn == false) {
	fpara(stderr,
	      "",
	      "We suggest you remove the existing entry directory, and then",
	      "rerun this tool with the correct set of file arguments.",
	      NULL);
	err(186, __func__, "%s failed with exit code: %d", cmd, WEXITSTATUS(exit_code));
	not_reached();
    }

    /*
     * free storage
     */
    if (cmd != NULL) {
	free(cmd);
	cmd = NULL;
    }
    if (linep != NULL) {
	free(linep);
	linep = NULL;
    }
    return;
}


/*
 * json_putc - print a character with JSON encoding
 *
 * JSON string encoding JSON string encoding.  We will encode as follows:
 *
 *     old		new
 *     --------------------
 *	"		\"
 *	/		\/
 *	\		\\
 *	<backspace>	\b
 *	<vertical tab>	\f
 *	<tab>		\t
 *	<enter>		\r
 *	<newline>	\n
 *	<		\u003C
 *	>		\u003E
 *	&		\uoo26
 *	%		\u0025
 *	\x80-\xff	\u0080 - \u00ff
 *
 * See:
 *
 *	https://developpaper.com/escape-and-unicode-encoding-in-json-serialization/
 *
 * given:
 *	stream	- string to print on
 *	c	- character to encode
 *
 * returns:
 *	true ==> stream print was OK,
 *	false ==> error printing to stream
 */
static bool
json_putc(int const c, FILE *stream)
{
    int ret;			/* libc function return */

    /*
     * firewall
     */
    if (stream == NULL) {
	warn(__func__, "called with NULL arg(s)");
	return false;
    }

    /*
     * case: \cFFFF encoding
     */
    errno = 0;			/* pre-clear errno for errp() */
    if ((c >= 0x80 && c <= 0xffff) || c == '<' || c == '>' || c == '&' || c == '%') {
	ret = fprintf(stream, "\\c%04X", c);
	if (ret <= 0) {
	    warnp(__func__, "fprintf #0 error in \\cFFFF encoding");
	    return false;
	}
        return true;
    }

    /*
     * case: \ escaped char
     */
    switch (c) {
    case '"':
	ret = fprintf(stream, "\\\"");
	break;
    case '/':
	ret = fprintf(stream, "\\/");
	break;
    case '\\':
	ret = fprintf(stream, "\\\\");
	break;
    case '\b':
	ret = fprintf(stream, "\\b");
	break;
    case '\f':
	ret = fprintf(stream, "\\f");
	break;
    case '\t':
	ret = fprintf(stream, "\\t");
	break;
    case '\r':
	ret = fprintf(stream, "\\r");
	break;
    case '\n':
	ret = fprintf(stream, "\\n");
	break;

    /*
     * case: un-escaped char
     */
    default:
	ret = fprintf(stream, "%c", c);
	break;
    }
    if (ret <= 0) {
	warnp(__func__, "fprintf #1 error");
	return false;
    }
    return true;
}


/*
 * json_fprintf_str - print a JSON string
 *
 * Print on stream:
 *
 * If str == NULL:
 *
 *	null
 *
 * else str != NULL:
 *
 *	str with JSON string encoding surrounded by "'s
 *
 * See:
 *
 *	https://www.json.org/json-en.html
 *
 * given:
 *	stream	- open file stream to print on
 *	str	- the string to JSON encode or NULL
 *
 * returns:
 *	true ==> stream print was OK,
 *	false ==> error printing to stream
 *
 * This function does not return on error.
 */
static bool
json_fprintf_str(FILE *stream, char const *str)
{
    int ret;			/* libc function return */
    char const *p;

    /*
     * firewall
     */
    if (stream == NULL) {
	warn(__func__, "called with NULL arg(s)");
	return false;
    }

    /*
     * case: NULL
     */
    if (str == NULL) {
	errno = 0;			/* pre-clear errno for errp() */
	ret = fprintf(stream, "null");
	if (ret <= 0) {
	    warnp(__func__, "fprintf #0 error for null");
	    return false;
	}
	return true;
    }

    /*
     * print leading double-quote
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fputc('"', stream);
    if (ret == EOF) {
	warnp(__func__, "fputc #0 error for leading double-quote");
	return false;
    }

    /*
     * print name, JSON encoded
     */
    for (p=str; *p != '\0'; ++p) {
	if (json_putc(*p, stream) != true) {
	    warnp(__func__, "json_putc #0 error");
	    return false;
	}
    }

    /*
     * print trailing double-quote
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fputc('"', stream);
    if (ret == EOF) {
	warnp(__func__, "fputc #1 error for trailing double-quote");
	return false;
    }
    return true;
}


/*
 * json_fprintf_value_string - print name value (as a string) pair
 *
 * On a stream, we will print:
 *
 *	lead "name_encoded " middle "value_encoded" tail
 *
 * given:
 *	stream	- open file stream to print on
 *	lead	- leading whitespace string to print
 *	name	- name string to JSON encode or NULL
 *	middle	- middle string (often " : " )l
 *	value	- value string to JSON encode or NULL
 *	tail	- tailing string to print (often ",\n")
 *
 * returns:
 *	true
 *
 * returns:
 *	true ==> stream print was OK,
 *	false ==> error printing to stream
 *
 * This function does not return on error.
 */
static bool
json_fprintf_value_string(FILE *stream, char const *lead, char const *name, char const *middle, char const *value,
			  char const *tail)
{
    int ret;			/* libc function return */

    /*
     * firewall
     */
    if (stream == NULL || lead == NULL || middle == NULL || tail == NULL) {
	warn(__func__, "called with NULL arg(s)");
	return false;
    }

    /*
     * print leading string
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fprintf(stream, "%s", lead);
    if (ret < 0) { /* compare < 0 only in case lead is an empty string */
	warn(__func__, "fprintf printing lead");
	return false;
    }

    /*
     * print name as a JSON encoded string
     */
    if (json_fprintf_str(stream, name) != true) {
	warn(__func__, "json_fprintf_str error printing name");
	return false;
    }

    /*
     * print middle string
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fprintf(stream, "%s", middle);
    if (ret < 0) { /* compare < 0 only in case middle is an empty string */
	warn(__func__, "fprintf printing middle");
	return false;
    }

    /*
     * print value as a JSON encoded string
     */
    if (json_fprintf_str(stream, value) != true) {
	warn(__func__, "json_fprintf_str for value as a string");
	return false;
    }

    /*
     * print trailing string
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fprintf(stream, "%s", tail);
    if (ret < 0) { /* compare < 0 only in case tail is an empty string */
	warn(__func__, "fprintf printing end of line");
	return false;
    }
    return true;
}


/*
 * json_fprintf_value_long - print name value (as a long integer) pair
 *
 * On a stream, we will print:
 *
 *	lead "name_encoded" middle long_value tail
 *
 * given:
 *	stream	- open file stream to print on
 *	lead	- leading whitespace string to print
 *	name	- name string to JSON encode or NULL
 *	middle	- middle string (often " : " )l
 *	value	- value as long
 *	tail	- tailing string to print (often ",\n")
 *
 * returns:
 *	true
 *
 * returns:
 *	true ==> stream print was OK,
 *	false ==> error printing to stream
 *
 * This function does not return on error.
 */
static bool
json_fprintf_value_long(FILE *stream, char const *lead, char const *name, char const *middle, long value,
			char const *tail)
{
    int ret;			/* libc function return */

    /*
     * firewall
     */
    if (stream == NULL || lead == NULL || middle == NULL || tail == NULL) {
	warn(__func__, "called with NULL arg(s)");
	return false;
    }

    /*
     * print leading string
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fprintf(stream, "%s", lead);
    if (ret < 0) { /* compare < 0 only in case lead is an empty string */
	warnp(__func__, "fprintf printing lead");
	return false;
    }

    /*
     * print name as a JSON encoded string
     */
    if (json_fprintf_str(stream, name) != true) {
	warnp(__func__, "json_fprintf_str error printing name");
	return false;
    }

    /*
     * print middle string
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fprintf(stream, "%s", middle);
    if (ret < 0) { /* compare < 0 only in case middle is an empty string */
	warnp(__func__, "fprintf printing middle");
	return false;
    }

    /*
     * print value as a JSON long
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fprintf(stream, "%ld", value);
    if (ret <= 0) {
	warnp(__func__, "fprintf for value as a long");
	return false;
    }

    /*
     * print trailing string
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fprintf(stream, "%s", tail);
    if (ret < 0) { /* compare < 0 only in case tail is an empty string */
	warnp(__func__, "fprintf printing end of line");
	return false;
    }
    return true;
}


/*
 * json_fprintf_value_bool - print name value (as a boolean) pair
 *
 * On a stream, we will print:
 *
 *	lead "name_encoded" middle true tail
 * or:
 *	lead "name_encoded" middle false tail
 *
 * given:
 *	stream	- open file stream to print on
 *	lead	- leading whitespace string to print
 *	name	- name string to JSON encode or NULL
 *	middle	- middle string (often " : " )l
 *	value	- value as boolean
 *	tail	- tailing string to print (often ",\n")
 *
 * returns:
 *	true ==> stream print was OK,
 *	false ==> error printing to stream
 */
static bool
json_fprintf_value_bool(FILE *stream, char const *lead, char const *name, char const *middle, bool value,
			char const *tail)
{
    int ret;			/* libc function return */

    /*
     * firewall
     */
    if (stream == NULL || lead == NULL || middle == NULL || tail == NULL) {
	warn(__func__, "called with NULL arg(s)");
	return false;
    }

    /*
     * print leading string
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fprintf(stream, "%s", lead);
    if (ret < 0) { /* compare < 0 only in case lead is an empty string */
	warnp(__func__, "fprintf printing lead");
	return false;
    }

    /*
     * print name as a JSON encoded string
     */
    if (json_fprintf_str(stream, name) != true) {
	warn(__func__, "json_fprintf_str error printing name");
	return false;
    }

    /*
     * print middle string
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fprintf(stream, "%s", middle);
    if (ret < 0) { /* compare < 0 only in case middle is an empty string */
	warnp(__func__, "fprintf printing middle");
	return false;
    }

    /*
     * print value as a JSON boolean
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fprintf(stream, "%s", (value == true) ? "true" : "false");
    if (ret <= 0) {
	warnp(__func__, "fprintf for value as a boolean");
	return false;
    }

    /*
     * print trailing string
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fprintf(stream, "%s", tail);
    if (ret < 0) { /* compare < 0 only in case tail is an empty string */
	warnp(__func__, "fprintf printing end of line");
	return false;
    }
    return true;
}


/*
 * strnull - return NULL if string is empty
 *
 * given:
 *	str (NULL allowed)
 *
 * returns:
 *	str if str is NOT empty,
 *	else NULL
 */
static char const *
strnull(char const * const str)
{
    /*
     * if str is non-NULL and non-zero length, return str
     */
    if (str != NULL && strlen(str) > 0) {
	return str;
    }
    return NULL;
}


/*
 * write_info - create the .info.json file
 *
 * Form a simple JSON .info file describing the entry.
 *
 * given:
 *      infop           - pointer to info structure
 *      entry_dir       - path to entry directory
 *      test_mode       - true ==> test mode, do not upload
 *
 * returns:
 *	true
 *
 * This function does not return on error.
 */
static void
write_info(struct info *infop, char const *entry_dir, bool test_mode)
{
    struct tm *timeptr;		/* localtime return */
    char *info_path;		/* path to .info.json file */
    size_t info_path_len;	/* length of path to .info.json */
    FILE *info_stream;		/* open write stream to the .info.json file */
    size_t asctime_len;		/* length of asctime() string without the trailing newline */
    size_t gmtime_len;		/* length of gmtime string (gmtime() + " UTC") */
    int ret;			/* libc function return */
    char **q;			/* extra filename array pointer */
    char *p;
    int i;

    /*
     * firewall
     */
    if (infop == NULL || entry_dir == NULL) {
	err(187, __func__, "called with NULL arg(s)");
	not_reached();
    }
    if (infop->extra_count < 0) {
	warn(__func__, "extra_count %d < 0", infop->extra_count);
    }

    /*
     * fill out time information in the info structure
     */

    /*
     * timestamp epoch
     */
    errno = 0;			/* pre-clear errno for errp() */
    infop->epoch = strdup(TIMESTAMP_EPOCH);
    if (infop->epoch == NULL) {
	errp(188, __func__, "strdup of %s failed", TIMESTAMP_EPOCH);
	not_reached();
    }
    dbg(DBG_VVHIGH, "infop->epoch: %s", infop->epoch);

    /*
     * reset to UTC timezone
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = setenv("TZ", "UTC", 1);
    if (ret < 0) {
	errp(189, __func__, "cannot set TZ=UTC");
	not_reached();
    }
    errno = 0;			/* pre-clear errno for errp() */
    timeptr = localtime(&(infop->tstamp));
    if (timeptr == NULL) {
	errp(190, __func__, "localtime #1 returned NULL");
	not_reached();
    }
    if (timeptr->tm_zone == NULL) {
	err(191, __func__, "timeptr->tm_zone #1 is NULL");
	not_reached();
    }

    /*
     * ASCII UTC string
     */
    errno = 0;			/* pre-clear errno for errp() */
    p = asctime(timeptr);
    if (p == NULL) {
	errp(192, __func__, "asctime #1 returned NULL");
	not_reached();
    }
    asctime_len = strlen(p) - 1; /* -1 to remove trailing newline */
    gmtime_len = strlen(p) + 1 + LITLEN("UTC") + 1;
    errno = 0;			/* pre-clear errno for errp() */
    infop->gmtime = (char *)calloc(gmtime_len + 1, 1);
    if (infop->gmtime == NULL) {
	errp(193, __func__, "calloc of %lu bytes failed", (unsigned long)gmtime_len + 1);
	not_reached();
    }
    (void) strncat(infop->gmtime, p, asctime_len);
    (void) strcat(infop->gmtime, " UTC");
    dbg(DBG_VVHIGH, "infop->gmtime: %s", infop->gmtime);

    /*
     * open .info.json for writing
     */
    info_path_len = strlen(entry_dir) + 1 + LITLEN(".info.json") + 1;
    errno = 0;			/* pre-clear errno for errp() */
    info_path = (char *)malloc(info_path_len + 1);
    if (info_path == NULL) {
	errp(194, __func__, "malloc of %lu bytes failed", (unsigned long)info_path_len + 1);
	not_reached();
    }
    errno = 0;			/* pre-clear errno for errp() */
    ret = snprintf(info_path, info_path_len, "%s/.info.json", entry_dir);
    if (ret <= 0) {
	errp(195, __func__, "snprintf #0 error: %d", ret);
	not_reached();
    }
    dbg(DBG_HIGH, ".info.json path: %s", info_path);
    errno = 0;			/* pre-clear errno for errp() */
    info_stream = fopen(info_path, "w");
    if (info_stream == NULL) {
	errp(196, __func__, "failed to open for writing: %s", info_path);
	not_reached();
    }

    /*
     * write leading part of info to the open .info.json file
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fprintf(info_stream, "{\n") > 0 &&
	json_fprintf_value_string(info_stream, "\t", "IOCCC_info_version", " : ", INFO_VERSION, ",\n") &&
	json_fprintf_value_string(info_stream, "\t", "ioccc_contest", " : ", IOCCC_CONTEST, ",\n") &&
	json_fprintf_value_long(info_stream, "\t", "ioccc_year", " : ", (long)IOCCC_YEAR, ",\n") &&
	json_fprintf_value_string(info_stream, "\t", "mkiocccentry_version", " : ", infop->mkiocccentry_ver, ",\n") &&
	json_fprintf_value_string(info_stream, "\t", "iocccsize_version", " : ", infop->iocccsize_ver, ",\n") &&
	json_fprintf_value_string(info_stream, "\t", "IOCCC_contest_id", " : ", infop->ioccc_id, ",\n") &&
	json_fprintf_value_long(info_stream, "\t", "entry_num", " : ", (long)infop->entry_num, ",\n") &&
	json_fprintf_value_string(info_stream, "\t", "title", " : ", infop->title, ",\n") &&
	json_fprintf_value_string(info_stream, "\t", "abstract", " : ", infop->abstract, ",\n") &&
	json_fprintf_value_long(info_stream, "\t", "rule_2a_size", " : ", (long)infop->rule_2a_size, ",\n") &&
	json_fprintf_value_long(info_stream, "\t", "rule_2b_size", " : ", (long)infop->rule_2b_size, ",\n") &&
	json_fprintf_value_bool(info_stream, "\t", "empty_override", " : ", infop->empty_override, ",\n") &&
	json_fprintf_value_bool(info_stream, "\t", "rule_2a_override", " : ", infop->rule_2a_override, ",\n") &&
	json_fprintf_value_bool(info_stream, "\t", "rule_2a_mismatch", " : ", infop->rule_2a_mismatch, ",\n") &&
	json_fprintf_value_bool(info_stream, "\t", "rule_2b_override", " : ", infop->rule_2b_override, ",\n") &&
	json_fprintf_value_bool(info_stream, "\t", "highbit_warning", " : ", infop->highbit_warning, ",\n") &&
	json_fprintf_value_bool(info_stream, "\t", "nul_warning", " : ", infop->nul_warning, ",\n") &&
	json_fprintf_value_bool(info_stream, "\t", "trigraph_warning", " : ", infop->trigraph_warning, ",\n") &&
	json_fprintf_value_bool(info_stream, "\t", "wordbuf_warning", " : ", infop->wordbuf_warning, ",\n") &&
	json_fprintf_value_bool(info_stream, "\t", "ungetc_warning", " : ", infop->ungetc_warning, ",\n") &&
	json_fprintf_value_bool(info_stream, "\t", "Makefile_override", " : ", infop->Makefile_override, ",\n") &&
	json_fprintf_value_bool(info_stream, "\t", "test_mode", " : ", test_mode, ",\n") &&
	fprintf(info_stream, "\t\"manifest\" : [\n") > 0;
    if (ret == false) {
	errp(197, __func__, "fprintf error writing leading part of info to %s", info_path);
	not_reached();
    }

    /*
     * write mandatory files to the open .info.json file
     */
    ret = json_fprintf_value_string(info_stream, "\t\t{", "info_JSON", " : ", ".info.json", "},\n") &&
	  json_fprintf_value_string(info_stream, "\t\t{", "author_JSON", " : ", ".author.json", "},\n") &&
	  json_fprintf_value_string(info_stream, "\t\t{", "c_src", " : ", infop->prog_c, "},\n") &&
	  json_fprintf_value_string(info_stream, "\t\t{", "Makefile", " : ", infop->Makefile, "},\n") &&
	  json_fprintf_value_string(info_stream, "\t\t{", "remarks", " : ", infop->remarks_md,
				    ((infop->extra_count > 0) ?  "},\n" : "}\n"));
    if (ret == false) {
	errp(198, __func__, "fprintf error writing mandatory filename to %s", info_path);
	not_reached();
    }

    /*
     * write extra files to the open .info.json file
     */
    for (i=0, q=infop->extra_file; i < infop->extra_count && *q != NULL; ++i, ++q) {
        if (json_fprintf_value_string(info_stream, "\t\t{", "extra_file", " : ", *q,
				     (((i+1) < infop->extra_count) ? "},\n" : "}\n")) != true) {
	    errp(199, __func__, "fprintf error writing extra filename[%d] to %s", i, info_path);
	    not_reached();
	}
    }

    /*
     * write trailing part of info to the open .info.json file
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fprintf(info_stream, "\t],\n") > 0 &&
	json_fprintf_value_long(info_stream, "\t", "formed_timestamp", " : ", (long)infop->tstamp, ",\n") &&
	json_fprintf_value_long(info_stream, "\t", "formed_timestamp_usec", " : ", (long)infop->usec, ",\n") &&
	json_fprintf_value_string(info_stream, "\t", "timestamp_epoch", " : ", infop->epoch, ",\n") &&
	json_fprintf_value_string(info_stream, "\t", "formed_UTC", " : ", infop->gmtime, "\n") &&
	fprintf(info_stream, "}\n") > 0;
    if (ret == false) {
	errp(200, __func__, "fprintf error writing trailing part of info to %s", info_path);
	not_reached();
    }

    /*
     * free storage
     */
    if (info_path != NULL) {
	free(info_path);
	info_path = NULL;
    }

    /*
     * close the file
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fclose(info_stream);
    if (ret < 0) {
	errp(201, __func__, "fclose error");
	not_reached();
    }
    return;
}


/*
 * write_author - create the .author.json file
 *
 * Form a simple JSON .author file describing the entry.
 *
 * given:
 *      infop           - pointer to info structure
 *      author_count    - length of the author structure array in elements
 *      authorp         - pointer to author structure array
 *      entry_dir       - path to entry directory
 *
 * This function does not return on error.
 */
static void
write_author(struct info *infop, int author_count, struct author *authorp, char const *entry_dir)
{
    char *author_path;		/* path to .author.json file */
    size_t author_path_len;	/* length of path to .author.json */
    FILE *author_stream;	/* open write stream to the .author.json file */
    int ret;			/* libc function return */
    int i;

    /*
     * firewall
     */
    if (authorp == NULL || entry_dir == NULL) {
	err(202, __func__, "called with NULL arg(s)");
	not_reached();
    }
    if (author_count <= 0) {
	warn(__func__, "author_count %d <= 0", author_count);
    }

    /*
     * open .author.json for writing
     */
    author_path_len = strlen(entry_dir) + 1 + LITLEN(".author.json") + 1;
    errno = 0;			/* pre-clear errno for errp() */
    author_path = (char *)malloc(author_path_len + 1);
    if (author_path == NULL) {
	errp(203, __func__, "malloc of %lu bytes failed", (unsigned long)author_path_len + 1);
	not_reached();
    }
    errno = 0;			/* pre-clear errno for errp() */
    ret = snprintf(author_path, author_path_len, "%s/.author.json", entry_dir);
    if (ret <= 0) {
	errp(204, __func__, "snprintf #0 error: %d", ret);
	not_reached();
    }
    dbg(DBG_HIGH, ".author.json path: %s", author_path);
    errno = 0;			/* pre-clear errno for errp() */
    author_stream = fopen(author_path, "w");
    if (author_stream == NULL) {
	errp(205, __func__, "failed to open for writing: %s", author_path);
	not_reached();
    }

    /*
     * write leading part of authorship to the open .author.json file
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fprintf(author_stream, "{\n") > 0 &&
	json_fprintf_value_string(author_stream, "\t", "IOCCC_author_version", " : ", AUTHOR_VERSION, ",\n") &&
	json_fprintf_value_string(author_stream, "\t", "ioccc_contest", " : ", IOCCC_CONTEST, ",\n") &&
	json_fprintf_value_long(author_stream, "\t", "ioccc_year", " : ", (long)IOCCC_YEAR, ",\n") &&
	json_fprintf_value_string(author_stream, "\t", "mkiocccentry_version", " : ", infop->mkiocccentry_ver, ",\n") &&
	json_fprintf_value_string(author_stream, "\t", "IOCCC_contest_id", " : ", infop->ioccc_id, ",\n") &&
	json_fprintf_value_long(author_stream, "\t", "entry_num", " : ", (long)infop->entry_num, ",\n") &&
	fprintf(author_stream, "\t\"authors\" : [\n") > 0;
    if (ret == false) {
	errp(206, __func__, "fprintf error writing leading part of authorship to %s", author_path);
	not_reached();
    }

    /*
     * write author info to the open .author.json file
     */
    for (i = 0; i < author_count; ++i) {
	errno = 0;		/* pre-clear errno for errp() */
	ret = fprintf(author_stream, "\t\t{\n") > 0 &&
	    json_fprintf_value_string(author_stream, "\t\t\t", "name", " : ", authorp[i].name, ",\n") &&
	    json_fprintf_value_string(author_stream, "\t\t\t", "location_code", " : ", authorp[i].location_code, ",\n") &&
	    json_fprintf_value_string(author_stream, "\t\t\t", "location_name", " : ", authorp[i].location_name, ",\n") &&
	    json_fprintf_value_string(author_stream, "\t\t\t", "email", " : ", strnull(authorp[i].email), ",\n") &&
	    json_fprintf_value_string(author_stream, "\t\t\t", "url", " : ", strnull(authorp[i].url), ",\n") &&
	    json_fprintf_value_string(author_stream, "\t\t\t", "twitter", " : ", strnull(authorp[i].twitter), ",\n") &&
	    json_fprintf_value_string(author_stream, "\t\t\t", "github", " : ", strnull(authorp[i].github), ",\n") &&
	    json_fprintf_value_string(author_stream, "\t\t\t", "affiliation", " : ", strnull(authorp[i].affiliation), ",\n") &&
	    json_fprintf_value_long(author_stream, "\t\t\t", "author_number", " : ", authorp[i].author_num, "\n") &&
	    fprintf(author_stream, "\t\t}%s\n", (((i + 1) < author_count) ? "," : "")) > 0;
	if (ret < 0) {
	    errp(207, __func__, "fprintf error writing author info to %s", author_path);
	    not_reached();
	}
    }

    /*
     * write trailing part of authorship to the open .author.json file
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fprintf(author_stream, "\t],\n") > 0 &&
	json_fprintf_value_long(author_stream, "\t", "formed_timestamp", " : ", (long)infop->tstamp, ",\n") &&
	json_fprintf_value_long(author_stream, "\t", "formed_timestamp_usec", " : ", (long)infop->usec, ",\n") &&
	json_fprintf_value_string(author_stream, "\t", "timestamp_epoch", " : ", infop->epoch, ",\n") &&
	json_fprintf_value_string(author_stream, "\t", "formed_UTC", " : ", infop->gmtime, "\n") &&
	fprintf(author_stream, "}\n") > 0;
    if (ret == false) {
	errp(208, __func__, "fprintf error writing trailing part of authorship to %s", author_path);
	not_reached();
    }

    /*
     * free storage
     */
    if (author_path != NULL) {
	free(author_path);
	author_path = NULL;
    }

    /*
     * close the file
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fclose(author_stream);
    if (ret < 0) {
	errp(209, __func__, "fclose error");
	not_reached();
    }
    return;
}


/*
 * form_tarball - form the compressed tarball
 *
 * Given the completed entry directory, form a compressed tar file for the user to submit.
 * Remind the user where to submit their compressed tarball file.
 *
 * given:
 *      work_dir        - working directory under which the entry directory is formed
 *      entry_dir       - path to entry directory
 *      tarball_path    - path of the compressed tarball to form
 *      tar             - path to the tar utility
 *      ls              - path to ls utility
 *
 * This function does not return on error.
 */
static void
form_tarball(char const *work_dir, char const *entry_dir, char const *tarball_path, char const *tar, char const *ls)
{
    char *basename_entry_dir;	/* basename of the entry directory */
    char *basename_tarball_path;/* basename of tarball_path */
    char *cmd;			/* the tar command to form the compressed tarball */
    int exit_code;		/* exit code from system(cmd) */
    struct stat buf;		/* stat of the tarball */
    int ret;			/* libc function return */

    /*
     * firewall
     */
    if (work_dir == NULL || entry_dir == NULL || tarball_path == NULL || tar == NULL || ls == NULL) {
	err(210, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * verify entry directory contents
     */
    verify_entry_dir(entry_dir, ls);
    dbg(DBG_LOW, "verified entry directory: %s", entry_dir);

    /*
     * cd into the work_dir, just above the entry_dir and where the compressed tarball will be formed
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = chdir(work_dir);
    if (ret < 0) {
	errp(211, __func__, "cannot cd %s", work_dir);
	not_reached();
    }

    /*
     * form the tar create command
     */
    basename_entry_dir = base_name(entry_dir);
    basename_tarball_path = base_name(tarball_path);
    cmd = cmdprintf("% --format=v7 -cJf % %", tar, basename_tarball_path, basename_entry_dir);
    if (cmd == NULL) {
	err(212, __func__, "failed to cmdprintf: tar --format=v7 -cJf tarball_path entry_dir");
	not_reached();
    }
    dbg(DBG_HIGH, "about to perform: system(%s)", cmd);

    /*
     * pre-flush to avoid system() buffered stdio issues
     */
    clearerr(stdout);	/* pre-clear ferror() status */
    errno = 0;		/* pre-clear errno for errp() */
    ret = fflush(stdout);
    if (ret < 0) {
	errp(213, __func__, "fflush(stdout) error code: %d", ret);
	not_reached();
    }
    clearerr(stderr);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fflush(stderr);
    if (ret < 0) {
	errp(214, __func__, "fflush(stderr) #1: error code: %d", ret);
	not_reached();
    }

    /*
     * perform the tar create command
     */
    para("",
	 "About to run the tar command to form the compressed tarball ...",
	 "",
	 NULL);
    errno = 0;			/* pre-clear errno for errp() */
    exit_code = system(cmd);
    if (exit_code < 0) {
	errp(215, __func__, "error calling system(%s)", cmd);
	not_reached();
    } else if (exit_code == 127) {
	errp(216, __func__, "execution of the shell failed for system(%s)", cmd);
	not_reached();
    } else if (exit_code != 0) {
	err(217, __func__, "%s failed with exit code: %d", cmd, WEXITSTATUS(exit_code));
	not_reached();
    }

    /*
     * enforce the maximum size of the compressed tarball
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = stat(basename_tarball_path, &buf);
    if (ret != 0) {
	errp(218, __func__, "stat of the compressed tarball failed: %s", basename_tarball_path);
	not_reached();
    }
    if (buf.st_size > MAX_TARBALL_LEN) {
	fpara(stderr,
	      "",
	      "The compressed tarball exceeds the maximum allowed size, sorry.",
	      "",
	      NULL);
	err(219, __func__, "The compressed tarball: %s size: %lu > %ld",
		 basename_tarball_path, (unsigned long)buf.st_size, (long)MAX_TARBALL_LEN);
	not_reached();
    }

    /*
     * free command for next use
     */
    if (cmd != NULL) {
	free(cmd);
	cmd = NULL;
    }

    /*
     * form the tar list command
     */
    cmd = cmdprintf("% -tvJf %", tar, basename_tarball_path);
    if (cmd == NULL) {
	err(220, __func__, "failed to cmdprintf: tar -tJf tarball_path");
	not_reached();
    }
    dbg(DBG_HIGH, "about to perform: system(%s)", cmd);

    /*
     * pre-flush to avoid system() buffered stdio issues
     */
    clearerr(stdout);	/* pre-clear ferror() status */
    errno = 0;		/* pre-clear errno for errp() */
    ret = fflush(stdout);
    if (ret < 0) {
	errp(221, __func__, "fflush(stdout) error code: %d", ret);
	not_reached();
    }
    clearerr(stderr);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fflush(stderr);
    if (ret < 0) {
	errp(222, __func__, "fflush(stderr) #1: error code: %d", ret);
	not_reached();
    }

    /*
     * perform the tar list command
     */
    errno = 0;			/* pre-clear errno for errp() */
    exit_code = system(cmd);
    if (exit_code < 0) {
	errp(223, __func__, "error calling system(%s)", cmd);
	not_reached();
    } else if (exit_code == 127) {
	errp(224, __func__, "execution of the shell failed for system(%s)", cmd);
	not_reached();
    } else if (exit_code != 0) {
	err(225, __func__, "%s failed with exit code: %d", cmd, WEXITSTATUS(exit_code));
	not_reached();
    }
    para("",
	 "... the output above is the listing of the compressed tarball.",
	 "",
	 NULL);

    /*
     * free memory
     */
    if (basename_entry_dir != NULL) {
	free(basename_entry_dir);
	basename_entry_dir = NULL;
    }
    if (basename_tarball_path != NULL) {
	free(basename_tarball_path);
	basename_tarball_path = NULL;
    }
    if (cmd != NULL) {
	free(cmd);
	cmd = NULL;
    }
    return;
}


/*
 * remind_user - remind the user to upload (if not in test mode)
 *
 * given:
 *      work_dir        - working directory under which the entry directory is formed
 *      entry_dir       - path to entry directory
 *      tar             - path to the tar utility
 *      tarball_path    - path of the compressed tarball to form
 *      test_mode       - true ==> test mode, do not upload
 *      answers		- path to the answers file (if specified)
 *      infop		- pointer to info structure
 *      answers_errors	- > 0 ==> I/O errors on answers file
 */
static void
remind_user(char const *work_dir, char const *entry_dir, char const *tar, char const *tarball_path, bool test_mode,
	    char const *answers, struct info *infop, unsigned answers_errors)
{
    int ret;			/* libc function return */
    char *entry_dir_esc;
    char *work_dir_esc;

    /*
     * firewall
     */
    if (work_dir == NULL || entry_dir == NULL || tar == NULL || tarball_path == NULL) {
	err(226, __func__, "called with NULL arg(s)");
	not_reached();
    }

    entry_dir_esc = cmdprintf("%", entry_dir);
    if (entry_dir_esc == NULL) {
	err(227, __func__, "failed to cmdprintf: entry_dir");
	not_reached();
    }

    /*
     * tell user they can now remove entry_dir
     */
    para("Now that we have formed the compressed tarball file, you",
	 "can remove the entry directory we have formed by executing:",
	 "",
	 NULL);
    ret = printf("    rm -rf %s%s\n", entry_dir[0] == '-' ? "-- " : "", entry_dir_esc);
    if (ret <= 0) {
	errp(228, __func__, "printf #0 error");
	not_reached();
    }
    free(entry_dir_esc);

    work_dir_esc = cmdprintf("%", work_dir);
    if (work_dir_esc == NULL) {
	err(229, __func__, "failed to cmdprintf: work_dir");
	not_reached();
    }

    para("",
	 "If you are curious, you may examine the newly created compressed tarball",
	 "by running the following command:",
	 "",
	 NULL);
    ret = printf("    %s -Jtvf %s%s/%s\n", tar, work_dir[0] == '-' ? "./" : "", work_dir_esc, tarball_path);
    if (ret <= 0) {
	errp(230, __func__, "printf #2 error");
	not_reached();
    }
    free(work_dir_esc);

    /*
     * remind the user about their answers file
     */
    if (answers != NULL) {
	errno = 0;			/* pre-clear errno for errp() */
	ret = printf("\nTo more easily update this entry you can run:\n\n    ./mkiocccentry -i %s ...\n", answers);
	if (ret <= 0) {
	    warnp(__func__, "unable to tell user how to more easily update entry");
	}
	if (answers_errors > 0) {
	    errno = 0;	/* pre-clear errno for errp() */
	    ret = printf("Warning: There were %u I/O error%s on the answers file. Make SURE to verify that using the file\n"
			 "results in the proper input before reuploading!\n",
			 answers_errors, answers_errors==1?"":"s");
	    if (ret <= 0) {
		warnp(__func__, "unable to warn user that there were I/O errors on the answers file");
	    }
	}

	if (infop->warnings_ignored == true) {
	    errno = 0;	/* pre-clear errno for errp() */
	    ret = printf("\nYou've ignored one or more warnings. If you update your files so that these warnings\n"
		         "aren't triggered or if different warnings are triggered, it is very likely that the\n"
			 "answers file will cause invalid results and we STRONGLY recommend that you re-enter\n"
			 "your answers!\n");
	    if (ret <= 0) {
		warnp(__func__, "unable to warn user about ignoring warnings when writing to the answer file");
	    }
	}
	else {
	    errno = 0;	/* pre-clear errno for errp() */
	    printf("\nAlthough you've not ignored any warnings be aware that if you add or change a file that\n"
		    "triggers a warning the next time, then this can result in improper answers being provided.\n");
	    if (ret <= 0) {
		warnp(__func__, "unable to warn user that if they add or change a file that triggers a warning, it cause invalid input");
	    }
	}
    }

    /*
     * case: test mode
     */
    if (test_mode == true) {

	/*
	 * remind them that this is a test entry, not an official entry
	 */
	para("",
	     "As you entered an IOCCC contest ID of test, the compressed tarball",
	     "that was just formed CANNOT be used as an IOCCC entry.",
	     "",
	     NULL);

    /*
     * case: entry mode
     */
    } else {

	/*
	 * inform them of the compressed tarball file
	 */
	para("",
	     "Assuming that the IOCCC is still open, you may submit your entry",
	     "by uploading following compressed tarball file:",
	     "",
	     NULL);
	ret = printf("    %s/%s\n", work_dir, tarball_path);
	if (ret <= 0) {
	    errp(231, __func__, "printf #2 error");
	    not_reached();
	}
    }

    /*
     * case: test mode report
     */
    if (test_mode == false) {
	para("",
	     "If the contest is still open, you may upload the above",
	     "tarball to the following submission URL:",
	     "",
	     NULL);
	ret = printf("    %s\n\n", IOCCC_SUBMIT_URL);
	if (ret < 0) {
	    errp(232, __func__, "printf #4 error");
	    not_reached();
	}
    }
    return;
}


/*
 * cmdprintf - malloc a safer shell command line for use with system() and popen()
 *
 * given:
 *
 *      format - The format string, any % on this string inserts the next string from the list,
 *               escaping special characters that the shell might threaten as command characters.
 *               In the worst case, the algorithm will make twice as many characters.
 *               Will not use escaping if it isn't needed.
 *
 * returns:
 *	malloced shell command line, or
 *	NULL ==> error
 *
 * NOTE: This code is base on an enhancement request by GitHub user @ilyakurdyukov:
 *
 *		https://github.com/ioccc-src/mkiocccentry/issues/11
 *
 *	 Thank you Ilya Kurdyukov!
 */
static char *
cmdprintf(char const *format, ...)
{
    va_list va;
    size_t size = 0;
    char const *next;
    char const *p;
    char const *f;
    char const *esc = "\t\n\r !\"#$&()*;<=>?[\\]^`{|}~";
    char *d;
    char *cmd;
    char c;
    int nquot;

    /*
     * determine how much storage we will need for the command line
     */
    va_start(va, format);
    f = format;
    while ((c = *f++)) {
	if (c == '%') {
	    p = next = va_arg(va, char const *);
	    nquot = 0;
	    while ((c = *p++)) {
		if (c == '\'') {
		    /* nquot >= 2: 'x##x' */
		    /* nquot == 1: x\#xx */
		    /* nquot == 0: xxxx */
		    /* +1 for escaping the single quote */
		    size += (size_t)(nquot >= 2 ? 2 : nquot) + 1;
		    nquot = 0;
		} else {
		    /* count the characters need to escape */
		    nquot += strchr(esc, c) != NULL;
		}
	    }
	    /* -2 for excluding counted NUL and */
	    /* counted % sign in the format string */
	    size += (size_t)(nquot >= 2 ? 2 : nquot) + (size_t)(p - next) - 2;
	}
    }
    va_end(va);
    size += (size_t)(f - format);

    /*
     * malloc storage or return NULL
     */
    errno = 0;			/* pre-clear errno for errp() */
    cmd = (char *)malloc(size);	/* trailing zero included in size */
    if (cmd == NULL) {
	warnp(__func__, "malloc from the cmdprintf of %lu bytes failed", (unsigned long)size);
	return NULL;
    }

    /*
     * form the safer command line
     */
    d = cmd;
    va_start(va, format);
    f = format;
    while ((c = *f++)) {
	if (c != '%') {
	    *d++ = c;
	} else {
	    p = next = va_arg(va, char const *);
	    nquot = 0;
	    while ((c = *p++)) {
		if (c == '\'') {
		    if (nquot >= 2) {
			*d++ = '\'';
		    }
		    while (next < p - 1) {
			c = *next++;
			/* nquot == 1 means one character needs to be escaped */
			/* quotes around are not used in this mode */
			if (nquot == 1 && strchr(esc, c)) {
			    *d++ = '\\';
			    /* set nquot to zero since we processed it */
			    /* to not call strchr() again */
			    nquot = 0;
			}
			*d++ = c;
		    }
		    if (nquot >= 2) {
			*d++ = '\'';
		    }
		    nquot = 0;
		    next++;
		    *d++ = '\\';
		    *d++ = '\'';
		} else {
		    nquot += strchr(esc, c) != NULL;
		}
	    }

	    if (nquot >= 2) {
		*d++ = '\'';
	    }
	    while (next < p - 1) {
		c = *next++;
		if (nquot == 1 && strchr(esc, c)) {
		    *d++ = '\\';
		    nquot = 0;
		}
		*d++ = c;
	    }
	    if (nquot >= 2) {
		*d++ = '\'';
	    }

	}
    }
    va_end(va);
    *d = '\0';	/* NUL terminate command line */

    if ((size_t)(d + 1 - cmd) != size) {
	errp(233, __func__, "cmdprintf: written characters (%ld) don't match the size (%lu)",
			    (long)(d + 1 - cmd), (unsigned long)size);
	not_reached();
    }

    /*
     * return safer command line string
     */
    return cmd;
}
