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

echo "prefix=$prefix" > makefile
echo "envc=$envc" >> makefile
echo "currdir=$(pwd)" >> makefile

if [ $envc = "unx" ]
then
    echo "include ../u/env/env_unx.mk" >> makefile
elif [ $envc = "win" ]
then
    echo "include ../u/env/env_win.mk" >> makefile
fi

cat libu.mk >> makefile
