--- ref/auth.reference.json.h	2025-06-24 08:43:22
+++ chk_sem_auth.h	2025-06-24 08:38:33
@@ -33,19 +33,17 @@
 /*
  * json_sem - JSON semantics support
  */
-#include "../../jparse/json_sem.h"
+#include "../jparse/json_sem.h"
 
 
 #if !defined(SEM_AUTH_LEN)
 
-#define SEM_AUTH_LEN (39)
+#define SEM_AUTH_LEN (40)
 
 extern struct json_sem sem_auth[SEM_AUTH_LEN+1];
 
 extern bool chk_affiliation(struct json const *node,
 	unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err);
-extern bool chk_alt_url(struct json const *node,
-	unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err);
 extern bool chk_author_handle(struct json const *node,
 	unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err);
 extern bool chk_author_number(struct json const *node,
@@ -66,6 +64,8 @@
 	unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err);
 extern bool chk_url(struct json const *node,
 	unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err);
+extern bool chk_alt_url(struct json const *node,
+	unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err);
 extern bool chk_IOCCC_auth_version(struct json const *node,
 	unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err);
 extern bool chk_IOCCC_contest(struct json const *node,
