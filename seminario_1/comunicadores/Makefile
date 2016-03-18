#========================================== 	
# Autor:	Marlene Vásquez
#==========================================
.SUFFIXES:
.PHONY:    msg,cm1,cm2, cm3, clean
#==========================================
msg:
	@echo "usa 'make cm1' para ejecutar comunicadores - solución inicial."
	@echo "usa 'make cm2' para ejecutar comunicadores - solucion modificada."
	@echo "usa 'make clean' para borrar todos los archivos creados al compilar."
#==========================================
compilador := mpicxx
flagsc     := -g -Wall
#==========================================
#	COMUNICADORES1
#==========================================
cm1: comunicadores1
	mpirun -np 5 comunicadores1

#==========================================
#	COMUNICADORES 2
#==========================================
cm2: comunicadores2
	mpirun -np 15 comunicadores2

#==========================================
#	COMUNICADORES 3
#==========================================
cm3: comunicadores3
	mpirun -np 15 comunicadores3
#==========================================
#	ejecutables
#==========================================
%: %.cpp
	$(compilador) $(flagsc) -o $@ $<

#==========================================
#	clean
#==========================================
clean:
	rm -f comunicadores1 comunicadores2 comunicadores3 *~ 
