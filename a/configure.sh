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

entry_exec=jzpl.sh
echo '#!/usr/bin/bash' > $entry_exec
echo "prefix=$prefix" >> $entry_exec
echo 'export PATH=$PATH:$prefix' >> $entry_exec
echo 'export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$prefix' >> $entry_exec
echo '$prefix/jzpl $*' >> $entry_exec
chmod 700 $entry_exec

makefile
echo "prefix=$prefix" > makefile
echo "envc=$envc" >> makefile
echo "thw=$thw" >> makefile
echo "currdir=$(pwd)" >> makefile
echo "entry_exec=$entry_exec" >> makefile
echo "include ../u/env/env_"${envc}".mk" >> makefile

cat jzpl.mk >> makefile
