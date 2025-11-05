--- ref/info.reference.json.c	2025-11-04 05:14:30
+++ chk_sem_info.c	2025-11-04 05:13:33
@@ -39,21 +39,21 @@
 
 struct json_sem sem_info[SEM_INFO_LEN+1] = {
 /* depth    type        min     max   count   index  name_len validate  name    data */
-  { 5,	JTYPE_STRING,	1,	80,	80,	0,	0,	NULL,	NULL,	NULL },
+  { 5,	JTYPE_STRING,	10,	80,	80,	0,	0,	NULL,	NULL,	NULL },
   { 4,	JTYPE_MEMBER,	1,	1,	1,	1,	8,	chk_Makefile,	"Makefile",	NULL },
   { 4,	JTYPE_MEMBER,	1,	1,	1,	2,	9,	chk_auth_JSON,	"auth_JSON",	NULL },
-  { 4,	JTYPE_MEMBER,	1,	1,	1,	3,	9,	chk_c_alt_src,	"c_alt_src",	NULL },
+  { 4,	JTYPE_MEMBER,	0,	1,	1,	3,	9,	chk_c_alt_src,	"c_alt_src",	NULL },
   { 4,	JTYPE_MEMBER,	1,	1,	1,	4,	5,	chk_c_src,	"c_src",	NULL },
-  { 4,	JTYPE_MEMBER,	1,	31,	31,	5,	10,	chk_extra_file,	"extra_file",	NULL },
+  { 4,	JTYPE_MEMBER,	0,	31,	31,	5,	10,	chk_extra_file,	"extra_file",	NULL },
   { 4,	JTYPE_MEMBER,	1,	1,	1,	6,	9,	chk_info_JSON,	"info_JSON",	NULL },
   { 4,	JTYPE_MEMBER,	1,	1,	1,	7,	7,	chk_remarks,	"remarks",	NULL },
-  { 4,	JTYPE_MEMBER,	1,	1,	1,	8,	12,	chk_shell_script,	"shell_script",	NULL },
-  { 4,	JTYPE_MEMBER,	1,	1,	1,	9,	10,	chk_try_alt_sh,	"try_alt_sh",	NULL },
-  { 4,	JTYPE_MEMBER,	1,	1,	1,	10,	6,	chk_try_sh,	"try_sh",	NULL },
-  { 3,	JTYPE_OBJECT,	1,	40,	40,	11,	0,	NULL,	NULL,	NULL },
-  { 2,	JTYPE_NUMBER,	1,	7,	7,	12,	0,	NULL,	NULL,	NULL },
-  { 2,	JTYPE_STRING,	1,	45,	45,	13,	0,	NULL,	NULL,	NULL },
-  { 2,	JTYPE_BOOL,	1,	11,	11,	14,	0,	NULL,	NULL,	NULL },
+  { 4,	JTYPE_MEMBER,	0,	31,	31,	8,	12,	chk_shell_script,	"shell_script",	NULL },
+  { 4,	JTYPE_MEMBER,	0,	1,	1,	9,	10,	chk_try_alt_sh,	"try_alt_sh",	NULL },
+  { 4,	JTYPE_MEMBER,	0,	1,	1,	10,	6,	chk_try_sh,	"try_sh",	NULL },
+  { 3,	JTYPE_OBJECT,	0,	40,	40,	11,	0,	NULL,	NULL,	NULL },
+  { 2,	JTYPE_NUMBER,	7,	7,	7,	12,	0,	NULL,	NULL,	NULL },
+  { 2,	JTYPE_STRING,	41,	45,	45,	13,	0,	NULL,	NULL,	NULL },
+  { 2,	JTYPE_BOOL,	7,	11,	11,	14,	0,	NULL,	NULL,	NULL },
   { 2,	JTYPE_ARRAY,	1,	1,	1,	15,	0,	NULL,	NULL,	NULL },
   { 1,	JTYPE_MEMBER,	1,	1,	1,	16,	13,	chk_IOCCC_contest,	"IOCCC_contest",	NULL },
   { 1,	JTYPE_MEMBER,	1,	1,	1,	17,	16,	chk_IOCCC_contest_id,	"IOCCC_contest_id",	NULL },
