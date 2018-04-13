/********************************
 *				*
 *** Name: Nicholas Allaire	*
 *** Date: 1/19/2018		*
 *** Email: nallaire@ucsd.edu	*
 *** SID: A10639753		*
 *				*
 *******************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include "svalidate.h"

int main(int argc, char *argv[]) {
	
	int nextProc;		// ID of next process in ring
	int nproc;		// number of processes (command line input)
	int my_id;		// ID of each proccess
	int procid;		// proccess ID to start in the ring (command line input)
	float seed;		// seed value to start computation (command line input)
	int startingProc;	// process ID of starting process
	int passingProc;	// process ID of next process in loop during message passing
	float sum;		// sum after each computation
	MPI_Status status;	// MPI status

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);

	// Verify user supplied 3 inputs
	// Else display error and return
	if (argc != 3) {
		if( my_id == 0) {
			fprintf(stderr, "usage: mpi-ring2 <procid> <seedval>\n");
			return 9;
		} else {
			MPI_Recv(&nextProc, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		}
	}

	// Verify procid can be converted to an int
	// Else display error and return
	if (isInteger(trim(argv[1]))) {
		procid = atoi(trim(argv[1]));
	} else {
		if (my_id == 0) {
			fprintf(stderr, "usage: mpi-ring2 <procid> <seedval>\n");
			return 10;
		}
	}
	
	// Verify seed can be converted to a float
	// Else display error and return
	if (isFloat(trim(argv[2]))) {
		seed = atof(trim(argv[2]));
		sum = seed;
	} else {
		if (my_id == 0) {
			fprintf(stderr, "usage: mpi-ring2 <procid> <seedval>\n");
			return 11;
		}
	}

	// Verify seed value is positive
	// Else display error and return
	if (seed < 0) {
		if (my_id == 0) {
			fprintf(stderr, "usage: mpi-ring2 <procid> <seedval>\n");
			return 12;
		}
	}	
	
	// Positive procid, perform message passing clockwise
	if (procid >= 0) {

		// Only 1 process, exit without computation
		if (nproc == 1) {
			MPI_Finalize();
                	return 0;
        	}

		// Calculate starting process if greater than nproc
		if (procid >= nproc) {
			startingProc = procid - nproc;
			while (startingProc >= nproc) {
				startingProc = startingProc - nproc;
			}
		} else {
			startingProc = procid;
		}

		// Calculate next process from starting process
		if ((startingProc + 1) == nproc) {
			nextProc = 0;
		} else {
			nextProc = startingProc + 1;
		}
	
		// Print first message if process ID is 0
		if (my_id == 0) {
                        printf("start %d seed %.6f nproc %d\n", startingProc, seed, nproc);
                }

		// Start calculation on process after starting process, else pause all other processes 
		// until they have received data from the process ID behind them
		if (my_id == ((startingProc + 1) % nproc)) {

			// Perform calculation (sum + my_id)^1.2, where sum is initially the seed value
			sum = sum + nextProc;
			sum = powf(sum, 1.2);

			// Calculate next process
			if ((nextProc + 1) == nproc) {
				nextProc = 0;
			} else {
				nextProc = nextProc + 1;
			}

			// Send calculation to the next process
			MPI_Send(&sum, 1, MPI_FLOAT, nextProc, 0, MPI_COMM_WORLD);
			MPI_Send(&nextProc, 1, MPI_INT, nextProc, 1, MPI_COMM_WORLD);

		} else {

			// Receive calculation from previous process
			MPI_Recv(&sum, 1, MPI_FLOAT, (nproc + my_id - 1) % nproc, 0, MPI_COMM_WORLD, &status);
			MPI_Recv(&passingProc, 1, MPI_INT, (nproc + my_id - 1) % nproc, 1, MPI_COMM_WORLD, &status);
			
			// Perform calculation (sum + my_id)^1.2
			sum = sum + passingProc;
			sum = powf(sum, 1.2);
			
			// If passing process is the same as starting process then full loop has occuried and pass sum to process ID 0
			// else pass info to next process	
			if (passingProc == startingProc) {
				MPI_Send(&sum, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
			} else {

				// Calculate next process
				if ((passingProc + 1) == nproc) {
                                	passingProc = 0;
                        	} else {
                                	passingProc = passingProc + 1;
                       	 	}

				// Send calculation to the next process
				MPI_Send(&sum, 1, MPI_FLOAT, passingProc, 0, MPI_COMM_WORLD);
                        	MPI_Send(&passingProc, 1, MPI_INT, passingProc, 1, MPI_COMM_WORLD);
			}
		}

	// Negative procid, perform message passing counter clockwise
	} else {

                // Only 1 process, exit without computation
                if (nproc == 1) {
                        MPI_Finalize();
                	return 0;
                }

		// Calculate starting process if less than nproc
		startingProc = nproc + procid;
		while (startingProc < 0) {
			startingProc = nproc + startingProc;
		}
		
		// Calculate next process from starting process
		if ((startingProc - 1) == -1) {
			nextProc = nproc - 1;
		} else { 
			nextProc = startingProc - 1;
		}
		
		// Print first message if process ID is 0
		if (my_id == 0) {
			printf("start %d seed %.6f nproc %d\n", startingProc, seed, nproc);
		}

		// Start calculation on process after startingProc, else pause all other processes
		// until they have received the data from the process ID in front of them
		if (my_id == nextProc) {

			// Perform calculation (sum + my_id)^1.2, where sum is initially the seed value
			sum = sum + nextProc;
			sum = powf(sum, 1.2);

			// Calculate next process
			if ((nextProc - 1) == -1) {
				nextProc = nproc - 1;
			} else {
				nextProc = nextProc - 1;
			}

			// Send calculation to the next process
			MPI_Send(&sum, 1, MPI_FLOAT, nextProc, 0, MPI_COMM_WORLD);
			MPI_Send(&nextProc, 1, MPI_INT, nextProc, 1, MPI_COMM_WORLD);

		} else {

			// Receive calculation from previous process
			MPI_Recv(&sum, 1, MPI_FLOAT, (nproc + my_id + 1) % nproc, 0, MPI_COMM_WORLD, &status);
			MPI_Recv(&passingProc, 1, MPI_INT, (nproc + my_id + 1) % nproc, 1, MPI_COMM_WORLD, &status);

			// Perform calculation (sum + my_id)^1.2
			sum = sum + passingProc;
			sum = powf(sum, 1.2);
			
			// If passing process is the same as starting process then full loop has occuried and pass sum to process ID 0
			// else pass info to next process
			if (passingProc == startingProc) {
				MPI_Send(&sum, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
			} else {

				// Calculate next process
				if ((passingProc - 1) == -1) {
					passingProc = nproc - 1;
				} else {
					passingProc = passingProc - 1;
				}

				// Send calculation to the next process
				MPI_Send(&sum, 1, MPI_FLOAT, passingProc, 0, MPI_COMM_WORLD);
				MPI_Send(&passingProc, 1, MPI_INT, passingProc, 1, MPI_COMM_WORLD);
			}
		}	
	}
	
	// Print result
	if (my_id == 0) {
		MPI_Recv(&sum, 1, MPI_FLOAT, startingProc, 0, MPI_COMM_WORLD, &status);
		printf("%.6f\n", sum);
	}

	MPI_Finalize();
	return 0;
}
