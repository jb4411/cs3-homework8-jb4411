/// file: file_quicksort.c
/// description: TODO
/// @author Jesse Burdick-Pless jb4411

#include <string.h>
#include <stdio.h>
#include <time.h>
#include <getopt.h>
#include <assert.h>
#include <stdlib.h>
#include <pthread.h>

/// num_spawned is a variable manipulated by multiple threads.
static int num_spawned = 0;

/// sharedLock protects the variable from concurrent access.
static pthread_mutex_t sharedLock;

int **partition( int pivot, size_t size, const int *data ) {
	int less_size = 0, same_size = 0, more_size = 0;
	int *less = calloc(size, sizeof(int));
	assert(less != NULL);
	int *same = calloc(size, sizeof(int));
	assert(same != NULL);
	int *more = calloc(size, sizeof(int));
	assert(more != NULL);

	size_t i = 0;
	int num = 0;
	while( i < size ) {
		num = data[i];
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
		i++;
	}
	int **parts = calloc(4, sizeof(int *));
	assert(parts != NULL);
	parts[0] = less;
	parts[1] = same;
	parts[2] = more;
	parts[3] = calloc(3, sizeof(int));
	parts[3][0] = less_size;
	parts[3][1] = same_size;
	parts[3][2] = more_size;
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
		int *old_less = parts[0];
		int *less = quicksort(less_size, old_less);
		if( old_less != NULL ) {
			free(old_less);
		}

		int *same = parts[1];

		// sort more
		int *old_more = parts[2];
		int *more = quicksort(more_size, old_more);
		if( old_more != NULL ) {
			free(old_more);
		}

		// combine less + same + more into result
		int *result = calloc(size, sizeof(int));
		assert(result != NULL);
		if( less != NULL ) {
			memcpy(result, less, sizeof(int) * less_size);
		}
		if( same != NULL ) {
			memcpy(result + less_size, same, sizeof(int) * same_size);
		}
		if( more != NULL ) {
			memcpy(result + less_size + same_size, more, sizeof(int) * more_size);
		}

		// free memory that is no longer needed
		free(less);
		free(same);
		free(more);
		free(parts[3]);
		free(parts);

		return result;
	}	
}

void *quicksort_threaded( void *args ) {
	// args layout:  args[0] = size, args[1] = *data
	if( size == 0 ) {
                return NULL;
        } else {
		size_t size = (size_t) args[0];
		int *data = args[1]; //TODO
                int pivot = (int) data[0]; // TODO
		int **parts = partition(pivot, size, data);
                int less_size = parts[3][0];
                int same_size = parts[3][1];
                int more_size = parts[3][2];
                // sort less
                int *old_less = parts[0];
                int *less = quicksort(less_size, old_less);
                if( old_less != NULL ) {
                        free(old_less);
                }

                int *same = parts[1];

                // sort more
                int *old_more = parts[2];
                int *more = quicksort(more_size, old_more);
                if( old_more != NULL ) {
                        free(old_more);
                }

                // combine less + same + more into result
                int *result = calloc(size, sizeof(int));
                assert(result != NULL);
                if( less != NULL ) {
                        memcpy(result, less, sizeof(int) * less_size);
                }
                if( same != NULL ) {
                        memcpy(result + less_size, same, sizeof(int) * same_size);
                }
                if( more != NULL ) {
                        memcpy(result + less_size + same_size, more, sizeof(int) * more_size);
                }

                // free memory that is no longer needed
                free(less);
                free(same);
                free(more);
                free(parts[3]);
                free(parts);

                return result;
        }
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
	int num = 0;
	while( fscanf(fp, "%d", &num) != EOF ) {
		data[size] = num;
		// print the unsorted values if -p
		if( print ) {
			printf("%d, ", data[size]);
		}
		size++;
		if( size == capacity ) {
			capacity *= 2;
			data = realloc(data, capacity * sizeof(int));
			assert(data != NULL);
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
			i++;
		}
		printf("\n");
	}
	free(sorted);


	free(data);
	return 0;

	// print the unsorted values if -p
	if( print ) {
		printf("Unsorted list before threaded quicksort: ");
		size_t i = 0;
		while( i < size ) {
			printf("%d, ", data[i]);
			i++;
		}
		printf("\n");
	}

	// sort (threaded) and print the timing

}

















