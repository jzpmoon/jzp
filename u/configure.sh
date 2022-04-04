#!/usr/bin/bash
prefix=$(pwd)
envc="unx"
thw="arm64"

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
echo "currdir=$(pwd)" >> makefile

echo "include ../u/env/env_"${envc}".mk" >> makefile

cat libu.mk >> makefile

cat $(echo "./env/env_"${envc}".h") > umacro.h

cat $(echo "./env/thw_"${thw}".h") >> umacro.h

cat ./uexport.h | sed 's:xapi:uapi:g' | sed 's:xcall:ucall:g' | \
    sed 's:_UEXPORT_H_:_UEXPORT_LIBU_H_:g' >> umacro.h
