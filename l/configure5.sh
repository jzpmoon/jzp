#!/usr/bin/bash
prefix=$(pwd)
envc="unx"
thw=arm64

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
    elif [ $key = "--thw" ]
    then
	thw=$val
    fi
    IFS=$old_ifs
done

echo "prefix=$prefix" > makefile
echo "envc=$envc" >> makefile
echo "thw=$thw" >> makefile
echo "currdir=$(pwd)" >> makefile

echo "include ../u/env/env_"${envc}".mk" >> makefile

cat libl5.mk >> makefile
