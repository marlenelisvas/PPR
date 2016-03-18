#========================================== 	
# Autor:	Marlene Vásquez
#==========================================
.SUFFIXES:
.PHONY:    msg,sr1,sr2,sr3,clean
#==========================================
msg:	
	@echo "usa 'make sr1' para ejecutar calculo de PI "
	@echo "usa 'make sr2' para ejecutar calculo de PI "
	@echo "usa 'make sr3' para ejecutar calculo de PI "
	@echo "usa 'make clean' para borrar todos los archivos creados al compilar."
	
#==========================================
compilador := mpicxx
flagsc     := -g -Wall 
#==========================================
# 		send_receive1
#==========================================
sr1: send_receive1
	mpirun -np 10 send_receive1

#==========================================
# 	send_receive2
#==========================================
sr2: send_receive2
	mpirun -np 10 send_receive1

#==========================================
# 	send_receive3
#==========================================
sr3: send_receive3
	mpirun -np 10 send_receive1

#==========================================
#	ejecutables
#==========================================
%: %.cpp
	$(compilador) $(flagsc) -o $@ $<

#==========================================
# 	clean
#==========================================
clean:
	rm -f send_receive1 send_receive2 send_receive3 *~ 
