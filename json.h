#ifndef JSON_H
#define JSON_H

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
    bool first_rule_is_all;	/* true ==> Makefile first rule is all */
    bool found_all_rule;	/* true ==> Makefile has an all rule */
    bool found_clean_rule;	/* true ==> Makefile has clean rule */
    bool found_clobber_rule;	/* true ==> Makefile has a clobber rule */
    bool found_try_rule;	/* true ==> Makefile has a try rule */
    unsigned answers_errors;	/* > 0 ==> flushing or closing answers file failed */
    /*
     * filenames
     */
    char *prog_c;		/* prog.c filename */
    char *Makefile;		/* Makefile filename */
    char *remarks_md;		/* remarks.md filename */
    int extra_count;		/* number of extra files */
    char **extra_file;		/* list of extra filenames followed by NULL */
    char *tarball_path;		/* tarball filename */
    /*
     * time
     */
    time_t tstamp;		/* seconds since epoch when .info json was formed (see gettimeofday(2)) */
    int usec;			/* microseconds since the tstamp second */
    char *epoch;		/* epoch of tstamp, currently: Thr Jan 1 00:00:00 1970 UTC */
    char *gmtime;		/* UTC converted string for tstamp (see asctime(3)) */
};




#endif /* JSON_H */
