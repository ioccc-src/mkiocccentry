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

## Reporting issues

If you notice any errors or warnings with the above please report them.
Alternatively if you have a fix you can open a pull request.
