#!/bin/bash
cnt=0
file="text/test"
mtf="mtf/test"
result="result"
A="pwd"
B=$(eval $A)
gcc -o text2mtf text2mtf.c
while [ $cnt -le 10 ]
do
    
    if [ $cnt == 10 ]; then
        echo "running test${cnt}.txt"
        ./text2mtf "${file}${cnt}.txt"
        if  diff "${mtf}${cnt}.mtf" "${file}${cnt}.mtf"; then
            mv "${file}${cnt}.mtf" ${result}
            echo "test${cnt} PASSED"
        else
            rm "${file}${cnt}.mtf"
            echo "test${cnt} FAILED"
        fi
    cnt=$(($cnt + 1))
    
    else
        echo "running ${file}0${cnt}.txt" 
        ./text2mtf "${file}0${cnt}.txt"
         if  cmp "${mtf}0${cnt}.mtf" "${file}0${cnt}.mtf" ; then
            mv "${file}0${cnt}.mtf" ${result}
            echo "test0${cnt} PASSED"
        else
            rm "${file}0${cnt}.mtf"
            echo "test0${cnt} FAILED"
        fi
    cnt=$(($cnt + 1))
    fi
done

   # if (diff ${results})