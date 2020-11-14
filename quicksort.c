/// file: file_quicksort.c
/// description: TODO
/// @author Jesse Burdick-Pless jb4411

#include <stdio.h>
#include <time.h>
#include <getopt.h>
#include <assert.h>
#include <stdlib.h>

int *quicksort( size_t size, const int *data ) {

}

void *quicksort_threaded( void *args ) {

}

int main( int argc, char **argv ) {
	int opt;
	int print = 0;
	while ( (opt = getopt( argc, argv, "p") ) != -1 ) {
                switch( opt ) {
			case 'p':
				print = 1;
				break;
			case '?':
				exit( EXIT_FAILURE );
				break;
			default:
				exit( EXIT_FAILURE );

		}
	}
	if( argc < 2 || (print && (argc < 3)) ) {
		fprintf( stderr, "error: missing command line arguments.\n" );
		exit( EXIT_FAILURE );
	}

	// open integer file
	FILE *fp;
	fp = fopen(argv[1 + print], "r");
	if( fp == NULL ) {
		fprintf( stderr, "error: could not open integer file\n" );
                exit( EXIT_FAILURE );
	}
	
	if( print ) {
		printf("Unsorted list before non-threaded quicksort: ");
	}

	// read integer file
	size_t size = 0;
	size_t capacity = 10;
	int num = 0;
	int *data = malloc(sizeof(int) * 10);
	assert(data != NULL);
	while( fread(&num, sizeof(int), 1, fp) ) {
		// print the unsorted values if -p
		if( print ) {
			printf("%d, ", num);
		}
		data[size] = num;
		size++;
		if( size == capacity ) {
			data = realloc(data, capacity * 2);
			assert(data != NULL);
			capacity *= 2;			
		}
	}
	if( print ) {
                printf("\n");
	}
	fclose(fp);
	fp = NULL;
	
	// sort (non-threaded) and print timing
	int *sorted;
	clock_t start, end;
	start = clock();
	sorted = quicksort( size, data );
	end = clock();
	double total_time = (double) (end - start) / CLOCKS_PER_SECOND;
	printf("Non-threaded time: %f\n", total_time);

	// print the sorted values if -p
	if( print ) {
		printf("Resulting list: ");
		size_t i = 0;	
		while( i < size ) {
			printf("%d, ", sorted[i]);
		}
		printf("\n");
	}
	
	// print the unsorted values if -p
	if( print ) {
                printf("Unsorted list before threaded quicksort: ");
		size_t i = 0;
                while( i < size ) {
                        printf("%d, ", data[i]);
                }
		printf("\n");
	}

	// sort (threaded) and print the timing
	
}

















