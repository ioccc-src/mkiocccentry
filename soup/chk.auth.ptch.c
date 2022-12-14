--- soup/ref/auth.reference.json.c	2022-12-13 07:46:35
+++ soup/chk_sem_auth.c	2022-12-13 07:47:55
@@ -40,9 +40,9 @@
 struct json_sem sem_auth[SEM_AUTH_LEN+1] = {
 /* depth    type        min     max   count   index  name_len validate  name */
   { 5,	JTYPE_NUMBER,	1,	5,	5,	0,	0,	NULL,	NULL },
-  { 5,	JTYPE_STRING,	1,	104,	104,	1,	0,	NULL,	NULL },
-  { 5,	JTYPE_BOOL,	1,	10,	10,	2,	0,	NULL,	NULL },
-  { 5,	JTYPE_NULL,	1,	1,	1,	3,	0,	NULL,	NULL },
+  { 5,	JTYPE_STRING,	16,	105,	105,	1,	0,	NULL,	NULL },
+  { 5,	JTYPE_BOOL,	2,	10,	10,	2,	0,	NULL,	NULL },
+  { 5,	JTYPE_NULL,	0,	25,	25,	3,	0,	NULL,	NULL },
   { 4,	JTYPE_MEMBER,	1,	5,	5,	4,	11,	chk_affiliation,	"affiliation" },
   { 4,	JTYPE_MEMBER,	1,	5,	5,	5,	13,	chk_author_handle,	"author_handle" },
   { 4,	JTYPE_MEMBER,	1,	5,	5,	6,	13,	chk_author_number,	"author_number" },
@@ -56,8 +56,8 @@
   { 4,	JTYPE_MEMBER,	1,	5,	5,	14,	11,	chk_past_winner,	"past_winner" },
   { 4,	JTYPE_MEMBER,	1,	5,	5,	15,	3,	chk_url,	"url" },
   { 3,	JTYPE_OBJECT,	1,	5,	5,	16,	0,	NULL,	NULL },
-  { 2,	JTYPE_NUMBER,	1,	6,	6,	17,	0,	NULL,	NULL },
-  { 2,	JTYPE_STRING,	1,	28,	28,	18,	0,	NULL,	NULL },
+  { 2,	JTYPE_NUMBER,	6,	6,	6,	17,	0,	NULL,	NULL },
+  { 2,	JTYPE_STRING,	28,	28,	28,	18,	0,	NULL,	NULL },
   { 2,	JTYPE_BOOL,	1,	1,	1,	19,	0,	NULL,	NULL },
   { 2,	JTYPE_ARRAY,	1,	1,	1,	20,	0,	NULL,	NULL },
   { 1,	JTYPE_MEMBER,	1,	1,	1,	21,	18,	chk_IOCCC_auth_version,	"IOCCC_auth_version" },
