# jparse - JSON parser written in C

`jparse` is a JSON parser written in C with the help of `flex(1)` and
`bison(1)`. It was co-developed by Cody Boone Ferguson and Landon Curt Noll (one
of the IOCCC Judges) in support of the *IOCCC* (_International Obfuscated C Code
Contest_), originally in the
[mkiocccentry repo](https://github.com/ioccc-src/mkiocccentry).

# Synopsis

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

# Exit status

If the JSON is valid the exit status of `jparse` is 0. Different non-zero values
are for different error conditions, or help string printed.


# History

For more detailed history that goes beyond this humble document we
recommend you check `jparse(1)` and the `chkentry(1)` man page in the
`mkiocccentry` repo as well as its `CHANGES.md` and `README.md` files. If you
want to go further than that you can read the GitHub git log as well as reading
the source code. If you do read the source code we **STRONGLY** recommend you
read the `jparse.l` and `jparse.y` files and **NOT** the bison or flex generated
code! This is because the generated code is likely to give you nightmares and
cause other horrible symptoms.

# Bugs

It's not yet fully re-entrant. This might or might not be done later on. See
below for the implications.

# Re-entrancy

Although the scanner and parser are re-entrant only one parse at one time in a
process has been tested. The testing of more than one parse at the same time is
way out of scope of this repo but will be tested more as I move this to a
separate repo.

If it's not clear this means that having more than one parse active in the same
process at the same time is not tested so even though it should be okay there
might be some issues that have yet to be discovered.

# Compiling and using in your own code

This won't be described until after the IOCCCMOCK contest and any changes are
made based on what is learnt. After this I (Cody) will make a separate repo for
the parser which will have an updated README and I will document how to use the
tool in your own projects. The below section Examples will be updated at this
point as well.

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
       
The following man pages might also be of value:

- jparse(1)
- mkiocccentry(1)
- jstrencode(1)
- jstrdecode(1)
- chkentry(1) (for the IOCCC reason the code exists in the first place)
- flex(1)
- bison(1)

