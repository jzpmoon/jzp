#!/usr/bin/bash
cfun=""
out="_temp.cfun"
callback="_cfun_init"

old_ifs=$IFS
for arg in $*
do
    IFS="="
    array=($arg)
    key=${array[0]}
    val=${array[1]}
    if [ $key = "--cfun" ]
    then
	cfun=$val
    elif [ $key = "--out" ]
    then
	out=$val
    elif [ $key = "--callback" ]
    then
	callback=$val
    fi
    IFS=$old_ifs
done

if [ $cfun != "" ]
then
    echo "#include \"vcfun.h\"" > $out
    cat $(echo ${cfun}.cfun | sed 's:,:.cfun :g') >> $out
    echo "void ${callback}(vcontext* ctx,vmod* mod){ \
            _vcfun_file_init_$(echo ${cfun} | \
            sed 's:,:(ctx,mod);_vcfun_file_init_:g')(ctx,mod); \
          }" >> $out
fi
