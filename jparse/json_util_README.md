# Command line utilities that read and process JSON in different ways


## Introduction to this document

NOTE: Please see [json_README.md](./json_README.md) for some important **JSON
terms** used in this document.

There is a general lack of JSON aware command line tools that allow someone to
obtain information from within a JSON document.  By "obtaining information" we
refer to the ability to extract (and if desired, format) data, either as a whole
or in part, that is contained within a [JSON
document](./json_README.md#json-document).

There does exist multiple [JSON](./json_README.md) [APIs (Application Program
Interfaces)](https://en.wikipedia.org/wiki/API) for various programming
languages such as: JavaScript, C, Java, Python, Perl, Go, Rust, PHP, etc.  This
[mkiocccentry repo](https://https://github.com/ioccc-src/mkiocccentry) contains
a rich JSON parser API C programs can use, as well as a semantic JSON check
interface for C.  However all those APIs require the user to "program" in a
specific language in order to do something as simple as obtain a selective [JSON
values](./json_README.md#json-value) from a [JSON
document](./json_README.md#json-document). With these tools we attempt to
address some of these problems.



## Scope of this document

In this document, we will concern ourselves only with [Valid
JSON](./json_README.md#valid-json) in the form of a [JSON
document](./json_README.md#json-document), where a document can be a file or
stdin. It must be [valid JSON](./json_README.md#valid-json) only because we can
only extract information with any degree of certainty from [Valid
JSON](./json_README.md#valid-json).

We consider command line utilities that _read_ information from a [JSON
document](./json_README.md#json-document) in the form of either a file or stdin.
In this document _we are not_ considering utilities that either create or modify
JSON.

If a command line utility is given an **invalid JSON document**, the utility
shall exit non-zero with an error message indicating that the [JSON
document](./json_README.md#json-document) is not valid JSON.  Therefore we're
only concerned with reading various information from an existing [JSON
document](./json_README.md#json-document) or stdin that contains [Valid
JSON](./json_README.md#valid-json).


## Common command line options

In this document, we assume that all utilities have at least the following
options:

```
	-h		Print help and exit
	-V		Print version and exit
	-v level	Verbosity level (def: 0)
	-J level	JSON verbosity level (def: 0)
	-q		Quiet mode, do not print to stdout (def: print stuff to stdout)
	-L <num>[{t|s}]	Print JSON level followed by a number of tabs or spaces (def: don't print levels)
	-L tab		Alias for: '-L 1t'

			Trailing 't' implies <num> tabs whereas trailing 's' implies <num> spaces.
			Not specifying 's' or 't' implies spaces.

	-l lvl		Print specific JSON levels (def: print at any level)

			If lvl is a number (e.g.: -l 3), level must == number.
			If lvl is a number followed by : (e.g. '-l 3:'), level must be >= number.
			If lvl is a : followed by a number (e.g. '-l :3'), level must be <= number.
			If lvl is num:num (e.g. '-l 3:5'), level must be inclusively in the range.

	-m		Maximum depth to traverse in the JSON tree
	-K		Run test mode and exit
	-o ofile	Output formatted JSON to ofile (def: standard output, - ==> standard output)
	-F fmt		Change the JSON format style (def: use default)

			tty		When output is to a TTY, use colour, otherwise use simple
			simple		Each line has one JSON level determined by '[]'s and '{}'s
			colour		Simple plus ANSI colour syntax highlighting
			color		Alias for colour (colour excluding you :-) )
			1line		One line output
			nows		One line output, no extra whitespace

```

Additional command line options will be added on a utility by utility basis.



## Common exit codes

In this document, we assume that all utilities have at least the following exit
codes:

```
Exit codes:
    0	all is OK and file is valid JSON
    1	error writing to output file (if -o specified)
    2	-h and help string printed or -V and version string printed
    3	invalid command line, invalid option or option missing an argument
    4	file does not exist, not a file, or unable to read the file
    5	file contents is not valid JSON
    6	test mode failed
...
 >=10	internal error
```

Additional exit codes will be added on a utility by utility basis.
