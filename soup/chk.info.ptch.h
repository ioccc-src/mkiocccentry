--- ref/info.reference.json.h	2025-07-09 08:36:55
+++ chk_sem_info.h	2025-07-09 08:30:33
@@ -33,12 +33,12 @@
 /*
  * json_sem - JSON semantics support
  */
-#include "../../jparse/json_sem.h"
+#include "../jparse/json_sem.h"
 
 
 #if !defined(SEM_INFO_LEN)
 
-#define SEM_INFO_LEN (46)
+#define SEM_INFO_LEN (51)
 
 extern struct json_sem sem_info[SEM_INFO_LEN+1];
 
@@ -82,6 +82,8 @@
 	unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err);
 extern bool chk_found_clobber_rule(struct json const *node,
 	unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err);
+extern bool chk_found_try_rule(struct json const *node,
+	unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err);
 extern bool chk_iocccsize_version(struct json const *node,
 	unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err);
 extern bool chk_manifest(struct json const *node,
