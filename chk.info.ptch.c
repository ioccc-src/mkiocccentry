--- ref/info.reference.json.c	2022-10-23 15:37:10.000000000 -0700
+++ chk_sem_info.c	2022-10-23 15:35:08.000000000 -0700
@@ -26,17 +26,17 @@
 
 struct json_sem sem_info[SEM_INFO_LEN+1] = {
 /* depth    type        min     max   count  name_len validate  name */
-  { 5,	JTYPE_STRING,	1,	12,	12,	0,	NULL,	NULL },
+  { 5,	JTYPE_STRING,	12,	INF,	12,	0,	NULL,	NULL },
   { 4,	JTYPE_MEMBER,	1,	1,	1,	8,	chk_Makefile,	"Makefile" },
   { 4,	JTYPE_MEMBER,	1,	1,	1,	11,	chk_author_JSON,	"author_JSON" },
   { 4,	JTYPE_MEMBER,	1,	1,	1,	5,	chk_c_src,	"c_src" },
-  { 4,	JTYPE_MEMBER,	1,	1,	1,	10,	chk_extra_file,	"extra_file" },
+  { 4,	JTYPE_MEMBER,	0,	INF,	1,	10,	chk_extra_file,	"extra_file" },
   { 4,	JTYPE_MEMBER,	1,	1,	1,	9,	chk_info_JSON,	"info_JSON" },
   { 4,	JTYPE_MEMBER,	1,	1,	1,	7,	chk_remarks,	"remarks" },
-  { 3,	JTYPE_OBJECT,	1,	6,	6,	0,	NULL,	NULL },
-  { 2,	JTYPE_NUMBER,	1,	7,	7,	0,	NULL,	NULL },
-  { 2,	JTYPE_STRING,	1,	52,	52,	0,	NULL,	NULL },
-  { 2,	JTYPE_BOOL,	1,	16,	16,	0,	NULL,	NULL },
+  { 3,	JTYPE_OBJECT,	6,	6,	6,	0,	NULL,	NULL },
+  { 2,	JTYPE_NUMBER,	7,	7,	7,	0,	NULL,	NULL },
+  { 2,	JTYPE_STRING,	52,	52,	52,	0,	NULL,	NULL },
+  { 2,	JTYPE_BOOL,	16,	16,	16,	0,	NULL,	NULL },
   { 2,	JTYPE_ARRAY,	1,	1,	1,	0,	NULL,	NULL },
   { 1,	JTYPE_MEMBER,	1,	1,	1,	16,	chk_IOCCC_contest_id,	"IOCCC_contest_id" },
   { 1,	JTYPE_MEMBER,	1,	1,	1,	18,	chk_IOCCC_info_version,	"IOCCC_info_version" },
