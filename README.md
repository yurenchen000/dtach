🖥️ dtach
========

A light weight replacement for `tmux`, `screen`.  
which not mess your scrollback.

<br>

This is fork of https://github.com/crigler/dtach/  
The upstream seems no longer maintained (last commit was in 2017)

so I fork and store my changes here.


<br>

## ✨ What's New

- Easy-to-use wrapper, use dtach like tmux, screen.  
https://github.com/crigler/dtach/issues/24

- Save & Show session output history (depends on script, unterm)  
https://github.com/crigler/dtach/issues/24#issuecomment-2888560651

- Custom bashrc at `~/.dtach_bashrc`,  with `DTACH_NAME` env  
 can set independent bash_history or PS1 based on session name 

- Allow dtach from altscreen like vim,htop.. (not corrupt the termianl)  
https://github.com/crigler/dtach/issues/23

<br>

## 📚 Usage

### 📜 dtach.sh
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

### 📜 dtach.gum
A [gum](https://github.com/charmbracelet/gum) based interactive session chooser:

//it will show you all sesions, choose by <kbd> ↑ </kbd> <kbd> ↓ </kbd>, confirm by <kbd> Enter </kbd>  
```console
$ dtach.gum
```

<br>

## 🛠️ Build & Install

### build



//A. build on ubuntu22+  (ubuntu18 need build libvterm from source: https://www.leonerd.org.uk/code/libvterm/
```sh
sudo apt install libvterm-dev # libvterm-dev 0.1.4 for unterm build

./configure
make dtach
make unterm
```

OR 

//B. download pre-built binary for ubuntu18+, amd64  
https://github.com/yurenchen000/dtach/releases/


### install
//1. install depends
```sh
sudo apt install bsdutils ## for script cmd, usually installed by default
sudo apt install gum      ## for gum, OR download from https://github.com/charmbracelet/gum/releases
```

//2. install files
```sh
sudo cp -pvi dtach unterm dtach.sh dtach.gum  /usr/local/bin/
```

<br>

## ⚙️ Config


### 📜 .dtach_bashrc

`~/.dtach_bashrc` example
```bash
## vi: ft=bash

. ~/.bashrc

PS1='d\[\033[01;32m\]\u@\h\[\033[00m\]:\[\033[01;34m\]\w\[\033[00m\]\$ '

export LC_ALL=C.UTF-8
export TIME_STYLE=long-iso

[ -n "$DTACH_NAME" ] && \
export HISTFILE=~/.dtach/"$DTACH_NAME.hist"
```

<br>

## Related Tools

[![related-repos](https://res.ez2.fun/svg/repos-ssh_enhance.svg)](https://github.com/yurenchen000/yurenchen000/blob/main/repos.md#ssh-enhance)

