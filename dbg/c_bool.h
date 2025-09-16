/*
 * c_bool - ensure that bool, true, and false are understood by the C compiler
 *
 * "Some people suggest that those who, in the process of accumulating frequent
 *  flyer and hotel points, should work on making some language other than C,
 *  incompatible with the existing code base."
 *
 * "It is 'Wirthless' to assume that one can force people to program the way
 *  you think they should be programming simply by changing the language
 *  out from under other programmers, or in the vain attempt to creat(2) :-)
 *  yet another 'language of the month' and then attempt to market that
 *  new language as something that will solve the world's coding problems."
 *
 * "It is true that some people have been known to define 0 as true."
 *
 * "And he that breaks a thing to find out what it is has left the path of wisdom."
 *
 *      -- Gandalf to Saruman, The Fellowship of the Ring.
 *
 * "Sméagol'll get into real true hot water, when this water boils,
 *  if he don’t do as he asked ..."
 *
 *      -- Samwise Gamgee to Gollum in Ithilien, The Two Towers.
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


#if !defined(INCLUDE_C_BOOL_H)
#    define  INCLUDE_C_BOOL_H


/*
 * Be sure we have some form of standard truth :-)
 *
 * We need to be sure that bool, true, and false are understood by the C compiler.
 *
 * We will go to great lengths to be sure that C compilers, from C89 to C23 and beyond
 * understood bool, true, and false.  We even allow K&R C compilers who updated
 * their C preprocessor to have a chance to learn about some form of standard truth :-)
 *
 * NOTE: Some old K&R C compilers understood more modern C preprocessor directives
 *       such as defined(FOO), etc.  If your K&R C compiler cannot, sorry, your
 *       K&R C compiler will fail to compile this code.
 *
 * For amusement, we even make an attempt for those suffering in "Microsoft land".  :-)
 *
 * NOTE: For a good reference C reserved keywords, see:
 *
 *      https://en.cppreference.com/w/c/keyword.html
 */


/*
 * Microsoft Visual C++ Compiler?  We hope not!
 *
 * Microsoft Visual C++ Compiler workaround because, of course Microsoft, screwed stuff up yet again.
 *
 * We could just ignore Microsoft, nevertheless the following puts those who suffer from
 * using Microsoft on the same footing as the rest of the world.  So as an act of "kindness"
 * to those who are unfortunate enough to use Microsoft stuff, we do the following.
 *
 * We keep this C preprocessor method around as a testimony as to just how disgusting the
 * Microsoft development environment can be!
 *
 * See also:
 *
 *  https://github.com/Cordic77/C-programming/blob/master/detect_stdc.h
 *
 * IMPORTANT NOTE: This next section is NOT intended to be fix for various Microsoft development
 *                 environments, past, present or future.  This section is for your amusement and
 *                 is effectively unsupported as we have no interest in testing this section for
 *                 Microsoft development compatibility.  We include this because those who do NOT
 *                 use Microsoft development environments should have NO reason to define _MSC_VER
 *                 and as such, this next section won't bother their code.
 *
 * TL;DR If you are not using Microsoft stuff, congratulations!  You can ignore this next _MSC_VER section.
 */
