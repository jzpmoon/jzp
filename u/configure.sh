#!/usr/bin/bash
prefix=$(pwd)

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
    fi
    IFS=$old_ifs
done

echo "prefix=$prefix" > makefile
echo "currdir=$(pwd)" >> makefile
cat libu.mk >> makefile
