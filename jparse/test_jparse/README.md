# Preparing the bad location tests

If ever a file is added to the `test_JSON/bad_loc` directory then one **MUST**
run from `jparse/test_jparse/` the following commands exactly:

```sh
rm ./test_JSON/bad_loc/*.err
for i in ./test_JSON/bad_loc/*.json; do ../jparse -- "$i" 2>"$i.err" ; done
```

Then run from the top level directory `make test`. If all is okay then you must
do the following, also from the top level directory:

```sh
git add ./jparse/test_jparse/test_JSON/bad_loc/*.json
git add ./jparse/test_jparse/test_JSON/bad_loc/*.err
```

Then commit and make a pull request to have it added to the repo.

**WITHOUT THIS, `make test` WILL FAIL!**
