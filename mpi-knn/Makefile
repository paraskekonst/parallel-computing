# This is a very primitive makefile that would allow users unfamiliar
# with your system to compile and run your codes
#
# Nikos Pitsianis
#

SOURCE =knn_blocking
NPROCS =4
all:
	make run

$(SOURCE).out:	$(SOURCE).c	
	mpicc -O3 $< -o $(SOURCE).out -lm

run:	$(SOURCE).out
	# mpiexec -machinefile mpd.hosts -n $(NPROCS) ./$(SOURCE).out
	mpiexec  -n $(NPROCS) ./$(SOURCE).out

clean:


