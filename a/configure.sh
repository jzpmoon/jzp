#!/usr/bin/bash
prefix=$(pwd)
envc="unx"

old_ifs=$IFS
for arg in $*
do
    IFS="="
    array=($arg)
    key=${array[0]}
    val=${array[1]}
    if [ $key = "--prefix" ]
    then
	prefix=$val
    elif [ $key = "--envc" ]
    then
	envc=$val
    fi
    IFS=$old_ifs
done

entry_exec=jzpl.sh
echo '#!/usr/bin/bash' > $entry_exec
echo "prefix=$prefix" >> $entry_exec
echo 'export PATH=$PATH:$prefix' >> $entry_exec
echo 'export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$prefix' >> $entry_exec
echo '$prefix/jzpl $*' >> $entry_exec
chmod 700 $entry_exec

entry_mk=makefile
echo "prefix=$prefix" > $entry_mk
echo "envc=$envc" >> $entry_mk
echo "currdir=$(pwd)" >> $entry_mk
echo "entry_exec=$entry_exec" >> $entry_mk

if [ $envc = "unx" ]
then
    echo "include ../u/env/env_unx.mk" >> $entry_mk
elif [ $envc = "win" ]
then
    echo "include ../u/env/env_win.mk" >> $entry_mk
fi

cat jzpl.mk >> $entry_mk
