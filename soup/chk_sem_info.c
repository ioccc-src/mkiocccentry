/*
 * chk_sem_info - check .info.json semantics
 *
 * "Because grammar and syntax alone do not make a complete language." :-)
 *
 * The concept of this file was developed by:
 *
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * and it was was auto-generated by:
 *
 *	make chk_sem_info.c
 *
 * The JSON parser was co-developed by:
 *
 *	@xexyl
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


/* special comments for the seqcexit tool */
/* exit code out of numerical order - ignore in sequencing - ooo */
/* exit code change of order - use new value in sequencing - coo */


/*
 * chk_sem_info - check .info.json semantics
 */
#include "chk_sem_info.h"


struct json_sem sem_info[SEM_INFO_LEN+1] = {
/* depth    type        min     max   count   index  name_len validate  name */
  { 5,	JTYPE_STRING,	10,	84,	84,	0,	0,	NULL,	NULL },
  { 4,	JTYPE_MEMBER,	1,	1,	1,	1,	8,	chk_Makefile,	"Makefile" },
  { 4,	JTYPE_MEMBER,	1,	1,	1,	2,	11,	chk_auth_JSON,	"auth_JSON" },
  { 4,	JTYPE_MEMBER,	1,	1,	1,	3,	5,	chk_c_src,	"c_src" },
  { 4,	JTYPE_MEMBER,	0,	37,	37,	4,	10,	chk_extra_file,	"extra_file" },
  { 4,	JTYPE_MEMBER,	1,	1,	1,	5,	9,	chk_info_JSON,	"info_JSON" },
  { 4,	JTYPE_MEMBER,	1,	1,	1,	6,	7,	chk_remarks,	"remarks" },
  { 3,	JTYPE_OBJECT,	5,	42,	42,	7,	0,	NULL,	NULL },
  { 2,	JTYPE_NUMBER,	7,	7,	7,	8,	0,	NULL,	NULL },
  { 2,	JTYPE_STRING,	52,	52,	52,	9,	0,	NULL,	NULL },
  { 2,	JTYPE_BOOL,	16,	16,	16,	10,	0,	NULL,	NULL },
  { 2,	JTYPE_ARRAY,	1,	1,	1,	11,	0,	NULL,	NULL },
  { 1,	JTYPE_MEMBER,	1,	1,	1,	12,	16,	chk_IOCCC_contest_id,	"IOCCC_contest_id" },
  { 1,	JTYPE_MEMBER,	1,	1,	1,	13,	13,	chk_IOCCC_contest,	"IOCCC_contest" },
  { 1,	JTYPE_MEMBER,	1,	1,	1,	14,	18,	chk_IOCCC_info_version,	"IOCCC_info_version" },
  { 1,	JTYPE_MEMBER,	1,	1,	1,	15,	10,	chk_IOCCC_year,	"IOCCC_year" },
  { 1,	JTYPE_MEMBER,	1,	1,	1,	16,	17,	chk_Makefile_override,	"Makefile_override" },
  { 1,	JTYPE_MEMBER,	1,	1,	1,	17,	8,	chk_abstract,	"abstract" },
  { 1,	JTYPE_MEMBER,	1,	1,	1,	18,	16,	chk_chkentry_version,	"chkentry_version" },
  { 1,	JTYPE_MEMBER,	1,	1,	1,	19,	14,	chk_empty_override,	"empty_override" },
  { 1,	JTYPE_MEMBER,	1,	1,	1,	20,	9,	chk_entry_num,	"entry_num" },
  { 1,	JTYPE_MEMBER,	1,	1,	1,	21,	17,	chk_first_rule_is_all,	"first_rule_is_all" },
  { 1,	JTYPE_MEMBER,	1,	1,	1,	22,	15,	chk_fnamchk_version,	"fnamchk_version" },
  { 1,	JTYPE_MEMBER,	1,	1,	1,	23,	10,	chk_formed_UTC,	"formed_UTC" },
  { 1,	JTYPE_MEMBER,	1,	1,	1,	24,	16,	chk_formed_timestamp,	"formed_timestamp" },
  { 1,	JTYPE_MEMBER,	1,	1,	1,	25,	21,	chk_formed_timestamp_usec,	"formed_timestamp_usec" },
  { 1,	JTYPE_MEMBER,	1,	1,	1,	26,	14,	chk_found_all_rule,	"found_all_rule" },
  { 1,	JTYPE_MEMBER,	1,	1,	1,	27,	16,	chk_found_clean_rule,	"found_clean_rule" },
  { 1,	JTYPE_MEMBER,	1,	1,	1,	28,	18,	chk_found_clobber_rule,	"found_clobber_rule" },
  { 1,	JTYPE_MEMBER,	1,	1,	1,	29,	14,	chk_found_try_rule,	"found_try_rule" },
  { 1,	JTYPE_MEMBER,	1,	1,	1,	30,	15,	chk_highbit_warning,	"highbit_warning" },
  { 1,	JTYPE_MEMBER,	1,	1,	1,	31,	17,	chk_iocccsize_version,	"iocccsize_version" },
  { 1,	JTYPE_MEMBER,	1,	1,	1,	32,	8,	chk_manifest,	"manifest" },
  { 1,	JTYPE_MEMBER,	1,	1,	1,	33,	13,	chk_min_timestamp,	"min_timestamp" },
  { 1,	JTYPE_MEMBER,	1,	1,	1,	34,	20,	chk_mkiocccentry_version,	"mkiocccentry_version" },
  { 1,	JTYPE_MEMBER,	1,	1,	1,	35,	10,	chk_no_comment,	"no_comment" },
  { 1,	JTYPE_MEMBER,	1,	1,	1,	36,	11,	chk_nul_warning,	"nul_warning" },
  { 1,	JTYPE_MEMBER,	1,	1,	1,	37,	16,	chk_rule_2a_mismatch,	"rule_2a_mismatch" },
  { 1,	JTYPE_MEMBER,	1,	1,	1,	38,	16,	chk_rule_2a_override,	"rule_2a_override" },
  { 1,	JTYPE_MEMBER,	1,	1,	1,	39,	12,	chk_rule_2a_size,	"rule_2a_size" },
  { 1,	JTYPE_MEMBER,	1,	1,	1,	40,	16,	chk_rule_2b_override,	"rule_2b_override" },
  { 1,	JTYPE_MEMBER,	1,	1,	1,	41,	12,	chk_rule_2b_size,	"rule_2b_size" },
  { 1,	JTYPE_MEMBER,	1,	1,	1,	42,	7,	chk_tarball,	"tarball" },
  { 1,	JTYPE_MEMBER,	1,	1,	1,	43,	9,	chk_test_mode,	"test_mode" },
  { 1,	JTYPE_MEMBER,	1,	1,	1,	44,	15,	chk_timestamp_epoch,	"timestamp_epoch" },
  { 1,	JTYPE_MEMBER,	1,	1,	1,	45,	5,	chk_title,	"title" },
  { 1,	JTYPE_MEMBER,	1,	1,	1,	46,	16,	chk_trigraph_warning,	"trigraph_warning" },
  { 1,	JTYPE_MEMBER,	1,	1,	1,	47,	14,	chk_txzchk_version,	"txzchk_version" },
  { 1,	JTYPE_MEMBER,	1,	1,	1,	48,	14,	chk_ungetc_warning,	"ungetc_warning" },
  { 1,	JTYPE_MEMBER,	1,	1,	1,	49,	15,	chk_wordbuf_warning,	"wordbuf_warning" },
  { 0,	JTYPE_OBJECT,	1,	1,	1,	50,	0,	NULL,	NULL },
  { 0,	JTYPE_UNSET,	0,	0,	0,	-1,	0,	NULL,	NULL }
};