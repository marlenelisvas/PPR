#!/bin/bash
#floyd 1D
FICH=salida4.txt
NT=4;
echo "*******400******** ">> $FICH 
LIMIT=10
for ((a=0; a<LIMIT; a++))
do
 ./floyd ../input/input400  $NT>> $FICH 
done

echo " *****800******\n ">>$FICH 

for ((a=0; a<LIMIT; a++))
do
 ./floyd ../input/input800 $NT >> $FICH 
done

echo " *****1200******\n ">>$FICH 

for ((a=0; a<LIMIT; a++))
do
 ./floyd ../input/input1200 $NT >> $FICH 
done

echo " *****1600******\n ">>$FICH 

for ((a=0; a<LIMIT; a++))
do
 ./floyd ../input/input1600 $NT >> $FICH 
done
