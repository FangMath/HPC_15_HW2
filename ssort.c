/* Parallel sample sort
 */
#include <stdio.h>
#include <unistd.h>
#include <mpi.h>
#include <stdlib.h>


static int compare(const void *a, const void *b)
{
  int *da = (int *)a;
  int *db = (int *)b;

  if (*da > *db)
    return 1;
  else if (*da < *db)
    return -1;
  else
    return 0;
}

int main( int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Functions need one input as number of integers to be sorted!\n");
        abort();
    }

  int rank, P, root;
  int i, j, totalN, N, S, newN;
  int *vec, *Svec;
  int *PSvec, *splitter, *chunk, *sendcnts, *sdispls, *newvec, *recvcnts, *rdispls;
  double T1, T2;


  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &P);
  root = 0;

  /******** sort starts, time it ************/
  MPI_Barrier(MPI_COMM_WORLD);
  T1 = MPI_Wtime();     /* start the clock */

  /* Number of random numbers per processor (this should be increased
   * for actual tests or could be passed in through the command line */
  N = atoi(argv[1]);
  totalN = N*P;
  N = totalN/P;
  if (rank == 0){
  printf("The total amount of integers to be sorted is %d.\n", totalN);
  printf("Each processor has %d integers.\n", N);
  }

  vec = calloc(N, sizeof(int));

  /* seed random number generator differently on every core */
  srand((unsigned int) (rank + 393919));

  /* fill vector with random integers */
  for (i = 0; i < N; ++i) {
    vec[i] = rand();
  }

  /* sort locally */
  qsort(vec, N, sizeof(int), compare);


  /* randomly sample s entries from vector or select local splitters,
   * i.e., every N/P-th entry of the sorted vector */
  /* Number of sample entries from each chunk, need to be re-considered later */
  S = N/10;
  if (S < 2) S = 2;
  Svec = calloc(S, sizeof(int));
  for (i = 0; i < S; ++i) {
      Svec[i] = vec[i*((N-1)/(S-1))];
  }

  /* every processor communicates the selected entries
   * to the root processor; use for instance an MPI_Gather */
  PSvec = calloc(S*P, sizeof(int));
  MPI_Gather(Svec, S, MPI_INT, PSvec, S, MPI_INT, root, MPI_COMM_WORLD);

  /* root processor does a sort, determinates splitters that
   * split the data into P buckets of approximately the same size */
  splitter = calloc(P-1, sizeof(int));
  if (rank == 0){
  qsort(PSvec, P*S, sizeof(int), compare);

  for (i = 1; i < P; ++i) {
      splitter[i-1] = PSvec[(i*(P*S-1))/P];
  }
  }

  /* root process broadcasts splitters */
  MPI_Bcast(splitter, P-1, MPI_INT, root, MPI_COMM_WORLD);

  /* every processor uses the obtained splitters to decide
   * which integers need to be sent to which other processor (local bins) */

  /* the data before chunk[i] is smaller than splitter[i], and is to be sent to processor i */
  chunk = calloc(P-1, sizeof(int));
  chunk[0] = 0; 
  for (i = 0; i < P-1; ++i){
      if (i > 0) chunk[i] = chunk[i-1];
      for (j = chunk[i]; j < N; ++j){
          if (vec[j] >= splitter[i]){
          chunk[i] = j; break;
          }
      }
  }

  /* send and receive: either you use MPI_AlltoallV, or
   * (and that might be easier), use an MPI_Alltoall to share
   * with every processor how many integers it should expect,
   * and then use MPI_Send and MPI_Recv to exchange the data */


  /* set send counts */
  sendcnts = calloc(P, sizeof(int));
  sendcnts[0] = chunk[0];
  for (i = 1; i < P-1; ++i){
      sendcnts[i] = chunk[i] - chunk[i-1];
  }
  sendcnts[P-1] = N - chunk[P-2];

  /* set send displacements */
  sdispls = calloc(P, sizeof(int));
  sdispls[0] = 0;
  for (i = 1; i < P; ++i){
      sdispls[i] = sdispls[i-1] + sendcnts[i-1];
  }
  
  /* use MPI_Alltoall to tell each processor
   * how large the chunk it should expect, 
   * i.e. the receive count */
  recvcnts = calloc(P, sizeof(int));
  MPI_Alltoall(sendcnts, 1, MPI_INT, recvcnts, 1, MPI_INT, MPI_COMM_WORLD);

  /* set receive displacements */
  rdispls = calloc(P, sizeof(int));
  rdispls[0] = 0;
  for (i = 1; i < P; ++i){
      rdispls[i] = rdispls[i-1] + recvcnts[i-1];
  }

  newN = recvcnts[P-1] + rdispls[P-1];
  newvec = calloc(newN, sizeof(int)); // receive buffer

  MPI_Alltoallv(vec, sendcnts, sdispls, MPI_INT, newvec, recvcnts, rdispls, MPI_INT, MPI_COMM_WORLD);

  /* do a local sort */
  printf("rank %d received data chunk of size %d \n", rank, newN);
  qsort(newvec, newN, sizeof(int), compare);

  /******** sort is done, time it again ************/
  MPI_Barrier(MPI_COMM_WORLD);
  T2 = MPI_Wtime();     /* end time */
  printf("Task %d time(wall)= %lf sec\n", rank, T2-T1);

  /* every processor writes its result to a file */
  {
    FILE* fd = NULL;
    char filename[256];
    snprintf(filename, 256, "./output/ssort%02d.txt", rank);
    fd = fopen(filename,"w+");

    if(NULL == fd)
    {
      printf("Error opening file \n");
      return 1;
    }

    for (i = 0; i < newN; ++i){
    fprintf(fd, "%d\n", newvec[i]);
    }

    fclose(fd);
  }

  if (rank == 0) free(PSvec);
  free(vec);
  free(Svec);
  free(splitter);
  free(chunk);
  free(newvec);
  free(sendcnts);
  free(sdispls);
  free(recvcnts);
  free(rdispls);


  MPI_Finalize();
  return 0;
}
