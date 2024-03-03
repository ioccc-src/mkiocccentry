--- ref/auth.reference.json.h	2024-03-02 05:59:12
+++ chk_sem_auth.h	2024-03-02 05:58:35
@@ -33,7 +33,7 @@
 /*
  * json_sem - JSON semantics support
  */
-#include "../../jparse/json_sem.h"
+#include "../jparse/json_sem.h"
 
 
 #if !defined(SEM_AUTH_LEN)
@@ -44,8 +44,6 @@
 
 extern bool chk_affiliation(struct json const *node,
 	unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err);
-extern bool chk_alt_url(struct json const *node,
-	unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err);
 extern bool chk_author_handle(struct json const *node,
 	unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err);
 extern bool chk_author_number(struct json const *node,
@@ -68,6 +66,8 @@
 	unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err);
 extern bool chk_url(struct json const *node,
 	unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err);
+extern bool chk_alt_url(struct json const *node,
+	unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err);
 extern bool chk_IOCCC_auth_version(struct json const *node,
 	unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err);
 extern bool chk_IOCCC_contest(struct json const *node,
