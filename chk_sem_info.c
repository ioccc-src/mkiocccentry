/*
 * chk_sem_info - check .info.json semantics
 *
 * "Because grammar and syntax alone do not make a complete language." :-)
 *
 * The concept of this file was developed by:
 *
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * This file was auto-generated by:
 *
 *	make chk_sem_info.c
 */


/* special comments for the seqcexit tool */
/*ooo*/ /* exit code out of numerical order - ignore in sequencing */
/*coo*/ /* exit code change of order - use new value in sequencing */


/*
 * chk_sem_info - check .info.json semantics
 */
#include "chk_sem_info.h"


struct json_sem sem_info[SEM_INFO_LEN] = {
/* depth    type        min     max   count  name_len validate  name */
  { 5,	JTYPE_STRING,	12,	INF,	0,	0,	NULL,	NULL },
  { 4,	JTYPE_MEMBER,	1,	1,	0,	8,	chk_Makefile,	"Makefile" },
  { 4,	JTYPE_MEMBER,	1,	1,	0,	11,	chk_author_JSON,	"author_JSON" },
  { 4,	JTYPE_MEMBER,	1,	1,	0,	5,	chk_c_src,	"c_src" },
  { 4,	JTYPE_MEMBER,	0,	INF,	0,	10,	chk_extra_file,	"extra_file" },
  { 4,	JTYPE_MEMBER,	1,	1,	0,	9,	chk_info_JSON,	"info_JSON" },
  { 4,	JTYPE_MEMBER,	1,	1,	0,	7,	chk_remarks,	"remarks" },
  { 3,	JTYPE_OBJECT,	6,	6,	0,	0,	NULL,	NULL },
  { 2,	JTYPE_NUMBER,	7,	7,	0,	0,	NULL,	NULL },
  { 2,	JTYPE_STRING,	52,	52,	0,	0,	NULL,	NULL },
  { 2,	JTYPE_BOOL,	16,	16,	0,	0,	NULL,	NULL },
  { 2,	JTYPE_ARRAY,	1,	1,	0,	0,	NULL,	NULL },
  { 1,	JTYPE_MEMBER,	1,	1,	0,	16,	chk_IOCCC_contest_id,	"IOCCC_contest_id" },
  { 1,	JTYPE_MEMBER,	1,	1,	0,	18,	chk_IOCCC_info_version,	"IOCCC_info_version" },
  { 1,	JTYPE_MEMBER,	1,	1,	0,	17,	chk_Makefile_override,	"Makefile_override" },
  { 1,	JTYPE_MEMBER,	1,	1,	0,	8,	chk_abstract,	"abstract" },
  { 1,	JTYPE_MEMBER,	1,	1,	0,	16,	chk_chkentry_version,	"chkentry_version" },
  { 1,	JTYPE_MEMBER,	1,	1,	0,	14,	chk_empty_override,	"empty_override" },
  { 1,	JTYPE_MEMBER,	1,	1,	0,	9,	chk_entry_num,	"entry_num" },
  { 1,	JTYPE_MEMBER,	1,	1,	0,	17,	chk_first_rule_is_all,	"first_rule_is_all" },
  { 1,	JTYPE_MEMBER,	1,	1,	0,	15,	chk_fnamchk_version,	"fnamchk_version" },
  { 1,	JTYPE_MEMBER,	1,	1,	0,	10,	chk_formed_UTC,	"formed_UTC" },
  { 1,	JTYPE_MEMBER,	1,	1,	0,	16,	chk_formed_timestamp,	"formed_timestamp" },
  { 1,	JTYPE_MEMBER,	1,	1,	0,	21,	chk_formed_timestamp_usec,	"formed_timestamp_usec" },
  { 1,	JTYPE_MEMBER,	1,	1,	0,	14,	chk_found_all_rule,	"found_all_rule" },
  { 1,	JTYPE_MEMBER,	1,	1,	0,	16,	chk_found_clean_rule,	"found_clean_rule" },
  { 1,	JTYPE_MEMBER,	1,	1,	0,	18,	chk_found_clobber_rule,	"found_clobber_rule" },
  { 1,	JTYPE_MEMBER,	1,	1,	0,	14,	chk_found_try_rule,	"found_try_rule" },
  { 1,	JTYPE_MEMBER,	1,	1,	0,	15,	chk_highbit_warning,	"highbit_warning" },
  { 1,	JTYPE_MEMBER,	1,	1,	0,	13,	chk_ioccc_contest,	"ioccc_contest" },
  { 1,	JTYPE_MEMBER,	1,	1,	0,	10,	chk_ioccc_year,	"ioccc_year" },
  { 1,	JTYPE_MEMBER,	1,	1,	0,	17,	chk_iocccsize_version,	"iocccsize_version" },
  { 1,	JTYPE_MEMBER,	1,	1,	0,	8,	chk_manifest,	"manifest" },
  { 1,	JTYPE_MEMBER,	1,	1,	0,	13,	chk_min_timestamp,	"min_timestamp" },
  { 1,	JTYPE_MEMBER,	1,	1,	0,	20,	chk_mkiocccentry_version,	"mkiocccentry_version" },
  { 1,	JTYPE_MEMBER,	1,	1,	0,	10,	chk_no_comment,	"no_comment" },
  { 1,	JTYPE_MEMBER,	1,	1,	0,	11,	chk_nul_warning,	"nul_warning" },
  { 1,	JTYPE_MEMBER,	1,	1,	0,	16,	chk_rule_2a_mismatch,	"rule_2a_mismatch" },
  { 1,	JTYPE_MEMBER,	1,	1,	0,	16,	chk_rule_2a_override,	"rule_2a_override" },
  { 1,	JTYPE_MEMBER,	1,	1,	0,	12,	chk_rule_2a_size,	"rule_2a_size" },
  { 1,	JTYPE_MEMBER,	1,	1,	0,	16,	chk_rule_2b_override,	"rule_2b_override" },
  { 1,	JTYPE_MEMBER,	1,	1,	0,	12,	chk_rule_2b_size,	"rule_2b_size" },
  { 1,	JTYPE_MEMBER,	1,	1,	0,	7,	chk_tarball,	"tarball" },
  { 1,	JTYPE_MEMBER,	1,	1,	0,	9,	chk_test_mode,	"test_mode" },
  { 1,	JTYPE_MEMBER,	1,	1,	0,	15,	chk_timestamp_epoch,	"timestamp_epoch" },
  { 1,	JTYPE_MEMBER,	1,	1,	0,	5,	chk_title,	"title" },
  { 1,	JTYPE_MEMBER,	1,	1,	0,	16,	chk_trigraph_warning,	"trigraph_warning" },
  { 1,	JTYPE_MEMBER,	1,	1,	0,	14,	chk_txzchk_version,	"txzchk_version" },
  { 1,	JTYPE_MEMBER,	1,	1,	0,	14,	chk_ungetc_warning,	"ungetc_warning" },
  { 1,	JTYPE_MEMBER,	1,	1,	0,	15,	chk_wordbuf_warning,	"wordbuf_warning" },
  { 0,	JTYPE_OBJECT,	1,	1,	0,	0,	NULL,	NULL },
};
