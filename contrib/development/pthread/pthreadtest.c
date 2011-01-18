/*
    Copyright © 2007, The AROS Development Team. All rights reserved.
    $Id$
*/


#include <proto/pthread.h>
#include <stdlib.h>
#include <stdio.h>

void *thread_func1( void *vptr_args );
void *thread_func2( void *vptr_args );

int main ( void )
{
 	int i, j;

	pthread_t thread1, thread2;

	pthread_create( &thread1, NULL, &thread_func1, NULL );
	printf("thread 1 created.. aros_pthread: %p, thread: %p\n", thread1.p, &thread1);

	pthread_create( &thread2, NULL, &thread_func2, NULL );
	printf("thread 2 created.. aros_pthread: %p, thread: %p\n", thread2.p, &thread2);


	for( j= 0; j < 20; ++j )
	{
		printf("a\n");

		for( i= 99999999; i; --i );  /* use some CPU time */

		printf("trying to cancel thread 1\n");
		pthread_cancel( thread1 );	
	}


	printf("join thread 2\n");
	pthread_join( thread2, NULL );

	printf("finished!\n");

	exit( EXIT_SUCCESS );
}

void *thread_func1( void *vptr_args )
{
	int i, j;

	for( j= 0; j < 20; ++j )
	{
		printf("b\n");

		for( i= 99999999; i; --i );  /* use some CPU time */
	}
  
	printf("func1 end\n");

	// pthread_exit( NULL );
}

void *thread_func2( void *vptr_args )
{
	int i, j;

	for( j= 0; j < 20; ++j )
	{
		printf("c\n");

		for( i= 199999999; i; --i );  /* use some CPU time */
	}
  
	printf("func2 end\n");

	// pthread_exit( NULL );
}
