dtach
=====

A light weight replacement for tmux,screen.  
which not mess your scrollback.

<br>

This is fork of https://github.com/crigler/dtach/  
The upstream seems no longer maintained (last commit was in 2017)

so I fork and store my changes here.


<br>

## What's New

- Easy-to-use wrapper, use dtach like tmux, screen.  
https://github.com/crigler/dtach/issues/24

- Save & Show session output history (depends on script, unterm)  
https://github.com/crigler/dtach/issues/24#issuecomment-2888560651

- Allow dtach from altscreen like vim,htop.. (not corrupt the termianl)  
https://github.com/crigler/dtach/issues/23

<br>

## Usage

### dtach.sh
//create new session, named test  
```console
$ dtach.sh new test
```

//dtach from session  
<kbd> Ctrl + \ </kbd>

//attach to session test  
```console
$ dtach.sh att test
```

//see output history of session test  
```console
$ dtach.sh log test
```

//only show last 20 line  
```console
$ dtach.sh log test 20
```

<br>

### dtach.gum
A [gum](https://github.com/charmbracelet/gum) based interactive session chooser:

//it will show you all sesions, choose by <kbd> ↑ </kbd> <kbd> ↓ </kbd>, confirm by <kbd> Enter </kbd>  
```console
$ dtach.gum
```
