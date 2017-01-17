// LabMPI.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// !!!!!!!!!!!! TREBUIE APELAT CU NUMARUL DE PROCESE = NUMARUL DE VERTICES 

int main(int argc, char* argv[])
{
	// define graph
	const int numberOfVertices = 4;
	const int maxSizeNeighbours = 2;
	int adjacencyArr[] = { 1, 3, 0, 2, 1, -1, 0, -1 };

	// constante pentru size-uri
	const int adjacencyArrSize = sizeof(adjacencyArr) / sizeof(int);

	// define structuri folosite in algoritm
	int coloringResult[numberOfVertices];
	coloringResult[0] = 0;
	for (int i = 1; i < numberOfVertices; i++) {
		coloringResult[i] = -1;
	}

	int availableColors[numberOfVertices];
	for (int i = 0; i < numberOfVertices; i++) {
		availableColors[i] = 0;
	}


	int rank;
	int size;

	//Initialize the MPI
	MPI_Init(&argc, &argv);

	//Get the current rank of the process and the the total number of processes
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	printf("rank: %d\n", rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);


	//all other processes besides the first one (0) are sending their data back to process 0 (the collector)
	if (rank > 0) {
		int start = (rank) * maxSizeNeighbours;
		int end = start + maxSizeNeighbours;

		for (int j = start; j < end; j++)
		{
			if (adjacencyArr[j] != -1)
			{
				if (coloringResult[adjacencyArr[j]] != -1)
				{
					availableColors[coloringResult[adjacencyArr[j]]] = 1;
				}
			}
		}

		int c;
		for (c = 0; c < numberOfVertices; c++)
		{
			if (availableColors[c] == 0)
			{
				break;
			}
		}

		coloringResult[rank] = c;

		MPI_Send(coloringResult, _countof(coloringResult), MPI_INT, 0, 0, MPI_COMM_WORLD);
	}

	//the process 0 acts as a collector and gathers data from all the other processes
	if (rank == 0) {
		int test[numberOfVertices];
		for (int i = 1; i < size; i++) {
			MPI_Recv(test, _countof(test), MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			for (int j = 0; j < numberOfVertices; j++) {
				if (test[j] != -1) {
					coloringResult[j] = test[j];
				}
			}
		}

		printf("FINAL RESULT: \n");
		for (int i = 0; i < numberOfVertices; i++) {
			printf("%d ", coloringResult[i]);
		}
		printf("\n");
	}

	//close the process
	MPI_Finalize();

	return 0;
}
