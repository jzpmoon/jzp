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

entry_exec=jzpl.sh
echo '#!/usr/bin/bash' > $entry_exec
echo "prefix=$prefix" >> $entry_exec
echo 'export PATH=$PATH:$prefix' >> $entry_exec
echo 'export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$prefix' >> $entry_exec
echo '$prefix/jzpl $*' >> $entry_exec
chmod 700 $entry_exec

entry_mk=makefile
echo "prefix=$prefix" > $entry_mk
echo "currdir=$(pwd)" >> $entry_mk
echo "entry_exec=$entry_exec" >> $entry_mk
cat jzpl.mk >> $entry_mk
