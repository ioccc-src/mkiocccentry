--- ref/author.reference.json.c	2022-10-28 01:30:52
+++ chk_sem_auth.c	2022-10-28 01:30:52
@@ -37,9 +37,9 @@
 struct json_sem sem_auth[SEM_AUTH_LEN+1] = {
 /* depth    type        min     max   count   index  name_len validate  name */
   { 5,	JTYPE_NUMBER,	1,	5,	5,	0,	0,	NULL,	NULL },
-  { 5,	JTYPE_STRING,	1,	105,	105,	1,	0,	NULL,	NULL },
-  { 5,	JTYPE_BOOL,	1,	9,	9,	2,	0,	NULL,	NULL },
-  { 5,	JTYPE_NULL,	1,	1,	1,	3,	0,	NULL,	NULL },
+  { 5,	JTYPE_STRING,	16,	105,	105,	1,	0,	NULL,	NULL },
+  { 5,	JTYPE_BOOL,	2,	9,	9,	2,	0,	NULL,	NULL },
+  { 5,	JTYPE_NULL,	0,	25,	25,	3,	0,	NULL,	NULL },
   { 4,	JTYPE_MEMBER,	1,	5,	5,	4,	11,	chk_affiliation,	"affiliation" },
   { 4,	JTYPE_MEMBER,	1,	5,	5,	5,	13,	chk_author_handle,	"author_handle" },
   { 4,	JTYPE_MEMBER,	1,	5,	5,	6,	13,	chk_author_number,	"author_number" },
@@ -53,8 +53,8 @@
   { 4,	JTYPE_MEMBER,	1,	5,	5,	14,	7,	chk_twitter,	"twitter" },
   { 4,	JTYPE_MEMBER,	1,	5,	5,	15,	3,	chk_url,	"url" },
   { 3,	JTYPE_OBJECT,	1,	5,	5,	16,	0,	NULL,	NULL },
-  { 2,	JTYPE_NUMBER,	1,	6,	6,	17,	0,	NULL,	NULL },
-  { 2,	JTYPE_STRING,	1,	28,	28,	18,	0,	NULL,	NULL },
+  { 2,	JTYPE_NUMBER,	6,	6,	6,	17,	0,	NULL,	NULL },
+  { 2,	JTYPE_STRING,	28,	28,	28,	18,	0,	NULL,	NULL },
   { 2,	JTYPE_BOOL,	1,	1,	1,	19,	0,	NULL,	NULL },
   { 2,	JTYPE_ARRAY,	1,	1,	1,	20,	0,	NULL,	NULL },
   { 1,	JTYPE_MEMBER,	1,	1,	1,	21,	20,	chk_IOCCC_author_version,	"IOCCC_author_version" },
