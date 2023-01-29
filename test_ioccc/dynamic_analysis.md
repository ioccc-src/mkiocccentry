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

## Issue: memory leaks in mkiocccentry.c
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

Fixed in commit 5eb62121b6298e5ecc5324c6c1fd13a7f904ee0b.


## Issue: memory leaks in txzchk.c
### Status: fixed

### Example

```
==2085869== 120 bytes in 1 blocks are definitely lost in loss record 1 of 2
==2085869==    at 0x484586F: malloc (vg_replace_malloc.c:381)
==2085869==    by 0x48EE332: getdelim (iogetdelim.c:62)
==2085869==    by 0x408C1C: readline (util.c:1627)
==2085869==    by 0x403208: check_tarball (txzchk.c:1429)
==2085869==    by 0x402866: main (txzchk.c:186)

```

### Solution

Add the missing call to free the variable `dir_name`.

### Example

```
==2086037== 1,320 bytes in 11 blocks are definitely lost in loss record 1 of 1
==2086037==    at 0x484586F: malloc (vg_replace_malloc.c:381)
==2086037==    by 0x48EE332: getdelim (iogetdelim.c:62)
==2086037==    by 0x408C2C: readline (util.c:1627)
==2086037==    by 0x40360D: check_tarball (txzchk.c:1529)
==2086037==    by 0x402866: main (txzchk.c:186)
```


### Solution

Free the variable `linep` in three spots: when using `continue` in the loop due
to an error, at the end of each iteration of the loop and at the end of the
function.

### See also

Fixed in commit 290963a459b707cc6b22374cc20dbd76e9f5db6d.

## Issue: memory leaks in jstrencode.c and jstrdecode.c
### Status: partially fixed
### Example

```
==2089136== HEAP SUMMARY:
==2089136==     in use at exit: 2,245,253 bytes in 3 blocks
==2089136==   total heap usage: 10 allocs, 7 frees, 3,564,165 bytes allocated
==2089136==
==2089136== 458,752 bytes in 1 blocks are indirectly lost in loss record 1 of 3
==2089136==    at 0x484A6AF: realloc (vg_replace_malloc.c:1437)
==2089136==    by 0x40E057: dyn_array_grow (dyn_array.c:164)
==2089136==    by 0x40E457: dyn_array_seek (dyn_array.c:1385)
==2089136==    by 0x40AEEC: read_all (util.c:1904)
==2089136==    by 0x40289F: jstrencode_stream (jstrencode.c:88)
==2089136==    by 0x40275B: main (jstrencode.c:258)
==2089136==
==2089136== 1,786,453 bytes in 1 blocks are indirectly lost in loss record 2 of 3
==2089136==    at 0x484586F: malloc (vg_replace_malloc.c:381)
==2089136==    by 0x402AA4: json_encode (json_parse.c:257)
==2089136==    by 0x4028D5: jstrencode_stream (jstrencode.c:98)
==2089136==    by 0x40275B: main (jstrencode.c:258)
==2089136==
==2089136== 2,245,253 (48 direct, 2,245,205 indirect) bytes in 1 blocks are definitely lost in loss record 3 of 3
==2089136==    at 0x484A464: calloc (vg_replace_malloc.c:1328)
==2089136==    by 0x40CDB0: dyn_array_create (dyn_array.c:666)
==2089136==    by 0x40AEBB: read_all (util.c:1892)
==2089136==    by 0x40289F: jstrencode_stream (jstrencode.c:88)
==2089136==    by 0x40275B: main (jstrencode.c:258)
==2089136==
==2089136== LEAK SUMMARY:
==2089136==    definitely lost: 48 bytes in 1 blocks
==2089136==    indirectly lost: 2,245,205 bytes in 2 blocks
==2089136==      possibly lost: 0 bytes in 0 blocks
==2089136==    still reachable: 0 bytes in 0 blocks
==2089136==         suppressed: 0 bytes in 0 blocks

```

The same problem existed in `jstrdecode.c` as well.

### Solution

In `jstrencode_stream()` there was a typo `==` instead of `!=` prior to freeing
`buf`.

As well the `char *` `input` was not freed at all.

These same issues were in `jstrdecode_stream()`.

Now the memory loss count is (for both but here only showing for jstrencode):

```
==2242762== 48 bytes in 1 blocks are definitely lost in loss record 1 of 1
==2242762==    at 0x484A464: calloc (vg_replace_malloc.c:1328)
==2242762==    by 0x40CDD0: dyn_array_create (dyn_array.c:666)
==2242762==    by 0x40AEDB: read_all (util.c:1892)
==2242762==    by 0x4028A3: jstrencode_stream (jstrencode.c:88)
==2242762==    by 0x40275B: main (jstrencode.c:266)
==2242762==
==2242762== LEAK SUMMARY:
==2242762==    definitely lost: 48 bytes in 1 blocks
==2242762==    indirectly lost: 0 bytes in 0 blocks
==2242762==      possibly lost: 0 bytes in 0 blocks
==2242762==    still reachable: 0 bytes in 0 blocks
==2242762==         suppressed: 0 bytes in 0 blocks
```

As can be seen there still are some bytes that are not freed. It appears to be
because `read_all` uses the dynamic array facility and although the buffer
`input` itself is freed the array is not. This is just a guess.

If this is the case it is probably not worth trying to fix because it would
complicate the code. This is TBD.

For reference: these were detected via valgrind in the `jstr_test.sh` script.

### See also

Fixed in commit 670dc207b58dbeb0f0afd24e2f63fa7bf609daa2.


## Issue: memory leak in jsemtblgen.c
### Status: partially fixed
### Example

```
==2258030== 9 bytes in 1 blocks are still reachable in loss record 1 of 13
==2258030==    at 0x484586F: malloc (vg_replace_malloc.c:381)
==2258030==    by 0x49F14FD: strdup (strdup.c:42)
==2258030==    by 0x4025A5: main (jsemtblgen.c:306)
```

### Solution

Add missing call to the cap_tbl_name variable.

### Example

### See also

Fixed in commit fa9eca1c273fdd138d338a93875f462f62b0bc5b: Fix memory leak in
jsemtblgen.c.

Note that there are other memory leaks in the json parser itself. Whether these
are worth fixing is TBD later.


## Issue: memory leaks in `dyn_test.c`
### Status: fixed
### Example

```
==2257667== 48 bytes in 1 blocks are still reachable in loss record 1 of 2
==2257667==    at 0x484A464: calloc (vg_replace_malloc.c:1328)
==2257667==    by 0x401851: dyn_array_create (dyn_array.c:666)
==2257667==    by 0x40124E: main (dyn_test.c:105)
==2257667==
==2257667== 16,015,360 bytes in 1 blocks are still reachable in loss record 2 of 2
==2257667==    at 0x484A6AF: realloc (vg_replace_malloc.c:1437)
==2257667==    by 0x401631: dyn_array_grow (dyn_array.c:164)
==2257667==    by 0x401C14: dyn_array_append_set (dyn_array.c:904)
==2257667==    by 0x40131F: main (dyn_test.c:140)
```

### Solution

Add the appropriate call to `dyn_array_free`. Additionally add a call to
`free(array);` at the end of `dyn_array_free()`. Indeed it is ironic that a
function that frees memory did not free everything but there it is.

### See also

Fixed in commit 6c4138d788614f138013f4497b2ea1631923ab5b: Plug memory leaks in
dyn_{array,test}.c.


## Reporting issues

If you notice any errors or warnings with the above please report them.
Alternatively if you have a fix you can open a pull request.
