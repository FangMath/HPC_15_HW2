# HPC_15_HW2

This is Fang's solution to HPC homework2

#******************************************#
to debug with valgrind:
>> mpirun -np 8 valgrind ./ssort 1000000

* First time login
to login stampede:
ssh XSEDEusername@login.xsede.org
pwd...
gsissh -p 2222 stampede.tacc.xsede.org
(see https://portal.xsede.org/web/xup/single-sign-on-hub)

* Second time login
ssh tg827502@stampede.tacc.utexas.edu

sbatch job

squeue -u tg827502
scancel jobnumber

results/error messages are in xxx.o file

