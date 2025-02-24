--- ref/info.reference.json.h	2025-02-22 23:00:28
+++ chk_sem_info.h	2025-02-22 23:55:51
@@ -33,12 +33,12 @@
 /*
  * json_sem - JSON semantics support
  */
-#include "../../jparse/json_sem.h"
+#include "../jparse/json_sem.h"
 
 
 #if !defined(SEM_INFO_LEN)
 
-#define SEM_INFO_LEN (50)
+#define SEM_INFO_LEN (51)
 
 extern struct json_sem sem_info[SEM_INFO_LEN+1];
 
