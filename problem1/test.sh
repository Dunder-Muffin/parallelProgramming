#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "illegal number of parameters"
    echo "usage: test.sh <binary>"
    exit 1
fi

BIN=$1

DATA="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"/"testdata"
rm $DATA/*_tmp*

for typ in {l,d}; do
    for i in {1..3}; do
        for mod in {0..5}; do
            $BIN ${DATA}/a_${i}_${typ} ${DATA}/b_${i}_${typ} ${DATA}/c_${i}_${typ}_tmp_${mod} ${mod}
            if [ $? -ne 0 ]; then
                exit 2
            fi
            if cmp ${DATA}/c_${i}_${typ} ${DATA}/c_${i}_${typ}_tmp_${mod}; then
                continue
            else
                echo "${DATA}/c_${i}_${typ} ${DATA}/c_${i}_${typ}_tmp_${mod} differ"
                exit 1
            fi
        done
    done
done
