#!/bin/bash

function echofile(){
    file=$1
    if [[ "${file##*.}"x = "hh"x ]];then
        echo $1
    fi
}

function getdir(){
    for elem in `ls $1`
    do
        file=$1"/"$elem
        if [ -d $file ]
        then
            getdir $file
        else
            echofile $file
        fi
    done
}

root_dir="$1"
getdir $root_dir