#if defined(_MSC_VER)

  /*
   * Within this _MSC_VER code block, we have no __STDC_VERSION__ or __STDC_VERSION__ is C90 or earlier
   */
  #if !defined(__STDC_VERSION__) || __STDC_VERSION__ <= 199409L

    /*
     * Visual Studio 2019, Visual C++ v16.8 (or higher)
     */
    #if _MSC_VER >= 1928

        /*
         * Support for _Pragma, restrict, _Noreturn, _Alignas, _Alignof, _Generic, _Static_assert.  See also:
         *
         *  https://devblogs.microsoft.com/cppblog/c11-and-c17-standard-support-arriving-in-msvc/
         *
         * C17/18 is just a "bug fix release" and basically the same as C11 (in Microsoft's mind).
         */
        #if defined(STD__C_VERSION)
          #undef STD__C_VERSION                 /* paranoia */
        #endif
        #define STD__C_VERSION 201112L

        /*
         * FYI: Visual Studio 2019 version 16.8 still defaults to 'Default ("Legacy MSVC")', but finally
         *      adds compiler switches to select specific (C/C++) language standards: /std:c11, /std:c17
         *      and sets __STDC_VERSION__ accordingly (otherwise set to 199409L by default).
         */

    /*
     * Visual Studio 2015, Visual C++ v14 (or higher)
     */
    #elif _MSC_VER >= 1900

        /*
         * Most "C99" language features and good library support; e.g. adds "uchar.h" and snprintf
         * (among others).  See also:
         *
         *  See https://devblogs.microsoft.com/cppblog/c11-and-c17-standard-support-arriving-in-msvc/
         */
        #if defined(STD__C_VERSION)
          #undef STD__C_VERSION                 /* paranoia */
        #endif
        #define STD__C_VERSION 199901L

    /*
     * Visual Studio 97, Visual C++ 5.0 (or higher)
     */
    #elif _MSC_VER >= 1100

        /*
         * Visual Studio 2010 v10 (or higher)
         */
        #if _MSC_VER >= 1600

          /*
           * While <inttypes.h> was still missing in VS2010, it did ship with a <stdint.h> header
           */
          #if defined(HAVE_STDINT_H)
            #undef HAVE_STDINT_H                /* paranoia */
          #endif
          #define HAVE_STDINT_H 1

          /*
           * Visual Studio 2013, Visual C++ v12 (or higher)
           */
          #if _MSC_VER >= 1800

            /*
             * _Bool type, Compound literals, Designated initializers, Mixing declarations/code, __func__ macro.
             * See also:
             *
             *  https://blogs.msdn.microsoft.com/vcblog/2013/07/19/c99-library-support-in-visual-studio-2013/
             */
            #if defined(HAVE_INTTYPES_H)
              #undef HAVE_INTTYPES_H            /* paranoia */
            #endif
            #define HAVE_INTTYPES_H 1
            #if defined(HAVE_STDBOOL_H)
              #undef HAVE_STDBOOL_H             /* paranoia */
            #endif
            #define HAVE_STDBOOL_H 1
            #if !defined(STD__C_VERSION)
              #define STD__C_VERSION 199901L    /* paranoia */
            #endif
          #endif

        /*
         * Visual C++ 5.0 already shipped with a "VCPP-5.00\DEVSTUDIO\VC\CRT\SRC\ISO646.H"
         */
        #else
          #if !defined(STD__C_VERSION)
            #define STD__C_VERSION 199409L
          #endif
        #endif

    /*
     * MSC5.1 stuck closely to the ANSI standard; MSC6 is just about on the money
     */
    #elif _MSC_VER >= 600

        /*
         * See also:
         *
         *  http://www.drdobbs.com/windows/optimizing-with-microsoft-c-60/184408398
         */
        #if !defined(STD__C_VERSION)
          #define STD__C_VERSION 198900L        /* fake C89 C version */
        #endif

    /*
     * We have a very old _MSC_VER, so assume K&R C: "The C Programming Language", 1st Edition (1978)
     *
     * NOTE: We draw the line at K&R 1st Edition (1978) for those K&R C compilers with modernized C preprocessors.
     */
    #else

        #if !defined(STD__C_VERSION)
          #define STD__C_VERSION 197800L        /* fake K&R C version */
        #endif
    #endif

    /* assertion: Within this _MSC_VER code block, STD__C_VERSION was defined or was set best as we can */

    /*
     * Within this _MSC_VER code block, we have STD__C_VERSION, we are C90 or earlier
     *
     * If we have no __STDC_VERSION__, so set __STDC_VERSION__ to STD__C_VERSION because we must.
     * Yes, defining __STDC_VERSION__ is a "no-no", however it wasn't defined when it should have
     * been defined!  What else can we do?!?
     */
    #if !defined(__STDC_VERSION__)
        #define __STDC_VERSION__ STD__C_VERSION /* sorry (tm Canada) */
    #endif

    /* assertion: Within this _MSC_VER code block, __STDC_VERSION__ was defined or was set best as we can */

  /*
   * Within this _MSC_VER code block, we have __STDC_VERSION__ and it is post C90
   */
  #else

    /*
     * Within this _MSC_VER code block, we have __STDC_VERSION__, we are post C90
     *
     * If we have no STD__C_VERSION, set STD__C_VERSION to __STDC_VERSION__ and hope for the best.
     */
    #if !defined(STD__C_VERSION)
        #define STD__C_VERSION __STDC_VERSION__
    #endif
  #endif

  /* assertion: Within this _MSC_VER code block, __STDC_VERSION__ was defined or was set best as we can */
  /* assertion: Within this _MSC_VER code block, STD__C_VERSION was defined or was set best as we can */

