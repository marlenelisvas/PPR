#========================================== 	
# Autor:	Marlene Vásquez
#==========================================
.SUFFIXES:
.PHONY:    msg,p1,p2,p3,clean
#==========================================
msg:	
	@echo "usa 'make p1' para ejecutar calculo de PI solución inicial"
	@echo "usa 'make p2' para ejecutar calculo de PI solucion paralela"
	@echo "usa 'make p3' para ejecutar calculo de PI solucion distribuida por bloques"
	@echo "usa 'make clean' para borrar todos los archivos creados al compilar."
#==========================================
compilador := mpicxx
flagsc     := -g -Wall 

#==========================================
#	PI  solución inicial
#==========================================
p1: pi1
	mpirun -np  4 pi1 13
#==========================================
#	PI  solucion paralela
#==========================================
p2: pi2
	mpirun -np 4 pi2  13
#==========================================
#	PI  solucion distribuida por bloques
#==========================================
p3: pi3
	mpirun -np 4 pi3  23
#==========================================
#	ejecutables
#==========================================
%: %.cpp
	$(compilador) $(flagsc) -o $@ $<
#==========================================
#	clean  
#==========================================
clean:
	rm -f pi1 pi2 pi3 *~ 
