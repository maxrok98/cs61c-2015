#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

/* The naive transpose function as a reference. */
void transpose_naive(int n, int blocksize, int *dst, int *src) {
    for (int x = 0; x < n; x++) {
        for (int y = 0; y < n; y++) {
            dst[y + x * n] = src[x + y * n];
        }
    } 
}

/* Implement cache blocking below. You should NOT assume that n is a
 * multiple of the block size. */
void transpose_blocking(int n, int blocksize, int *dst, int *src) {
    // YOUR CODE HERE
    int block_x = 0, block_y = 0;
	int blocksize_x = blocksize, blocksize_y = blocksize; // shoud be used if n % blocksize != 0

    for(block_x = 0; block_x < n; block_x+=blocksize){
		//if(n - block_x < blocksize) blocksize_x = n - block_x;
		blocksize_x = (n - block_x)*(n - block_x < blocksize) + blocksize*(n - block_x >= blocksize); // branchless technique
    	for(block_y = 0; block_y < n; block_y+=blocksize){
			//if(n - block_y < blocksize) blocksize_y = n - block_y;
			blocksize_y = (n - block_y)*(n - block_y < blocksize) + blocksize*(n - block_y >= blocksize);
			for (int x = block_x; x < block_x + blocksize_x; x++) {
				for (int y = block_y; y < block_y + blocksize_y; y++) {
					dst[y + x * n] = src[x + y * n];
				}
			} 
		}
		//blocksize_y = blocksize;
	}
} 

void benchmark(int *A, int *B, int n, int blocksize, 
    void (*transpose)(int, int, int*, int*), char *description) {
 
    int i, j;
    printf("Testing %s: ", description);

    /* initialize A,B to random integers */
    srand48( time( NULL ) );
    for( i = 0; i < n*n; i++ ) A[i] = lrand48( );
    for( i = 0; i < n*n; i++ ) B[i] = lrand48( );

    /* measure performance */
    struct timeval start, end;

    gettimeofday( &start, NULL );
    transpose( n, blocksize, B, A );
    gettimeofday( &end, NULL );

    double seconds = (end.tv_sec - start.tv_sec) +
        1.0e-6 * (end.tv_usec - start.tv_usec);
    printf( "%g milliseconds\n", seconds*1e3 );


    /* check correctness */
    for( i = 0; i < n; i++ ) {
        for( j = 0; j < n; j++ ) {
            if( B[j+i*n] != A[i+j*n] ) {
                printf("Error!!!! Transpose does not result in correct answer!!\n");
                exit( -1 );
            }
        }
    }
}

int main( int argc, char **argv ) {
    if (argc != 3) {
        printf("Usage: transpose <n> <blocksize>\nExiting.\n");
        exit(1);
    }

    int n = atoi(argv[1]);
    int blocksize = atoi(argv[2]);

    /* allocate an n*n block of integers for the matrices */
    int *A = (int*)malloc( n*n*sizeof(int) );
    int *B = (int*)malloc( n*n*sizeof(int) );

    /* run tests */
    benchmark(A, B, n, blocksize, transpose_naive, "naive transpose");
    benchmark(A, B, n, blocksize, transpose_blocking, "transpose with blocking");

    /* release resources */
    free( A );
    free( B );
    return 0;
}

