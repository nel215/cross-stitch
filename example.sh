#!/bin/bash
mkdir -p tmp
rm tmp/*
for i in `seq 1 10`
do
    echo "start $i th testcase"
    java -jar ./tester.jar -exec ./a.out -seed $i -novis > "tmp/$i" &
done
wait
python gen-report.py
