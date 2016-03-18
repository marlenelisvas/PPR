#========================================== 	
# Autor:	Marlene Vásquez
#==========================================
.SUFFIXES:
.PHONY:    msg,ps1,ps2,ps3,clean
#==========================================	
msg:	
	@echo "usa 'make ps1' para ejecutar  el producto escalar - solucion inicial."
	@echo "usa 'make ps2' para ejecutar  el producto escalar - solucion paralela."
	@echo "usa 'make ps3' para ejecutar  el producto escalar - solucion propuesta."
	@echo "usa 'make clean' para borrar todos los archivos creados al compilar."
#==========================================
compilador := mpicxx
flagsc     := -g -Wall 

#==========================================
#	PRODUCTO ESCALAR - solucion inicial
#==========================================
ps1: productoescalar1
	mpirun -np 10 productoescalar1 100

#==========================================
#	PRODUCTO ESCALAR - solucion paralela
#==========================================
ps2: productoescalar2
	mpirun -np 10 productoescalar2 100

#==========================================
#	PRODUCTO ESCALAR - solucion propuesta
#==========================================
ps3: productoescalar3
	mpirun -np 10 productoescalar3 100

#==========================================
#	ejecutables
#==========================================
%: %.cpp
	$(compilador) $(flagsc) -o $@ $<
#==========================================
#	clean
#==========================================
clean:
	rm -f productoescalar1 productoescalar2 productoescalar3 *~ 
