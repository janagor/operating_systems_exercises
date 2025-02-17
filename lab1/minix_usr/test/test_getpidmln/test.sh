#!/bin/sh

cc t_getpidmln.c -o t

for i in 1 2 3 4 5
do
  ./t ${i}
done
