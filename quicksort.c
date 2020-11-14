/// file: file_quicksort.c
/// description: TODO
/// @author Jesse Burdick-Pless jb4411

#include <string.h>
#include <stdio.h>
#include <time.h>
#include <getopt.h>
#include <assert.h>
#include <stdlib.h>

int **partition( int pivot, size_t size, const int *data ) {
	int less_size = 0, same_size = 0, more_size = 0;
	int *less = malloc(sizeof(int) * size);
	int *same = malloc(sizeof(int) * size);
	int *more = malloc(sizeof(int) * size);

	size_t i = 0;
	int num = 0;
	while( i < size ) {
		num = data[size];
		if( num < pivot ) {
			less[less_size] = num;
			less_size++;
		} else if( num > pivot ) {
			more[more_size] = num;
			more_size++;
		} else {
			same[same_size] = num;
                        same_size++;
		}
	}
	int **parts = malloc(sizeof(int *) * 4);
	parts[0] = less;
	parts[1] = same;
	parts[2] = more;
	int sizes[3] = {less_size, same_size, more_size};
	parts[3] = sizes;

	return parts;
}

int *quicksort( size_t size, const int *data ) {
	if( size == 0 ) {
		return NULL;
	} else {
		int pivot = data[0];
		int **parts = partition(pivot, size, data);
		int less_size = parts[3][0];
		int same_size = parts[3][1];
		int more_size = parts[3][2];
		// sort less
		int *less = parts[0];
		less = quicksort(less_size, less);
		
		int *same = parts[1];
		
		// sort more
		int *more = parts[2];
		more = quicksort(more_size, more);
		
		// combine less + same + more into result
		int *result = malloc(sizeof(int) * size);
		assert(result != NULL);
		memcpy(result, less, sizeof(int) * less_size);
		memcpy(result + less_size, same, sizeof(int) * same_size);
		memcpy(result + less_size + same_size, more, sizeof(int) * more_size);
		
		// free memory that is no longer needed
		for( int i = 0; i < 3; ++i ) {
			if( parts[i] ) {
				free(parts[i]);
			}
		}
		free(less);
		free(same);
		free(more);

		return result;
	}	
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
	int *data = malloc(sizeof(int) * 10);
	assert(data != NULL);
	while( fscanf(fp, "%d", &data[size]) != EOF ) {
		// print the unsorted values if -p
		if( print ) {
			printf("%d, ", data[size]);
		}
		size++;
		if( size == capacity ) {
			data = realloc(data, capacity * 2);
			assert(data != NULL);
			capacity *= 2;			
		}
		if(size > 10) {
			break;
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
	double total_time = (double) (end - start) / CLOCKS_PER_SEC;
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

















