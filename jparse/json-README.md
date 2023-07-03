# JSON Terms

_Disclaimer_: While a number of **JSON terms** appear to describe JSON, they do
not constitute a "JSON specification".  For that see [so-called JSON
spec](#so-called-json-spec).

We define these **JSON terms** to assist you in understanding the JSON related
code, code comments, data structures and documentation found in this GitHub repo
particularly under the [jparse
directory](https://github.com/ioccc-src/mkiocccentry/tree/master/jparse).


## JSON document

A block of data that is in JSON format.  Such a document may be a JSON file, a
block of memory containing JSON, or some sort of JSON related programming object
such as a JSON parse tree.  The **JSON document** may be static content, or it
may be something that is delivered via some protocol such as via a network pipe.

A **JSON document** is restricted to JSON content only.  The headers of protocol
that delivers a JSON content are **NOT** part of the [JSON
document](#json-document).  When transporting JSON, only the part of the payload
containing JSON is considered a [JSON document](#json-document).  Similarly, a
file may contain one or more [JSON documents](#json-document), embedded amongst
other non-JSON data.  This file contains multiple [JSON
documents](#json-document), for example.

In terms of the [so-called JSON spec](#so-called-json-spec), a **JSON document**
is a [JSON element](#json-element).


## Valid JSON

A [JSON document](#json-document) is said to be **valid JSON** if the JSON
follows the [so-called JSON spec](#so-called-json-spec). A [JSON
document](#json-document) that cannot be parsed is said to be _invalid JSON_.

A [JSON document](#json-document), to be useful, needs to be [Valid
JSON](#valid-json).  This is because to be able to obtain information from the
[JSON document](#json-document), the JSON within that document needs to be
parsed.


## JSON element

A **JSON element** is a [JSON document](#json-document).  We prefer to use the
term "document" instead of "element" because we find that term to be more
descriptive.  Nevertheless, a **JSON element** and a [JSON
document](#json-document) are the same thing.

The **JSON element** is a single [JSON value](#json-value) surrounded by [JSON
whitespace](#json-whitespace).  Because [JSON whitespace](#json-whitespace) can
be nothing (zero bytes long), one might think of that surrounding [JSON
whitespace](#json-whitespace) being an "optional" part of a [JSON
document](#json-document).


## JSON value

A **JSON value** is a [JSON element](#json-element) with any surrounding [JSON
whitespace](#json-whitespace) removed.


## JSON object

A **JSON object** is represented in JSON by zero or more [JSON
members](#json-member) enclosed inside curly braces (`{` and `}`).  For example:

```json
{ }
{ "member_name" : "member_value" }
{ "member_name" : "member_value", "name2" : "value2" }
{ "member_name" : "member_value", "name2" : "value2", "name3" : 42 }
```

While there is [JSON whitespace](#json-whitespace) between each of the JSON
tokens in the **JSON object**, such can be considered optional:

```json
{}
{"member_name":"member_value"}
{"member_name":"member_value","name2":"value2"}
{"member_name":"member_value","name2":"value2","name3":42}
```

because [JSON whitespace](#json-whitespace) can be empty (zero bytes long).

A **JSON object** is zero or more unordered [JSON members](#json-member), while
a [JSON array](#json-array) is zero or more unordered [JSON
values](#json-value).


## JSON member

A **JSON member** consists of a [JSON name](#json-name) and a [JSON
value](#json-value).  A **JSON member** is a "name-value" pair.

In JSON syntax, the [JSON name](#json-name) and [JSON value](#json-value)
are separated by a `:` (colon), with [JSON whitespace](#json-whitespace)
(which can be empty (zero bytes long)) in between:

```json
"member_name" : "member_value"
```

While there is [JSON whitespace](#json-whitespace) between each of the JSON
tokens in the **JSON member**, such can be considered optional:

```json
"member_name":"member_value"
```

because [JSON whitespace](#json-whitespace) can be empty (zero bytes long).


## JSON array

A **JSON array** is represented in JSON by zero or more [JSON
values](#json-value) enclosed inside square braces (`[` and `]`).  For example:

```json
[ ]
[ { "member_name" : "member_value" , "name2" : 23209 }, [ 1, 2, 3 ], "string", 42, true, false, null ]
```

While there is [JSON whitespace](#json-whitespace) between each of the JSON
tokens in the **JSON array**, such can be considered optional:

```json
[]
[{"member_name":"member_value","name2":23209},[1,2,3],"string",42,true,false,null]
```

because [JSON whitespace](#json-whitespace) can be empty (zero bytes long).

A **JSON array** is zero or more unordered [JSON values](#json-value),
while a [JSON object](#json-object) is zero or more unordered [JSON
members](#json-member).


## JSON name

A **JSON name** is a [JSON string](#json-string) that is lead part
of a [JSON member](#json-member)'s "name-value" pair.


## JSON string

A **JSON string** is zero or more [JSON encoded](#json-encoded) characters.  A
**JSON string** is represented in JSON as a [JSON encoded](#json-encoded)
characters enclosed in double quotes (`"`).

To be [valid JSON](#valid-json), all **JSON string**s must be [JSON encoded
strings](#json-encoded-string) in a [JSON document](#json-document).

Note that a **JSON string** may be empty (zero bytes long). As represented in
JSON, such an empty (zero bytes long) **JSON string** would be:

```json
""
```

Unless otherwise specified, a **JSON string** is a [JSON encoded
string](#json-encoded-string).


## JSON encoded string

A **JSON encoded string** starts and ends with double quotes (`"`).

Inside the double quotes (`"`), the following characters _must_ be
encoded by preceding them with a reverse solidus (backslash - ASCII
`0x5c` - `U+005C`) character:

* `"` (double quote - ASCII `0x22` - `U+0022`)
* `\` (reverse solidus or backslash - ASCII `0x5c` - `U+005C`)
* backspace (control-H - `'\b'` - ASCII `0x08` - `U+0008`)
* formfeed (vertical tab - control-L - `'\v'` - ASCII `0x0c` - `U+000C`)
* newline (control-J - `'\n'` - ASCII `0x0a` - `U+000A`)
* carriage return (control-M - `'\r'` - ASCII `0x0d` - `U+000D`)
* horizontal tab (control-I - `'\t'` - ASCII `0x09` - `U+0009`)

In the above list, a single character is converted into 2 characters
by preceding them with a reverse solidus (backslash - ASCII
`0x5c` - `U+005C`) character.

For example, a double quote (ASCII `0x22` - `U+0022`) followed by a carriage
return (control-M - `'\r'` - ASCII `0x0d` - `U+000D`) must be encoded as the
following 4 characters:

```
\"\n
```

In addition the control characters `U+0000` to `U+001F` (`0x00` - `0x1f`) that
are _NOT_ listed above _must_ be encoded as a hexadecimal sequence as determined
by [ISO/IEC 10646](https://www.iso.org/standard/76835.html).  In particular,
they must be converted into a 6 character sequence that starts with `"\u"` and
ends with a 4 hexadecimal digits `0` - `9` and `a` - `f` (case-insensitive).

For example, the escape character (`'\e'` - ASCII `0x1b` - `U+001B`)
_must_ be encoded as follows:

```
\u000e
```

When one removes the special 2 character backslash encoded characters, the
following character ranges are converted into the 6 character sequence that
starts with `"\uxxxx"` where _xxxx_ are 4 hexadecimal digits.

```
0x00 - 0x07
0x0a - 0x0b
0x0e - 0x1f
```

Therefore, the following character values _must_ be encoded, either
by an above mentioned special 2 character backslash encode sequence
or a 6 character sequence that starts with `"\uxxxx"` where _xxxx_
are 4 hexadecimal digits:


```
0x00 - 0x1f
0x2f
0x5c
```

_NOTE_: All other characters outside of the above mentioned range
are _not_ required to be encoded.

The Basic Multilingual Plane character set (`U+0000` - `U+FFFF`)
_may_ be converted into the 6 character sequence that starts with
`"\uxxxx"` where _xxxx_ are 4 hexadecimal digits.  A keyword in the
preceding sentence is _may_.  Outside of the above mentioned character
range of character values that _must_ be encoded (transformed into
2 or 6 character sequences), any other character is allowed but
_not_ required to be encoded as a character sequence that starts
with `"\uxxxx"` where _xxxx_ are 4 hexadecimal digits.

While one is not required to backslash an encoded slash character (ASCII
`0x2f` - `U+002F`), one is allowed to do so.  The solidus or slash
character (ASCII `0x2f` - `U+002F`) is allowed to be, but is _not required_,
to be backslash encoded:

* `/` (solidus or slash - ASCII `0x2f` - `U+002F`)

The following **JSON encoded string** are _both valid_:

```json
"http://www.isthe.com/chongo/index.html"
```

as well as:

```json
"http:\/\/www.isthe.com\/chongo\/index.html"
```

While both UPPER and lower case hexadecimal letters are allowed by
the [so-called JSON spec](#so-called-json-spec) and are considered
as valid, the _jparse JSON parser_ in this GitHub repo produces
only lower case hexadecimal letters.

According to the [so-called JSON spec](#so-called-json-spec), the
following four **JSON encoded string**s are all _valid_ and
produce the same result:

```
"\u002F"
"\u002f"
"\/"
"/"
```

The following is [Valid JSON](#valid-json):

```json
{
    "`¡™£¢∞§¶•ªº–≠œ∑´®†¥¨ˆøπ“‘«åß∂ƒ˙∆˚¬…æΩ≈ç√∫˜µ≤≥÷`⁄€‹›ﬁﬂ‡°·‚—±Œ„´‰ˇÁ¨ˆØ∏”’»ÅÍÎÏ˝ÓÔÒÚÆ¸˛Ç◊ı˜Â¯˘¿/" : "https://www.ioccc.org"
}
```

The presence of non-ASCII characters and the presence of slash
characters (ASCII `0x2f` - `U+002F`) does _not_ impose a requirement
for those characters to be encoded.

_NOTE_: The [so-called JSON spec](#so-called-json-spec) permits one
to encode a character for which Unicode does not currently provide
character assignments.  So just because one may encode a value into
a 6 character sequence does _not_ mean that the original value was an
assigned Unicode character.

Encoding UTF-16 characters are represented by a pair of consecutive
character sequences.  For example the G clef character (`U+1D11E`)
may be represented as the following 12 characters:

```json
\ud834\udd1e
```


## JSON decoded string

A **JSON decoded string** is a [JSON encoded string](#json-encoded-string)
where all encoded characters are replaced by their non-encoded counterparts.

The following [valid JSON](#valid-json) is a [JSON encoded
string](#json-encoded-string):

```
"Hello, with \\-escaped and \u00c3\u009f-like chars, world!\n"
```

is decoded into the following string that includes a trailing newline
(control-J - `'\n'` - ASCII `0x0a` - `U+000A`):

```
Hello, with \-escaped and ß-like chars, world!
```

_NOTE_: A **JSON decoded string** is _not_ allowed in a [JSON
document](#json-document).  To be [valid JSON](#valid-json), all [JSON
strings](#json-string) _must_ be [JSON encoded strings](#json-encoded-string).


## So-called JSON spec

What we term the **so-called JSON spec** is the contents of the
[JSON.org](https://www.json.org/json-en.html) as well as the [ECMA-404
JSON Data Interchange
Standard](https://www.ecma-international.org/publications-and-standards/standards/ecma-404/).
The former is a [JSON.org](https://www.json.org/json-en.html) website that [changes from time to
time](https://web.archive.org/web/changes/https://www.json.org/json-en.html)
and the latter is a _ECMA-404_ standard.

_NOTE_: We refer to the combination both
[JSON.org](https://www.json.org/json-en.html) and [the ECMA-404
JSON Data Interchange
Standard](https://www.ecma-international.org/publications-and-standards/standards/ecma-404/)
as the **so-called JSON spec**, as well as the various interpretations
and misunderstandings of them as the **so-called JSON spec**. See
the [substandard JSON spec editorial](#substandard-json-spec-editorial)
for more information.


## JSON whitespace

A **JSON whitespace** is either _nothing_, or it is one or more of an
_ASCII space_ (`0x20`), _ASCII linefeed_ (`0x0a`), _ASCII carriage
return_ (`0x0d`), or _ASCII horizontal tab_ (`0x09`).

It is useful to remember that **JSON whitespace** can be nothing
(zero bytes long).  So when we say that a [JSON element](#json-element)
is single a [JSON value](#json-value) surrounded by [JSON
whitespace](#json-whitespace), that surrounding [JSON
whitespace](#json-whitespace) may be nothing (zero bytes long).


## Appendix A

```
"Share and Enjoy!"
    --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)
```


### Substandard JSON spec editorial

What we term the **so-called JSON spec** is the contents of the
[JSON.org](https://www.json.org/json-en.html).  We explicitly use
the term __so-called__ and __spec__ deliberately.  To call what
that [JSON.org](https://www.json.org/json-en.html) website shows
a specification would impugn many other specifications, many of
which are much more well written then the **so-called JSON spec**.

We speculate that the desire of the creators of the **so-called JSON
spec** was to _create a spec so small as to fit in the back of a business
card_ and it is in the opinion of the authors of this document that it produced a
rather substandard specification.  And despite the desire of the
creators of the **so-called JSON spec** to create a minimal
specification, the  **so-called JSON spec** contains aspects that
are **NOT** minimal or desirable. An example of this is the deliberate removal
of comments being supported. This is where an Easter egg in the json files of
the IOCCC comes from!

Also, despite what the creators of the **so-called JSON spec** claim,
their web site does [change from time to
time](https://web.archive.org/web/changes/https://www.json.org/json-en.html),
albeit sometimes for minor reasons such as a cosmetic change or
to link to a new language translation.  It may be best to use the
[ECMA-404 JSON Data Interchange
Standard](https://www.ecma-international.org/publications-and-standards/standards/ecma-404/)
instead of the somewhat more convenient
[JSON.org](https://www.json.org/json-en.html) web page for more
precise JSON details.  However, there is only so much that _ECMA-404_
standard can do with what they have to work with.

A number of _JSON parsers_ exist that have different behaviors.
Part of these differences can be blamed on bugs and mistakes
made by the JSON parser authors, and partly because of the way
that the [JSON.org](https://www.json.org/json-en.html) website reads
and changes and also because the _ECMA-404_ standard has some
subtle details.  All too often, a standard goes to great lengths
to avoid being a _tutorial_: for various reasons (both valid
and poor), standards writers tend to enjoy writing dense text that
is technicality correct but easily misunderstood.

There are a number of _JSON validators_ available that suffer from
problems similar to that mentioned for _JSON parsers_.  For example
the popular [jsonlint.com](https://jsonlint.com) will incorrectly
flag this JSON as invalid:

```json
{
	"foo": 1,
	"foo": 1
}
```

and produce the error message:

```
Error: Duplicate key 'foo'
```

even though the [ECMA-404 JSON Data Interchange
Standard](https://www.ecma-international.org/publications-and-standards/standards/ecma-404/)
says it is not an error and even though page 3 of [ECMA-404 2nd Edition / December
2017](https://www.ecma-international.org/wp-content/uploads/ECMA-404_2nd_edition_december_2017.pdf)
has the following text:

```
The JSON syntax does not impose any restrictions on the strings
used as names, does not require that name strings be unique, and
does not assign any significance to the ordering of name/value pairs
```

The authors of this document have identified a number of mis-features
of the **so-called JSON spec** in addition to how it fails to even
live up to various [claims made by the
creators](https://www.youtube.com/watch?v=x92vbAN_j1k&list=PLEzQf147-uEoNCeDlRrXv6ClsLDN-HtNm&index=2&t=1343s)
of the **so-called JSON spec**.

The authors of this document believe that the **so-called JSON
spec** is an example of "rank amateur specification writing" that
may serve as a case study for **how _not_ to write a specification**.
We end this **Substandard JSON spec editorial** with the following
question:

```
How many mis-features and flaws can you find in the "so-called JSON spec"? :-)
```
