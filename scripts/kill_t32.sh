#!/bin/bash

for pid in $(pgrep t32*) 
do
    echo $pid
    kill -9 $pid
done
