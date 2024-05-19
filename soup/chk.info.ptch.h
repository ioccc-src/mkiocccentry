--- ref/info.reference.json.h	2024-05-19 00:08:13
+++ chk_sem_info.h	2024-05-18 23:59:08
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
 
