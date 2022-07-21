--- ref/info.reference.json.c	2022-07-21 01:57:45.000000000 -0700
+++ chk_sem_info.c	2022-07-21 02:05:35.000000000 -0700
@@ -1,10 +1,10 @@
 struct json_sem sem_info[SEM_INFO_LEN] = {
 /* depth    type        min     max   count  name_len validate  name */
-  { 5,	JTYPE_STRING,	12,	12,	0,	0,	NULL,	NULL },
+  { 5,	JTYPE_STRING,	12,	INF,	0,	0,	NULL,	NULL },
   { 4,	JTYPE_MEMBER,	1,	1,	0,	8,	chk_Makefile,	"Makefile" },
   { 4,	JTYPE_MEMBER,	1,	1,	0,	11,	chk_author_JSON,	"author_JSON" },
   { 4,	JTYPE_MEMBER,	1,	1,	0,	5,	chk_c_src,	"c_src" },
-  { 4,	JTYPE_MEMBER,	1,	1,	0,	10,	chk_extra_file,	"extra_file" },
+  { 4,	JTYPE_MEMBER,	0,	INF,	0,	10,	chk_extra_file,	"extra_file" },
   { 4,	JTYPE_MEMBER,	1,	1,	0,	9,	chk_info_JSON,	"info_JSON" },
   { 4,	JTYPE_MEMBER,	1,	1,	0,	7,	chk_remarks,	"remarks" },
   { 3,	JTYPE_OBJECT,	6,	6,	0,	0,	NULL,	NULL },
