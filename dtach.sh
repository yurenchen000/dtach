#

: <<'EOF'
dtach  -a ~/dtach_dir/ansible.sock -z -r winch
dtach  -A ~/dtach_dir/b2.sock -z bash
EOF

# SOCK_DIR=~/dtach_dir
SOCK_DIR=~/.dtach

# -----
echoR(){ echo -e "\e[31m$@\e[0m"; }    
echoG(){ echo -e "\e[32m$@\e[0m"; }    
echoY(){ echo -e "\e[33m$@\e[0m"; }    
echoH(){ echo -e "\e[01;32;40m$@\e[0m"; }

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

attach(){
  local name=$1
  [ -n "$name" ] || return
  dtach  -a $SOCK_DIR/$name.sock -z -r winch
  # dtach  -a $SOCK_DIR/$name.sock -z  # will broke scroll back
}

create(){
  local name=$1
  [ -n "$name" ] || return
  dtach  -A $SOCK_DIR/$name.sock -z -r winch bash 
}

#####
ls(){ list; }
att(){ attach "$@"; }
cre(){ create "$@"; }
new(){ create "$@"; }

"$@"

