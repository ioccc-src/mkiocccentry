--- ref/auth.reference.json.h	2023-01-09 08:20:24
+++ chk_sem_auth.h	2023-01-09 08:19:43
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
@@ -67,6 +65,8 @@
 extern bool chk_past_winner(struct json const *node,
 	unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err);
 extern bool chk_url(struct json const *node,
+	unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err);
+extern bool chk_alt_url(struct json const *node,
 	unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err);
 extern bool chk_IOCCC_auth_version(struct json const *node,
 	unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err);
