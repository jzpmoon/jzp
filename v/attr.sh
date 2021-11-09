#!/usr/bin/bash
attr=""
out="_temp.attr"
callback="_attr_init"

old_ifs=$IFS
for arg in $*
do
    IFS="="
    array=($arg)
    key=${array[0]}
    val=${array[1]}
    if [ $key = "--attr" ]
    then
	attr=$val
    elif [ $key = "--out" ]
    then
	out=$val
    elif [ $key = "--callback" ]
    then
	callback=$val
    fi
    IFS=$old_ifs
done

if [ $attr != "" ]
then
    echo "#include \"vparser.h\"" > $out
    cat $(echo ${attr}.attr | sed 's:,:.attr :g') >> $out
    echo "void ${callback}(vtoken_state* ts){ \
            _vattr_file_init_$(echo ${attr} | \
            sed 's:,:(ts);_vattr_file_init_:g')(ts); \
          }" >> $out
fi
