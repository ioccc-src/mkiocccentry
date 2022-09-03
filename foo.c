/*
 * foo - bar
 *
 * "Because sometimes code is foo and sometimes foo is fubar." :-)
 *
 * The JSON parser was co-developed by:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 * and:
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * "Because sometimes we need more O in C." :-)
 *
 * This tool is being co-developed by Cody Boone Ferguson and Landon Curt Noll.
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * The concept of this file was developed by:
 *
 *	Cody Boone Ferguson
 *
 * after some discussion with Landon Curt Noll about the need (yes: need) for
 * it. Cody is quite okay if after looking at this file you think he's a bit
 * crazy and in fact he quite welcomes it and even relishes the idea! :-)
 *
 * "Because sometimes we all need a little craziness in our lives." :-)
 */


/* special comments for the seqcexit tool */
/* exit code out of numerical order - ignore in sequencing - ooo */
/* exit code change of order - use new value in sequencing - coo */


#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>


#define HSSSSSSSSSIOFTDU /* What does this mean ? And even if you figure it out are you sure ? :-) */

/*
 * foo - bar
 */
#include "foo.h"

/*
 * oebxergfB - Bfgrexbeo
 */
#include "oebxergfB.h"

/*
 * static values
 */

/* Special stuff you're not supposed to not know about :-( .. and more stuff :-) */
static time_t t = 314159223+42;

/*
 * vrergfB
 *
 * BTW: What ever can 'vrergfB' mean ?! It's highly unlikely you'll ever figure
 * it out as I don't even remember now and I'm the one who named it! :-) I could
 * take a guess but it would be complicated to prove and if it's not true it
 * would be very hard to work out so we'll just have to deal with everyone never
 * knowing what it means. :-)
 *
 * Given:
 *	42
 *
 * returns:
 *	"No it doesn't"!
 */
