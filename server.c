#include "cs537.h"
#include "request.h"
#include "stdio.h"
#include <pthread.h>

// 
// server.c: A very, very simple web server
//
// To run:
//  server <portnum (above 2000)>
//
// Repeatedly handles HTTP requests sent to this port number.
// Most of the work is done within routines written in request.c
//

// CS537: Parse the new arguments too
void getargs(int *port, int *threads, int *buffers, int argc, char *argv[])
{
    if (argc != 4) {
	fprintf(stderr, "Usage: %s <port>\n", argv[0]);
	exit(1);
    }
    *port = atoi(argv[1]);
    *threads = atoi(argv[2]);
    *buffers = atoi(argv[3]);
}

int* buffer;
int buffersize;
int fill = 0;
int use = 0;
int count = 0;

void put(int value) {
    buffer[fill] = value;
    fill = (fill + 1) % buffersize;
    count++;
}

int get() {
    int tmp = buffer[use];
    use = (use + 1) % buffersize;
    count--;
    return tmp;
}

pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t full = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *producer(int connfd) {
    pthread_mutex_lock(&mutex); // p1
    while (count == buffersize) // p2
        pthread_cond_wait(&empty, &mutex); // p3
    put(connfd); // p4
    pthread_cond_signal(&full); // p5
    pthread_mutex_unlock(&mutex); // p
    return 0;
}

void *consumer(void *arg) {
    while (1) {
        pthread_mutex_lock(&mutex); // c1
        while (count == 0) // c2
            pthread_cond_wait(&full, &mutex); // c3

        int connfd = get(); // c4
        requestHandle(connfd);

        pthread_cond_signal(&empty); // c5
        pthread_mutex_unlock(&mutex); // c6
        Close(connfd);
    }
}


int main(int argc, char *argv[])
{
    int listenfd, connfd, port, clientlen, threads, buffers;
    struct sockaddr_in clientaddr;

    getargs(&port, &threads, &buffers, argc, argv);

    buffer = (int*) malloc(sizeof(int) * buffers); 
    buffersize = buffers;

    // 
    // CS537: Create some threads...
    //
    pthread_t p[threads];
    int i;
    for (i = 0; i < threads; ++i)
    {
        pthread_create(&p[i], NULL, consumer, NULL);
    }

    listenfd = Open_listenfd(port);
    while (1) {
    	clientlen = sizeof(clientaddr);
    	connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);
        producer(connfd);

    	// 
    	// CS537: In general, don't handle the request in the main thread.
    	// Save the relevant info in a buffer and have one of the worker threads 
    	// do the work.
    	// 

    }

}


    


 