#endif    /* end of "Microsoft land" and the end of the _MSC_VER section */


/*
 * ISO/IEC 9899 "C" standard conformance - set C_STD based on __STDC_VERSION__
 *
 * Sigh: As if dealing with the rot produced by the ANSI C committee "isn't bad enough"!
 *
 * Some pedantic C compiler environments may set __STDC_VERSION__ but not STD__C_VERSION for some reason.
 */
#if defined(__STDC_VERSION__)

  /*
   * in case we have __STDC_VERSION__ but not STD__C_VERSION, set STD__C_VERSION to __STDC_VERSION__
   */
  #if !defined(STD__C_VERSION)
    #define STD__C_VERSION __STDC_VERSION__
  #endif

  /*
   * C_STD paranoia
   */
  #if !defined(C_STD)

    /*
     * C23 - ISO/IEC 9899:2024
     */
    #if STD__C_VERSION >= 202311L
        #define C_STD 2023

    /*
     * C17/C18 - ISO/IEC 9899:2018
     */
    #elif STD__C_VERSION >= 201710L
        #define C_STD 2017

    /*
     * C11 - ISO/IEC 9899:2011
     */
    #elif STD__C_VERSION >= 201112L
        #define C_STD 2011

    /*
     * C99 - ISO/IEC 9899:1999
     */
    #elif STD__C_VERSION >= 199901L
        #define C_STD 1999

    /*
     * C94/C95 - ISO/IEC 9899/AMD1:1995, Normative Addendum 1 (NA1)
     */
    #elif STD__C_VERSION >= 199409L
        #define C_STD 1995

    /*
     * C89 paranoia
     *
     * If we got here, then perhaps STD__C_VERSION is at the C89 level.
     */
    #elif STD__C_VERSION >= 198900L
        #define C_STD 1989

    /*
     * K&R paranoia
     *
     * If all else fails, assume K&R 1st Edition, even if __STDC_VERSION__ is 1.
     */
    #else
        #define C_STD 1978
    #endif

  #endif

  /* assertion: At this point in the code, STD__C_VERSION was defined or was set best as we can */
  /* assertion: At this point in the code, C_STD was defined or was set best as we can */

/*
 * If we have no __STDC_VERSION__, we assume pre-C95
 *
 * FYI: __STDC_VERSION__ was added in "C95", Normative Addendum 1 (NA1)
 *
 * Some pedantic C compiler environments, and old C compilers may not set __STDC_VERSION__.
 */
#else

  /*
   * paranoia
   */
  #if defined(C_STD)
    #undef C_STD                                /* paranoia */
  #endif

  /*
   * If have no __STDC_VERSION__ but we have __GNUC__, assume C89
   *
   * NOTE: __GNUC__ is also set by other (GCC compatible) compilers
   */
  #if defined(__GNUC__)

    #define C_STD 1989
    #if !defined(STD__C_VERSION)
        #define STD__C_VERSION 198900L          /* fake C89 C version */
    #endif

  /*
   * If we have no __STDC_VERSION__ and no __GNUC__, so we assume K&R
   */
  #else

    #define C_STD 1978
    #if !defined(STD__C_VERSION)
        #define STD__C_VERSION 197800L          /* fake K&R C 1st Edition version */
    #endif

  #endif

  /* assertion: At this point in the code, C_STD was defined or was set best as we can */
  /* assertion: At this point in the code, STD__C_VERSION was defined or was set best as we can */

  /*
   * time to set __STDC_VERSION__ according to STD__C_VERSION
   *
   * If we have no __STDC_VERSION__, so set __STDC_VERSION__ to STD__C_VERSION because we must.
   * Yes, defining __STDC_VERSION__ is a "no-no", however it wasn't defined when it should have
   * been defined!  What else can we do?!?
   */
  #define __STDC_VERSION__ STD__C_VERSION       /* sorry (tm Canada) */

