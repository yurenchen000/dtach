#!/bin/bash
# vi: ts=2 sw=2 et

: <<'EOF'
dtach  -a ~/dtach_dir/ansible.sock -z -r winch
dtach  -A ~/dtach_dir/b2.sock -z bash
EOF

# SOCK_DIR=~/dtach_dir
SOCK_DIR=~/.dtach
dir=`dirname $(readlink -f $BASH_SOURCE)`

# -----
echoR(){ echo -e "\e[31m$@\e[0m"; }    
echoG(){ echo -e "\e[32m$@\e[0m"; }    
echoY(){ echo -e "\e[33m$@\e[0m"; }    
echoH(){ echo -e "\e[01;32;40m$@\e[0m"; }

# -----
get_cur(){
  local out cur
  out=`pstree -asT $$ -G | grep '[d]tach'` && {
    cur=`echo "$out" | grep -oP '[^/ ]*(?=.sock)' | tail -1`
  	[ -n "$cur" ] && {
		echo "$cur"
		return 0
	}
  }
  return 1
}

# -----
list(){
  local f s
  local out
  local cur
  local info
  out=`pstree -asT $$ -G | grep '[d]tach'` && {
    cur=`echo "$out" | grep -oP '[^/ ]*(?=.sock)' | tail -1`
    [ "$1" == "-v" ] && {
      echo "> current in dtach: "
      echo "$out" | grep -P '[^/ ]*(?=.sock)' --color
      # echo "cur: $cur"
      echo
    }
  }
  [ "$1" == "-v" ] && info='   <==current'

  for f in $SOCK_DIR/*.sock; do
    [ -S "$f" ] || continue # sock or link to sock
    # echo 
    # echo "f: $f"
    s=${f##*/}
    # echo $s
    s=${s%.sock}
    if [ "$s" == "$cur" ]; then
      echoG "$s$info"
    else
      echo "$s"
    fi
  done
}

## note: -r ctrl_l may not always work, for example in vim insert mode
attach(){
  local name=$1
  [ -n "$name" ] || return
  local cur=`get_cur`
  [ -n "$cur" ] && {
    echo -e " current in \e[33m$cur\e[0m, not attach!"
    return
  }

  dtach  -a $SOCK_DIR/$name.sock -z -r winch
  # dtach  -a $SOCK_DIR/$name.sock -z -r ctrl_l
  # dtach  -a $SOCK_DIR/$name.sock -z  # will broke scroll back
}

create(){
  local name=$1
  [ -n "$name" ] || return
  dtach  -A $SOCK_DIR/$name.sock -z -r winch  script ~/.dtach/$name.out -f -c 'bash --rcfile ~/.dtach_bashrc'
  #dtach  -A $SOCK_DIR/$name.sock -z -r winch bash --rcfile ~/.dtach_bashrc
  # dtach  -A $SOCK_DIR/$name.sock -z -r ctrl_l bash --rcfile ~/.dtach_bashrc
}

show_hist(){
  local name=$1
  local line=${2:-999}
  [ -n "$name" ] || return
  local cur=`get_cur`
  [ "$name" == "$cur" ] && {
    echo -e " current in \e[33m$cur\e[0m, not show log here (avoid loop)"
    return 1
  }

  local time=`stat -c'%y' ~/.dtach/$name.out | awk '{sub(/\..*/, "", $2); print $1" "$2}'`
  read LINES COLUMNS < <(stty size)
  echoY "--------old hist--------(( $COLUMNS x $LINES"
  unterm -c $COLUMNS ~/.dtach/$name.out | tail -n $line
  # cat ~/.dtach/$name.out | \
  #   COLUMNS=$COLUMNS $dir/vterm_strip | \
  #   tail -n $line

    # perl -pe 'BEGIN { $/=undef } s/\x1B\[\?1049h.*?(\x1B\[\?1049l|$)//sg; s/\x1B\[\?\d{1,4}\$p//g;' | \
    # python3 $dir/pyte_strip.py |

  ### Accurate but slow
  # cat ~/.dtach/$name.out | python3 $dir/pyte_strip.py | tail -n $line
  ### Rough but fast
  # cat ~/.dtach/$name.out | perl -pe 'BEGIN { $/=undef } s/\x1B\[\?1049h.*?(\x1B\[\?1049l|$)//sg; s/\x1B\[\?\d{1,4}\$p//g;' | tail -n $line
  echo
  echoY "--------old hist--------)) $time"
  #cat ~/.dtach/$name.out | sed -e '/\x1B\[?1049h/,/\x1B\[?1049l/d' | tail -n $line; echo --------old hist--------
}

#####
ls(){ list "$@"; }
att(){ attach "$@"; }
cre(){ create "$@"; }
new(){ create "$@"; }
log(){ show_hist "$@"; }

help(){
  cat <<EOF
  ls
  list

  att[ach] NAME

  cre[ate] NAME
  new      NAME
EOF
  exit
}

[ -z "$1" -o "$1" == "-h" ] && help

"$@"

