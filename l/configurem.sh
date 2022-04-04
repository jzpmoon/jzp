#!/usr/bin/bash
prefix=$(pwd)
envc="unx"
thw=arm64
currdir=$(pwd)

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

mk=makefile_m

echo "prefix=$prefix" > $mk
echo "envc=$envc" >> $mk
echo "thw=$thw" >> $mk
echo "currdir=$currdir" >> $mk

echo "include ../u/env/env_"${envc}".mk" >> $mk

cat liblmod.mk >> $mk

cd ../v/
./configure.sh --prefix=$prefix --envc=$envc --thw=$thw
cd $currdir

echo "/*auto create*/" > lmmacro.h

cat ../u/uexport.h | sed 's:xapi:lmapi:g' | sed 's:xcall:lmcall:g' | \
    sed 's:_UEXPORT_H_:_UEXPORT_LIBLM_H_:g' >> lmmacro.h