#endif


/* assertion: At this point in the code, __STDC_VERSION__ was defined or was set best as we can */
/* assertion: At this point in the code, STD__C_VERSION was defined or was set best as we can */
/* assertion: At this point in the code, C_STD was defined or was set best as we can */


/*
 * Now that we have __STDC_VERSION__, STD__C_VERSION, and C_STD we can now
 * go about assuring we know some form of standard truth.  That is, we will
 * be sure that bool, true, and false are understood by the C compiler.
 */

#if __STDC_VERSION__ >= 202311L

  /*
   * Starting with C23:
   *
   *        bool is a fundamental type
   *        true is a predefined boolean constant
   *        false is a predefined boolean constant
   *
   * Nothing else needs to be done to assure the C compiler understands bool, true, and false.
   */

/*
 * From C99 up thru C17 and NOT C23, use <stdbool.h>
 */
#elif (__STDC_VERSION__ >= 199901L) && (__STDC_VERSION__ < 202311L)

   /*
    * For C99 up thru C17 but NOT including C23 has <stdbool.h>
    *
    * With <stdbool.h>:
    *
    *   bool expands to _Bool
    *        BTW: _Bool is a keyword that has been officially part of the language starting with C99.
    *             _Bool is a data type large enough to store the values 0 and 1.
    *   true which expands to 1
    *   false which expands to 0
    */
   #include <stdbool.h>

/*
 * C89 up thru C90 but NOT including C99
 *
 *      We have to hack together our bool, true, and false via the enum method.
 */
#elif (__STDC_VERSION__ >= 198900L) && (__STDC_VERSION__ < 199901L)

  /*
   * paranoia - in case something else defined bool, true, or false
   */
  #if defined(bool)
    #undef bool                                 /* paranoia */
  #endif
  #if defined(true)
    #undef true                                 /* paranoia */
  #endif
  #if defined(false)
    #undef false                                /* paranoia */
  #endif

  /*
   * use a enum to define bool, true, and false
   */
  enum {
    false = 0,
    true = 1
  } bool;

/*
 * Before C89 but at least K&R 1st Edition, or __STDC_VERSION__ is screwed up!
 *
 * BTW: If we are dealing with before K&R 1st Edition, such a compiler
 *      will likely be unable to compile with these C preprocessor directives,
 *      unless the K&R C compiler has a modernized C preprocessor.
 *
 * FYI: Some /bin/cpp in K&R C compiler environments could get rather wacky.
 *      For example a certain someone turned "Algol-like" stuff into C, that
 *      was then used as a shell.  There is a limit as to how far we will go
 *      to protect such environments, nevertheless there were some that
 *      we able to convert surprisingly modern C into K&R C.
 *
 *      If you have one those environments, then we presume you are used
 *      to having to tweak stuff to get it to compile anyway.  We just hope
 *      you appreciate our attempt to make your tweak job a bit more easy.
 *
 * BTW: This file is not intended to protect, nor withstand IOCCC.  :-)
 *
 * We cannot assume we have an enum in this extreme C compiler case.
 */
#else

  /*
   * paranoia - in case something else defined bool, true, or false
   */
  #if defined(bool)
    #undef bool                                 /* paranoia */
  #endif
  #if defined(true)
    #undef true                                 /* paranoia */
  #endif
  #if defined(false)
    #undef false                                /* paranoia */
  #endif

  /*
   * make bool an int, and then define true, and false
   */
  #define bool int
  #define true ((int)1)
  #define false ((int)0)

#endif

/* assertion: At this point in the code, the C compiler will understand bool */
/* assertion: At this point in the code, the C compiler will understand true */
/* assertion: At this point in the code, the C compiler will understand false */


#endif                          /* INCLUDE_C_BOOL_H */
