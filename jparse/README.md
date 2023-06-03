# jparse - JSON parser written in C


`jparse` is a JSON parser (a stand-alone tool and a library) written in C with
the help of `flex(1)` and `bison(1)`. It was co-developed in 2022 by:

*@xexyl* (**Cody Boone Ferguson**, [https://xexyl.net](https://xexyl.net),
[https://ioccc.xexyl.net](https://ioccc.xexyl.net))

and:

*chongo* (**Landon Curt Noll**, [http://www.isthe.com/chongo/index.html](http://www.isthe.com/chongo/index.htm)) /\oo/\


in support of the **IOCCC** ([International Obfuscated C Code
Contest](https://www.ioccc.org)), originally in the [mkiocccentry
repo](https://github.com/ioccc-src/mkiocccentry), but we provide it here so that
anyone may use it.

As a stand-alone tool it is less useful, perhaps save for validating json
documents and to see how it works. The library is much more useful because you
can integrate it into your own applications and work with the parsed tree. More
details on the library will be documented at a later date although we do give a
bit of information below.

We also have some additional tools, some of which will be documented later and
some of which are documented below, namely `jstrencode` and `jstrdecode`.


## Compiling

We determine if you have a recent enough `flex(1)` and `bison(1)`. If you do not
we use backup files. Either way, to compile you need only run:


```sh
make all
```


## Stand-alone tool

As a tool by itself `jparse` takes a block of memory from either a file (stdin
or a text file) or a string (via `-s` option) and parses it as JSON, reporting
if it is validly formed JSON or not.

### Synopsis


```sh
./jparse [-h] [-v level] [-J level] [-q] [-V] [-s] -- arg
```

The `-v` option increases the overall verbosity level whereas the `-J` option
increases the verbosity of the JSON parser. The `-q` option suppresses some of
the output.

If `-s` is passed the arg is expected to be a string; otherwise it is expected
to be a file.

The options `-V` and `-h` show the version of the parser and the help or usage
string, respectively.


### Exit status

If the JSON is valid the exit status of `jparse` is 0. Different non-zero values
are for different error conditions, or help or version string printed.


## The jparse library

As a library, `jparse` is much more useful as it allows one to parse JSON in
their application and then interact with the parsed tree.


### Linking the library into your own code

To use you need to include `jparse.h` and then link in the `jparse.a` static
library. This will be documented at a later date.


### Example use of the library

We currently do not have a short and simple example to go through. This will
come at a later date when I (Cody) make this into a separate GitHub repo.
Nevertheless one may check the library man page by:


```sh
man ./man/man3/jparse.3
```

which gives more information about the most important functions.

### Re-entrancy

Although the scanner and parser are both re-entrant, only one parse at one time
in a process has been tested. The testing of more than one parse at the same
time is way out of scope of this repo but will be tested more as I (@xexyl, Cody
Boone Ferguson) move this to a separate repo.

If it's not clear this means that having more than one parse active in the same
process at the same time is not tested so even though it should be okay there
might be some issues that have yet to be discovered.

## `jprint`: a JSON printer

This tool is currently being developed by:

*@xexyl* (**Cody Boone Ferguson**). See
[https://xexyl.net](https://xexyl.net) and
[https://ioccc.xexyl.net](https://ioccc.xexyl.net).

and:

**chongo** (**Landon Curt Noll**, <http://www.isthe.com/chongo/index.html>) /\oo/\

but much, much more needs to be done. As of 03 June 2023 the most it does is
check that the json file passed into the program exists, is a regular file and
is valid json, reporting errors or valid JSON, assuming that the file name is
specified.

NOTE: this tool is not to check for valid JSON as such but instead to print
information in valid JSON files. For now it does report it as valid or invalid
as all the code does for now is attempt to parse the JSON so that in future
commits (after discussions) it can either exit with an error code or else
process the `name_arg`s.

# History

For more detailed history that goes beyond this humble document we
recommend you check `jparse(1)` and the `chkentry(1)` man page in the
`mkiocccentry` repo as well as its `CHANGES.md` and `README.md` files. If you
want to go further than that you can read the GitHub git log as well as reading
the source code. If you do read the source code we **STRONGLY** recommend you
read the `jparse.l` and `jparse.y` files and **NOT** the bison or flex generated
code! This is because the generated code might give you nightmares and cause
other horrible symptoms. :-) See `sorry.tm.ca.h` for more details.

# Examples

Parse the JSON string `{ "test_mode" : false }`:

```sh
./jparse -s '{ "test_mode" : false }'
```

Parse input from stdin (send EOF, usually ctrl-d or ^D, to parse):

```sh
./jparse -
[]
^D
```

Parse just a negative number:

```sh
./jparse -s -- -5
```

Parse .info.json file:

```sh
./jparse .info.json
```

Run the `jparse_test.sh` script using the default `json_teststr.txt` file with verbosity set at 5:

```sh
./jparse_test.sh -v 5
```



# See also

For more information, try:

```sh
man ./man/man1/jparse.1
man ./man/man3/jparse.3
man ./man/man1/jstrencode.1
man ./man/man1/jstrdecode.1
```

NOTE: the library man page currently has no example and this will not happen
until later, possibly not until it's an actual repo.

### `jstrencode`

Encode data.  This tool can converts data into JSON encoded strings according to the so-called
[JSON data interchange syntax - ECMA-404](https://www.ecma-international.org/publications-and-standards/standards/ecma-404/).

This tool was co-developed in 2022 by:

*@xexyl* (**Cody Boone Ferguson**, [https://xexyl.net](https://xexyl.net),
[https://ioccc.xexyl.net](https://ioccc.xexyl.net))

and:

*chongo* (**Landon Curt Noll**, [http://www.isthe.com/chongo/index.html](http://www.isthe.com/chongo/index.htm)) /\oo/\

For more information and examples, try:

```sh
man ./man/man1/jstrencode.1
```

NOTE: After doing a `make all`, this tool may be found as: `./jstrencode`.


### `jstrdecode`

Decode JSON encoded strings.  This tool converts JSON encoded strings to their original data according to the so-called
[JSON data interchange syntax - ECMA-404](https://www.ecma-international.org/publications-and-standards/standards/ecma-404/).

This tool was co-developed in 2022 by:

*@xexyl* (**Cody Boone Ferguson**, [https://xexyl.net](https://xexyl.net),
[https://ioccc.xexyl.net](https://ioccc.xexyl.net))

and:

*chongo* (**Landon Curt Noll**, [http://www.isthe.com/chongo/index.html](http://www.isthe.com/chongo/index.htm)) /\oo/\

For more information and examples, try:

```sh
man ./man/man1/jstrdecode.1
```

NOTE: After doing a `make all`, this tool may be found as: `./jstrdecode`.

