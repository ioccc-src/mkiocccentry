# Dynamic analysis of the code

Although we cannot provide you with everything to perform dynamic analysis of
the code we can at least guide you with some.

This is for now a very brief document that talks about how to use valgrind:
nothing less and nothing more. We might update this later on with more
information at which point we'll remove this notice.

## valgrind

If you use `valgrind`, run each tool with:

	valgrind --leak-check=yes --track-origins=yes --leak-resolution=high --read-var-info=yes \
           --leak-check=full --show-leak-kinds=all ./mkiocccentry ...


making sure to replace `mkiocccentry` with whichever tool you want to test and the `...`
with the arguments and options you want.

## Issue: N bytes in M blocks are definitely lost in mkiocccentry.c
### Status: fixed
### Example


```
==2082951== 85 bytes in 5 blocks are definitely lost in loss record 2 of 3
==2082951==    at 0x484A464: calloc (vg_replace_malloc.c:1328)
==2082951==    by 0x41AAE9: readline_dup (util.c:1720)
==2082951==    by 0x40DC74: prompt (mkiocccentry.c:1312)
==2082951==    by 0x4088A1: get_author_info (mkiocccentry.c:3921)
==2082951==    by 0x403690: main (mkiocccentry.c:526)

```

### Solution

In this case a missing call to free() for the `alt_url` in the function
`free_author_array()` in `entry_util.c` was added.

### Example

```
==2083295== 13 bytes in 1 blocks are still reachable in loss record 1 of 2
==2083295==    at 0x484A464: calloc (vg_replace_malloc.c:1328)
==2083295==    by 0x41AAF9: readline_dup (util.c:1720)
==2083295==    by 0x403A04: main (mkiocccentry.c:607)
```

### Solution

Add the missing call to `free(line)` and set to `NULL` at the end of the block.


### Example

```
==2084790== 480 bytes in 1 blocks are still reachable in loss record 1 of 1
==2084790==    at 0x484586F: malloc (vg_replace_malloc.c:381)
==2084790==    by 0x407530: get_author_info (mkiocccentry.c:3450)
==2084790==    by 0x403690: main (mkiocccentry.c:526)
==2084790==

```

### Solution

The problem was that in the function `free_author_array()` each element of the
array was freed but `author_set` itself was not freed. Adding a call free it and
the problem is resolved.

### See also

Fixed in commit 66f539b70c2fa2a54dd39cd8645bc7166b93b5b7.

## Reporting issues

If you notice any errors or warnings with the above please report them.
Alternatively if you have a fix you can open a pull request.
