<!--
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.

Copyright (c) 2021 David Jackson
-->

# Auteur: Screenwriting in Plain Text

Auteur is a screenwriting program that will take a plain-text file (containing
special markup) and transform it into a printable screenplay in a standard
screenplay format.

## Usage

Auteur is used as follows:

```sh
$ auteur [your_screenplay] > screenplay.ps
```

You can convert the generated PostScript to a PDF using any tool that will do
so. For example, if you have Ghostscript installed:

```sh
$ ps2pdf screenplay.ps
```

## Auteur Markup

The auteur markup format takes inspiration from
[nroff](https://en.wikipedia.org/wiki/Nroff). Each of its directives has the
format `.Xx` where the two Xs are letters, the first capitalized, the second in
lowercase.

### Directives

* `.Tl [TITLE]` -- Title of the screenplay
* `.Au [AUTHOR]` -- Author of the screenplay
* `.Sl [SLUG LINE]` -- Slug line / Scene
* `.Ac [ACTION]` -- Action description
* `.Tr [TRANSITION]` -- Transition, e.g. "cut to"
* `.Ch [CHARACTER]` -- Character ID for dialog
* `.Pa [PARENTHETICAL]` -- Dialogue parenthetical (manner of delivery)
* `.Dl [DIALOG]` -- Dialogue from a character
* `.Np` -- New page
* `.% [COMMENT]` -- Comments (not printed)

### Example of a Screenplay

```txt
.Tl My Screenplay
.Au A. Writer
.Np

.Sl ext. field - day
.Ac A MAN is standing in a field.

.Ch Man
.Dl How did I get here?

.Tr cut to
.Sl. int. pub - evening
.Ac We see the Man drinking heavily.

.Ch Man
.Pa Shouting
.Dl Best birthday ever!

...
```

## Building

Auteur can be built with a simple `make` command.

## License

Auteur is licensed under the [MPL-2.0](https://www.mozilla.org/en-US/MPL/2.0/)
