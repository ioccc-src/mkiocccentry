## Release 2.0.1 2025-10-23

Change `int32_t` (which is not required by the standard to exist) to
`int_least32_t` (which is required by the standard to exist). This update was
done through `sgit(1)` and that means the man page was indeed updated. Also a
typo fixes: `an int...` (instead of `a int...`).

Improve comments in `canon_path()` function source code about return values.

Fix minor typos in the `canon_path(3)` man page.


## Release 2.0.0 2025-10-16

Improve `.gitignore` and sort the contents.

Correct Copyright date range.

Add `Makefile` (based on libpr Makefile).

Add `-D` to `cpath(1)` and `dotdot_err` to `canon_path()` to return
`PATH_ERR_DOTDOT_OVER_TOPDIR` if **..** (dot-dot) moves before start
of path.  By default now, when this happens to an absolute path,
the **..** (dot-dot) are removed.  And when  this happens to a
relative path, the **..** (dot-dot) are left on the path.

Added `-S regex` and `preg` to supply a non-default safety
extended regular expression.

Added `cpath(1)` and `canon_path(3)` man pages, along with
the section 3 man page copies of the c`canon_path(3)` man page.

Corrected and expanded the C code example in the `canon_path(3)` man
page and the `README.md` file.

Added various bug fixes.

Added `test_cpath.sh`, `make test` and test files.
