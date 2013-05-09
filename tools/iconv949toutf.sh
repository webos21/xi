#!/bin/bash

CKISO=ISO
CKUTF=UTF
FILES=`find ./ -name "*.java"`

for i in $FILES;
do 
TESTF=`file $i`
#TTTT=`echo "$TESTF" | grep -q "$CKISO"`
#echo "$TTTT";
if echo "$TESTF" | grep -q "$CKISO"
then
	echo $TESTF;
	iconv -f "CP949" -t "UTF-8" $i -o $i."utf8";
	mv $i."utf8" $i;
fi
done;


