#! /bin/bash

for i in $(seq 1 10); do
	perl separator_big5.pl testdata_old/${i}.txt > testdata/${i}.txt
done