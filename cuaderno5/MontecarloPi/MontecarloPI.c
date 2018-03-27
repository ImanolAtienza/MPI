/* compute pi using Monte Carlo method */
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"
#define CHUNKSIZE      1000
/* message tags */
#define REQUEST  1
#define REPLY    2
void RandServer (MPI_Comm world)
{
    MPI_Status status;
    int request,i,rands[CHUNKSIZE];

    do {
	    MPI_Recv(&request, 1, MPI_INT, MPI_ANY_SOURCE, REQUEST,
		     world, &status);
	    if (request) {
		for (i = 0; i < CHUNKSIZE; ) {
		    rands[i] = random();
		    if (rands[i] <= INT_MAX) i++;
		}
		MPI_Send(rands, CHUNKSIZE, MPI_INT,
			 status.MPI_SOURCE, REPLY, world);
	    }
	}
	while(request > 0);
}
void Worker (int server,MPI_Comm world, MPI_Comm workers, double epsilon,
             int *stotalin, int *stotalout)
{
    int request,i,done, in, out, max, workerid;
    int iter,totalin,totalout;
    int rands[CHUNKSIZE];
    double x, y, Pi, error;

	request = 1;
	done = in = out = 0;
	max  = INT_MAX;         /* max int, for normalization */
	MPI_Send(&request, 1, MPI_INT, server, REQUEST, world);
	MPI_Comm_rank(workers, &workerid);
	iter = 0;
	while (!done) {
	    iter++;
	    request = 1;
	    MPI_Recv(rands, CHUNKSIZE, MPI_INT, server, REPLY,
		     world, MPI_STATUS_IGNORE);
	    for (i=0; i<CHUNKSIZE; ) {
		x = (((double) rands[i++])/max) * 2 - 1;
		y = (((double) rands[i++])/max) * 2 - 1;
		if (x*x + y*y < 1.0)
		    in++;
		else
		    out++;
	    }
	    MPI_Allreduce(&in, &totalin, 1, MPI_INT, MPI_SUM,
			  workers);
	    MPI_Allreduce(&out, &totalout, 1, MPI_INT, MPI_SUM,
			  workers);
	    Pi = (4.0*totalin)/(totalin + totalout);
	    error = fabs( Pi-3.141592653589793238462643);
	    done = (error < epsilon || (totalin+totalout) > 100000000);
	    request = (done) ? 0 : 1;
	    if (workerid == 0) {
		  printf( "\rpi = %23.20f", Pi );
		  MPI_Send(&request, 1, MPI_INT, server, REQUEST,
			 world);
	    }
	    else {
		if (request)
		    MPI_Send(&request, 1, MPI_INT, server, REQUEST,
			     world);
	    }
	}
	*stotalin = totalin;
	*stotalout = totalout;
	MPI_Comm_free(&workers);
}
int main(int argc, char *argv[])
{
    int iter,color;
    int ranks[1];
    double  epsilon;
    int numprocs, myid, server, totalin, totalout;
    MPI_Comm world, workers;
    MPI_Group world_group, worker_group;

    MPI_Init(&argc, &argv);
    world = MPI_COMM_WORLD;
    MPI_Comm_size(world, &numprocs);
    MPI_Comm_rank(world, &myid);
    server = numprocs-1;	/* last proc is server */
    if (myid == 0) {
	if (argc < 2) {
	    fprintf(stderr, "Usage: %s epsilon\n", argv[0] );
	    MPI_Abort(MPI_COMM_WORLD, 1);
	}
        sscanf( argv[1], "%lf", &epsilon );
    }
    MPI_Bcast(&epsilon, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
 /* opcion 1
    MPI_Comm_group(world, &world_group);
    ranks[0] = server;
    MPI_Group_excl(world_group, 1, ranks, &worker_group);
    MPI_Comm_create(world, worker_group, &workers);
    MPI_Group_free(&worker_group);
*/
// opcion 2
    color = (myid==server);
    MPI_Comm_split(world,color,0,&workers);
//
    if (myid == server) {	/* I am the rand server */
        RandServer(world);
    }
    else {			/* I am a worker process */
     Worker(server,world,workers,epsilon, &totalin, &totalout);
    }

    if (myid == 0) {
        printf( "\npoints: %d\nin: %d, out: %d, <ret> to exit\n",
		totalin+totalout, totalin, totalout );
	getchar();
    }
    MPI_Finalize();
    return 0;
}
