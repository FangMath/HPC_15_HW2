EXECUTABLES = mpi_solved1 mpi_solved2 mpi_solved3 mpi_solved4 mpi_solved5 mpi_solved6 mpi_solved7
COMPILER = mpicc
FLAGS = -O3 -Wall -lrt

all: $(EXECUTABLES)

mpi_solved1: mpi_solved1.c
	$(COMPILER) $(FLAGS) mpi_solved1.c -o mpi_solved1 

mpi_solved2: mpi_solved2.c
	$(COMPILER) $(FLAGS) mpi_solved2.c -o mpi_solved2 

mpi_solved3: mpi_solved3.c
	$(COMPILER) $(FLAGS) mpi_solved3.c -o mpi_solved3 

mpi_solved4: mpi_solved4.c
	$(COMPILER) $(FLAGS) mpi_solved4.c -o mpi_solved4 

mpi_solved5: mpi_solved5.c
	$(COMPILER) $(FLAGS) mpi_solved5.c -o mpi_solved5 

mpi_solved6: mpi_solved6.c
	$(COMPILER) $(FLAGS) mpi_solved6.c -o mpi_solved6 

mpi_solved7: mpi_solved7.c
	$(COMPILER) $(FLAGS) mpi_solved7.c -o mpi_solved7 

clean:
	rm -rf $(EXECUTABLES)
