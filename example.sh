#!/bin/bash
mkdir -p tmp
for i in `seq 1 10`
do
    echo "start $i th testcase"
    java -jar ./tester.jar -exec ./a.out -seed $i -novis > "tmp/score$i" &
done
wait
cat tmp/score* | grep "^Score = "
