--- ref/auth.reference.json.c	2025-02-22 23:00:28
+++ chk_sem_auth.c	2025-02-22 23:55:51
@@ -40,9 +40,9 @@
 struct json_sem sem_auth[SEM_AUTH_LEN+1] = {
 /* depth    type        min     max   count   index  name_len validate  name    data */
   { 5,	JTYPE_NUMBER,	1,	5,	5,	0,	0,	NULL,	NULL,	NULL },
-  { 5,	JTYPE_STRING,	1,	104,	104,	1,	0,	NULL,	NULL,	NULL },
-  { 5,	JTYPE_BOOL,	1,	10,	10,	2,	0,	NULL,	NULL,	NULL },
-  { 5,	JTYPE_NULL,	1,	1,	1,	3,	0,	NULL,	NULL,	NULL },
+  { 5,	JTYPE_STRING,	15,	104,	104,	1,	0,	NULL,	NULL,	NULL },
+  { 5,	JTYPE_BOOL,	2,	10,	10,	2,	0,	NULL,	NULL,	NULL },
+  { 5,	JTYPE_NULL,	0,	30,	30,	3,	0,	NULL,	NULL,	NULL },
   { 4,	JTYPE_MEMBER,	1,	5,	5,	4,	11,	chk_affiliation,	"affiliation",	NULL },
   { 4,	JTYPE_MEMBER,	1,	5,	5,	5,	7,	chk_alt_url,	"alt_url",	NULL },
   { 4,	JTYPE_MEMBER,	1,	5,	5,	6,	13,	chk_author_handle,	"author_handle",	NULL },
@@ -56,8 +56,8 @@
   { 4,	JTYPE_MEMBER,	1,	5,	5,	14,	19,	chk_past_winning_author,	"past_winning_author",	NULL },
   { 4,	JTYPE_MEMBER,	1,	5,	5,	15,	3,	chk_url,	"url",	NULL },
   { 3,	JTYPE_OBJECT,	1,	5,	5,	16,	0,	NULL,	NULL,	NULL },
-  { 2,	JTYPE_NUMBER,	1,	6,	6,	17,	0,	NULL,	NULL,	NULL },
-  { 2,	JTYPE_STRING,	1,	26,	26,	18,	0,	NULL,	NULL,	NULL },
+  { 2,	JTYPE_NUMBER,	6,	6,	6,	17,	0,	NULL,	NULL,	NULL },
+  { 2,	JTYPE_STRING,	26,	26,	26,	18,	0,	NULL,	NULL,	NULL },
   { 2,	JTYPE_BOOL,	1,	1,	1,	19,	0,	NULL,	NULL,	NULL },
   { 2,	JTYPE_ARRAY,	1,	1,	1,	20,	0,	NULL,	NULL,	NULL },
   { 1,	JTYPE_MEMBER,	1,	1,	1,	21,	18,	chk_IOCCC_auth_version,	"IOCCC_auth_version",	NULL },
