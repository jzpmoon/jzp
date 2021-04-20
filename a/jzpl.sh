#!/usr/bin/bash
dir="$(cd "$(dirname "$0" )" && pwd)"

export PATH=$PATH:$dir
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$dir

./jzpl $*
