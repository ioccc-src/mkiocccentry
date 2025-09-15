/*
 * c_compat - backward compatibility for __attribute__, __func__, and not_reached()
 *
 * "The symbol compatible appears to have become undefined in the minds of
 "  some of the members of the ANSI C committee."
 *
 * "The wise speak only of what they know."
 *
 *      -- The Two Towers
 *
 * Copyright (c) 2025 by Landon Curt Noll.  All Rights Reserved.
 *
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright, this permission notice and text
 * this comment, and the disclaimer below appear in all of the following:
 *
 *       supporting documentation
 *       source copies
 *       source works derived from this source
 *       binaries derived from this source or from derived source
 *
 * LANDON CURT NOLL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL LANDON CURT NOLL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * Share and enjoy! :-)
 */


#if !defined(INCLUDE_C_COMPAT_H)
#    define  INCLUDE_C_COMPAT_H


/*
 * backward compatibility for __attribute__ and __func__
 *
 * Both __attribute__ and __func__ are used by the dbg library, however ...
 *
 * Not all compilers support __attribute__.  For example, MSVC, TenDRA, the Little C Compiler
 * does not support __attribute__, and early gcc (pre gcc 2.8) do not support __attribute__.
 * In such cases we simply define __attribute__(A) as a nothing macro.
 *
 * The all compilers support __func__.  For example, __func__ is not supported prior to C99,
 * so we fake it with __FILE__, or in the case if not having __FILE__ we define __func__
 * as a "((__unknown_function_name__))" string constant.
 */
#if !defined(__attribute__) && \
    (defined(__cplusplus) || !defined(__GNUC__)  || __GNUC__ == 2 && __GNUC_MINOR__ < 8)
  #define __attribute__(A)
#endif
#if __STDC_VERSION__ < 199901L
  #if !defined(__func__)
    #if defined(__FILE__)
      #define __func__ __FILE__
    #else
      #define __func__ "((__unknown_function_name__))"
    #endif
  #endif
#endif


/*
 * backward compatibility for not_reached()
 *
 * The not_reached() is used in the dbg library, however ...
 *
 * In the old days of lint, one could give lint and friends a hint by
 * placing the token NOTREACHED immediately between opening and closing
 * comments.  Modern compilers do not honor such commented tokens
 * and instead rely on features such as __builtin_unreachable
 * and __attribute__((noreturn)).
 *
 * The not_reached will either yield a __builtin_unreachable() feature call,
 * or it will call abort from stdlib.
 *
 * NOTE: Not all C compilers have the __has_builtin special operator, so we have
 *       to use it with care.  If we do not support __has_builtin, then
 *       we need to have not_reached() just call abort(3).
 */
#if defined(__has_builtin)
  #if __has_builtin(__builtin_unreachable)
    #define not_reached() __builtin_unreachable()
  #endif
#endif
#if !defined(not_reached)
  #define not_reached() abort()
#endif


#endif                          /* INCLUDE_C_COMPAT_H */
