#!/bin/bash

gcc -DE07 -o a2z a2z.c

for FILE in a; do
  echo -n "k#s '${FILE}' #$FILE " > $FILE.a
done

for FILE in ex7 ex8 exasp; do
  echo -n "k#s '${FILE}' #$FILE " > $FILE.a
  ./a2z < $FILE.asm > $FILE.z
done