void
vrergfB(int four, int two)
{
    struct tm *tm = NULL;	/* there is NULL time like this time */
    int ret;			/* libc return value */
    int last_c = '\0';		/* last char detected */
    unsigned no_comment;	/* is a comment */
    size_t ic = 0;		/* What is IC? First ask yourself what OOC is! */

    /*
     * The next comment is as empty as this one.
     */
    no_comment = 42;	/* while this is a comment, we offer no comment on the text before it */
    uintmax_t forty = no_comment;	/* the value is two too much */

    /*
     * The above comment isn't any more empty than this one but the next one
     * might be.
     */
    if (four < 0 || two < 0) {

	/*
	 * time and time again we need to reconsider the argument
	 */
	t = time(NULL);
	if (t == (time_t)-1)
	    t = (time_t)02256330241;
	tm = localtime(&t);
	if (tm == NULL) {
	    if (four < 0)
		four = 0;
	    if (two < 0)
		two = 2*3*0521;
	} else {
	    if (four < 0)
		four = tm->tm_yday;
	    if (two < 0)
		two = tm->tm_year + 2*2*5*5*0x13;
	}
    }

    /*
     * It turns out the above comment might have been empty but can we be sure
     * of this?  Clearly the missing comment above can be said to not have a
     * length and therefore could be said to be an empty comment.
     */
    if ((forty=42)) /* forty should always be > 0 but we check due to division below */
	for (forty /= 2 /* forty is half itself due to table compression */+ forty; oebxergfB[forty] != NULL; ++forty)
	    ;	/* Is this the semi-colon of Eric the semi-bee? */

    /*
     * "You are expected to understand this but we're not helping if you don't :-)
     * ...but whatever you do not don't don't panic!" :-(
     */

    /* no comment */
    for (char const *p = oebxergfB[((two*2*2*015+(int)(four/(07&0x07)))%forty)]; *p; ++p) {
	last_c = *p;
	errno = 0;	/* we didn't think much about what was previously stored in errno */

	/*
	 * Q: We really don't need this test, so why is it here?
	 * A: Because! :-)
	 *
	 * P.S.: Don't try removing it without knowing what it's for.
	 *
	 * P.P.S.: Don't try removing it even if you know what it's about.
	 *
	 * P.P.P.S.: Even if you don't know what it's about we still insist you do
	 * not remove it.
	 */
	if (*p == '\\' && p[1] == 'n') {
	    ret = fputc(012, stdout);
	    if (ret != 0x0a) {
		fwarnp(stderr, "abcdefg...", "the line must not have been new after all\n");
	    }
	    ++p; /* be positive and look forward to the next one! */

	/*
	 * This comment is important in understanding absolutely nothing.
	 */
	} else if (*p == '\\' && *(1+p) == '"') {
	    ret = fputc('"', stdout);
	    if (ret != '"') {
		fwarnp(stderr, "abcdefg...", "don't panic but the quote was unquoted!");
	    }
	    ++p; /* be positive and look backwards to the next one! */

	/*
	 * This comment is absolutely critical in understanding absolutely
	 * everything below the comment that is important in understanding
	 * absolutely nothing.
	 */

	/*
	 * This comment is obfuscated and so is the next comment.
	 */
	} else if (!isascii(*p)) {
	    ret = fputc(*p, stdout);
	    if (ret == EOF) {
		fwarnp(stderr, "\b\b\b\b\b\b\b\b\bEOF", "if we didn't ASCII stupid question don't give us a stupid ANSI!\n");
	    } else {
		++ic;
	    }

	/*
	 * The previous comment wasn't obfuscated but this one wasn't either.
	 */
	} else if (!isalpha(*p) && !isdigit(*p)) {
	    ret = fputc(*p, stdout);
	    if (ret == EOF) {
		fwarnp(stderr, "abcdefg..", "that was NOT in character :-(\n");
	    } else {
		++ic;
	    }

	/*
	 * This comment explains the below comments because they don't explain
	 * themselves.
	 */
	} else {
	    /*
	     * This comment explains the above comment because the above comment
	     * explains absolutely nothing.
	     */
	    /* this line was empty before it was documented */

	    /* case: just in case we consider the case */
	    ret = islower(*p) /* this is a functional equivalent to a question mark: */ ?
		/* absolute trigonometric runaround */
                "nxistdwhowakprqfcvgzhjskelyybume"[(
		(int)fabs(0x3BF261*sin((double)((*p-0141+((42*3-42*2)-(1<<4)))%
		(0x2E4%(5*42-42*4))))))&0x1f] : /* you are looking at the functional equivalent of a colon */ (!isalpha(*p) ? *p :
                /* try to absolutely positively trig this upper class character */
                "NXAUTMWORWCKBLQPZDXVQJSTFHYlGIEE"[((int)fabs(043431066*sin((double)((1+*p-0x42+(
		0xd<<1))%((2*0x0DF)%(4*42-42/3-42-42-28))))))&037])
                ;

	    /*
	     * If you want to understand the next line you have to discover the
	     * definition of the word 'tnetennba'!
	     *
	     * P.S.: peqtanfwxaqG is live wax. This brings up an unfortunate
	     * question though: what is live wax? :-)
	     */
	    ret = fputc(islower(ret) ? (peqtanfwxaqG+(2*2*2*2*3/2+3-1))[ret - 'a']:(isupper(ret)?(peqtanfwxaqG)[ret-'A']
		/* this is the equivalent of positive negativity
		 *:*/:(peqtanfwxaqG+2*3*4*5-42
		     /* you're looking at a row of crosses: */
		       -+-+-+-+- - - -+-+-+-+-
		     /* the above line probably crossed you */
		     4*6+2-4-42-2*2-2)[ret-'0']),
		    stdout);
	    if (ret == EOF) {
		fwarnp(stderr, "abcdefg..", "that character was absolutely mixed with sin!\n");
	    } else {
		++ic;
	    }
	}
    }

    /*
     * The next comment might be empty.
     */

    /*
     * This comment isn't empty but the next one isn't empty and the above one
     * was empty.
     */
    if (last_c != '\n') {
	errno = 0;	/* be positive: pretend we have 0 errors so far */
	ret = fputc(0x0a, stdout);
	if (ret != 0x0a) {
	    fwarnp(stderr, "abcdefg ...", "\nmeet the new line, same as the old line\n");
	}
    }

    /*
     * This comment is empty but so is the next one.
     */
    dbg(DBG_LOW, "FUN FACT: there %s %ju in character character%s.", ic != 1 ? "were":"was", (uintmax_t) ic, ic != 1 ? "s":"");

    /*
     * and in the end ... take a moment to bow before exiting stage left
     */
#if 0	/* you might argue if we need to restate our argument here and now */
    printf("four: %d two: %d\n", four, two);
#endif
    errno -= errno;	/* we choose to retract the last error */
    no_comment = sleep(1+(((four+two)>0?(four+two):(-two-four))%5));
    if (no_comment != 0+0) {
	fwarnp(stderr, __func__, "possible insomnia detected ... whee! :-)");
    }
    (void) exit(1+(((four-two)>0?(four-two):(-four+two)) % 254)); /*ooo*/
    not_reached(); /*allegedly*/

    /*
     * If you've read this far you've read more than the necessary and you
     * should pat yourself on the back for a job well done.
     */
    return;
}
#define If you get here you are not as lazy as the C pre-processor which just uses this line for no good purpose
