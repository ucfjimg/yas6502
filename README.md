# yas6502

yas6502 is yet another 6502 assembler. I wrote it for a couple of reasons: I had an older assembler
that I was using for my own projects, but it didn't support undocumented opcodes, the lexer and 
parser were written from scratch instead of using FLEX and BISON, and I generally just wasn't happy
with the code. I also wanted an assembler that was mostly implemented as a library so I could embed
it inside other tools.

Currently the code has been tested on Linux. It should also work without modification on macOS, and
on Windows with minor modifications.

## Building

Building is via CMAKE and requires FLEX and BISON as well as a C++ compiler. A cmake file is provided
for finding the installed headers and libraries.


## Dialect

The assembly recognized by yas6502 is fairly standard, with a few things that would be nice to add 
in the future. The indirect addressing modes are specifed with brackets rather than parentheses. The
assembler will issue warnings on attempts to use parentheses as indirection modes. The SET directive 
sets the value of a symbol. BYTE and WORD put values into memory. The ORG directive sets the location 
counter. 

```
        ORG     $F000
        SET     ZPADDR = $10+2*4
LABEL:  LDA     [ZPADDR,X]
        LDA     [ZPADDR],Y
        JMP     [LABEL]
        ORG     $FF00                          ; start of data
BYTES:  BYTE    $00, $01, $02
WORDS:  WORD    $1234, $2345
        END
```

Expressions can currently only use the four algebraic operators +, -, * and /.\, and parentheses.

## Object files

yas6502 is meant for single-file programs. There is no support for external linkage. As a result, object
files are very simple and just place code and data directly in memory at an absolute address. This means
they are very amenable to immediate conversion into any binary ROM format. There are only two things in
the object format: an address of the form @XXXX, where XXXX is a 16-bit address in hex, and XX, where 
XX is an 8-bit data byte in hex. All entries are white space delimited. Here is a simple object file which 
is a typical ROM start: SEI followed by CLD:

```
@F000
78 D8
```

## The listing file

Most of the listing file is self explanatory. There are some single-character codes in the instruction
encodings, however, that bear explanation.

```
  159 F014  AB 01             2  US            LAX #$01  
  160 F016  B9 BB F5          4+               LDA FONT,Y              ; Get the font bits
```

The lines start with the source line number, the address, and the instruction encoding that. Following 
that is the number of clock cycles. If the instruction can have penalties (such as for page crossing or a taken
branch), the clock cycles will be shown with a '+'.  In the first line, 'U' means that the instruction is
undocumented, and 'S' means that it is also reported to be unstable on some processors.

## TODO

There are a few features that I would consider essential for a production assembler that aren't there 
yet. I hope to have time to add them in the near future. They are

  - Robust expressions. At the very least logical operators; ideally as many of the operators in C as make sense.
  - A way to express ASCII strings without resorting to hex.
  - Ephemeral labels for short branches; e.g. a way to specify non-unique labels where references refer to the nearest instance.
  
Nice to have but less essential would be multi-file assembly and a macro facility.

## Revision history

### 13-May-2020 V0.01

Initial release.

