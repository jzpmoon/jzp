#!/usr/bin/bash
prefix=$(pwd)
envc="unx"
thw="arm64"
smk="libv.mk"
dmk="makefile"
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
	elif [ $key = "--smk" ]
    then
	smk=$val
	elif [ $key = "--dmk" ]
    then
	dmk=$val
    fi
    IFS=$old_ifs
done

echo "prefix=$prefix" > $dmk
echo "envc=$envc" >> $dmk
echo "thw=$thw" >> $dmk
echo "currdir=$currdir" >> $dmk

echo "include ../u/env/env_"${envc}".mk" >> $dmk

cat $smk >> $dmk

cd ../u/
./configure.sh --prefix=$prefix --envc=$envc --thw=$thw
cd $currdir
