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
#include <errno.h>


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
	int *data = (int *) args;
	if( data[0] == 0) {
                pthread_exit(NULL);
        } else {
		pthread_t threads[2];
		int rc = 0;
		
		int size = data[0];
		data = &data[1];
                int pivot = data[0]; 
		int **parts = partition(pivot, (size_t) size, data);
                int less_size = parts[3][0];
                int same_size = parts[3][1];
                int more_size = parts[3][2];
                
		// sort less
		int *old_less = parts[0];
		int *less_args = calloc(less_size + 1, sizeof(int));
		less_args[0] = less_size;
		memcpy(less_args + 1, old_less, sizeof(int) * less_size);
		rc = pthread_create( &threads[0], NULL, quicksort_threaded, (void *) less_args );
		if( rc ) {
			printf( "ERROR; pthread_create() returned %d\n", rc );
			exit( EXIT_FAILURE );
		}

		int *same = parts[1];

		// sort more
		int *old_more = parts[2];
                int *more_args = calloc(more_size + 1, sizeof(int));
                more_args[0] = more_size;
                memcpy(more_args + 1, old_more, sizeof(int) * more_size);
                rc = pthread_create( &threads[1], NULL, quicksort_threaded, (void *) more_args );
                if( rc ) {
                        printf( "ERROR; pthread_create() returned %d\n", rc );
                        exit( EXIT_FAILURE );
                }

		// join less and more threads
		void *retval = NULL;
		pthread_join(threads[0], &retval); 
		int *less_rt = (int *) retval;
		int *less = &less_rt[1];
		pthread_join(threads[1], &retval);
		int *more_rt = (int *) retval;
		int *more = &more_rt[1];

		// update num_spawned
                int lock = pthread_mutex_lock(&sharedLock);
                if( lock ) {
                        perror( "locking to update num_spawned" );
                        exit( errno );
                }
                // +1 for the thread for less, +1 for the thread for more
                num_spawned += 2;
                lock = pthread_mutex_unlock(&sharedLock);
                if( lock ) {
                        perror( "unlocking after updating num_spawned" );
                        exit( errno );
                }

		// free old_less and old_more
		if( old_less != NULL ) {
			free(old_less);
		}
		if( old_more != NULL ) {
			free(old_more);
		}
		// free less_args and more_args
		free(less_args);
		free(more_args);


		// combine less + same + more into result
		int *result = calloc(size + 1, sizeof(int));
		assert(result != NULL);
		result[0] = size;
		if( less != NULL ) {
			memcpy(result + 1, less, sizeof(int) * less_size);
		}
		if( same != NULL ) {
			memcpy(result + 1 + less_size, same, sizeof(int) * same_size);
		}
		if( more != NULL ) {
			memcpy(result + 1 + less_size + same_size, more, sizeof(int) * more_size);
		}

		// free memory that is no longer needed
		less = NULL;
		more = NULL;
		data = NULL;
		free(less_rt);
		free(same);
		free(more_rt);
		free(parts[3]);
		free(parts);

		pthread_exit((void *) result);
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
	sorted = NULL;

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
	int rc = pthread_mutex_init( &sharedLock, NULL );
	if( rc ) {
		printf( "ERROR; pthread_mutex_init() returned %d\n", rc );
		exit( EXIT_FAILURE );
	}
	pthread_t thread;
	int *args = malloc(sizeof(int) * (size + 1));
	args[0] = (int) size;
	memcpy(args + 1, data, sizeof(int) * size);	

	void *retval;
	start = clock();
	rc = pthread_create( &thread, NULL, quicksort_threaded, (void *) args );
	if( rc ) {
		printf( "ERROR; pthread_create() returned %d\n", rc );
		exit( EXIT_FAILURE );
	}
	pthread_join(thread, &retval);	
	end = clock();
	total_time = (double) (end - start) / CLOCKS_PER_SEC;
	printf("Threaded time: %f\n", total_time);
	printf("Threads spawned: %d\n", num_spawned);
	pthread_mutex_destroy(&sharedLock);

	// print the sorted values if -p
	if( print ) {
		int *tsorted = (int *) retval;
		tsorted = &tsorted[1];
		printf("Resulting list: ");
		size_t i = 0;
		while( i < size ) {
			printf("%d, ", tsorted[i]);
			i++;
		}
		printf("\n");
	}
	free(args);
	free(retval);

	free(data);
	return 0;
}
