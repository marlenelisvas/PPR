#!/bin/bash
#FICH=salidaSIN.txt
FICH=salida.txt
N=1;
LIMIT=10

echo "*******400******** ">> $FICH 

for ((a=0; a<LIMIT; a++))
do
 mpirun  -np $N  ./floyd ../input/input400 >> $FICH 
done

echo " *****800******\n ">>$FICH 

for ((a=0; a<LIMIT; a++))
do
 mpirun  -np $N   ./floyd ../input/input800 >> $FICH 
done

echo " *****1200******\n ">>$FICH 

for ((a=0; a<LIMIT; a++))
do
 mpirun  -np $N  ./floyd ../input/input1200 >> $FICH 
done

echo " *****1600******\n ">>$FICH 

for ((a=0; a<LIMIT; a++))
do
 mpirun  -np $N ./floyd ../input/input1600 >> $FICH 
done
