#include <unistd.h> //int nice(int incr); -- add value of incr to the nice value of the calling process
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <pthread.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <sys/mman.h>

#define ANSI_COLOR_GRAY    "\x1b[30m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_WHITE   "\x1b[37m"

#define ANSI_COLOR_RESET   "\x1b[0m"

#define TERM_CLEAR() printf("\033[H\033[J")
#define TERM_GOTOXY(x,y) printf("\033[%d;%dH", (y), (x))

typedef struct _thread_data_t {
    int localTid;
    const int *data;
    int numVals;
    pthread_mutex_t *lock;
    long long int *totalSum;
} thread_data_t;

int main(int argc, char* argv[]) //command: ./threaded_sum.c file_path number_of_threads
{
    TERM_CLEAR();

    //Process Arguements
    if (argc != 2) //Not the correct number of cl args
    {
        printf ("The incorrect number of arguments were provided, expected 2 but %d were given.\n", argc);
        return -1;
    }

    const int THREAD_ARRAY_SIZE = 2000000;
    int numbers[THREAD_ARRAY_SIZE];
    long long int totalSum = 0;
    int threadsRequested;

    threadsRequested = atoi(argv[1]);
    if (threadsRequested =< 0)
    {
        printf ("The number of threads requested must be more than zero, you input: %d\n", threadsRequested);
        return -1;
    }

    //Initialize Threads
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_t* threads = malloc(sizeof(pthread_t) * threadsReq);
    thread_data_t* mThreads = malloc(sizeof(thread_data_t) * threadsReq);

    int currentThreadInd = 0;
  
    for (int i = 0; i < threadsReq; i++)
    {
        int extra = (i < threadsRemaining) ? 1 : 0;
        int endThreadInd = currentThreadInd + threadsSplit + extra;
        
        mThreads[i].localTid = i;
        mThreads[i].data = thread[i - 1]; //Previously allocated thread
        mThreads[i].numVals = THREAD_ARRAY_SIZE;
        mThreads[i].lock = &mutex;
        mThreads[i].totalSum = &totalSum;

        pthread_create(&threads[i], NULL, arraySum, &mThreads[i]);
    }

    //Join and process threads
    for (int i = 0; i < threadsReq; i++)
    {
        pthread_join(threads[i], NULL);
    }

    //Memory cleanup
    free(threads);
    free(mThreads);
    pthread_mutex_destroy(&mutex);

    return 0;
}

void* arraySum(void* threadData)
{
    thread_data_t* tData = (thread_data_t*)threadData; //Reinterpret the data to adhere by the pthread API

    while (1)
    {
        long long int tempThreadSum = 0;
        long long latencyMax;

        for (int i = 0; i < tData->numVals; i++) //Calculate the time to process local sum -- Busy work
        {
            struct timespec start, end;
            clock_gettime(CLOCK_MONOTONIC, &start);

            tempThreadSum += tData->data[i];

            clock_gettime(CLOCK_MONOTONIC, &end);
            
            //Calculate latency
            long long secDiff = (long long)(timespec.end->tv_sec) - (long long)(timespec.start->tv_sec);
            long long nsecDiff = (long long)(timespec.end->tv_nsec) - (long long)(timespec.start->tv_nsec);
            long long currentLatency = secDiff * 1000000000LL + nsecDiff;

            if (currentLatency > latencyMax) { latencyMax = currentLatency; }
        }

        //Add local sum to the total sum ensuring thread saftey
        pthread_mutex_lock(tData->lock); //Lock and unlock the Critical Section
        *(tData->totalSum) += tempThreadSum; //Update total sum in thread_data_t
        pthread_mutex_unlock(tData->lock);

        print_progress(tData->localTid, latencyMax);
    }
    return NULL;
}

void print_progress(pid_t localTid, size_t value) {
        pid_t tid = syscall(__NR_gettid);

        TERM_GOTOXY(0,localTid+1);

	char prefix[256];
        size_t bound = 100;
        sprintf(prefix, "%d: %ld (ns) \t[", tid, value);
	const char suffix[] = "]";
	const size_t prefix_length = strlen(prefix);
	const size_t suffix_length = sizeof(suffix) - 1;
	char *buffer = calloc(bound + prefix_length + suffix_length + 1, 1);
	size_t i = 0;

	strcpy(buffer, prefix);
	for (; i < bound; ++i)
	{
	    buffer[prefix_length + i] = i < value/10000 ? '#' : ' ';
	}
	strcpy(&buffer[prefix_length + i], suffix);
        
        if (!(localTid % 7)) 
            printf(ANSI_COLOR_WHITE "\b%c[2K\r%s\n" ANSI_COLOR_RESET, 27, buffer);  
        else if (!(localTid % 6)) 
            printf(ANSI_COLOR_BLUE "\b%c[2K\r%s\n" ANSI_COLOR_RESET, 27, buffer);  
        else if (!(localTid % 5)) 
            printf(ANSI_COLOR_RED "\b%c[2K\r%s\n" ANSI_COLOR_RESET, 27, buffer);  
        else if (!(localTid % 4)) 
            printf(ANSI_COLOR_GREEN "\b%c[2K\r%s\n" ANSI_COLOR_RESET, 27, buffer);  
        else if (!(localTid % 3)) 
            printf(ANSI_COLOR_CYAN "\b%c[2K\r%s\n" ANSI_COLOR_RESET, 27, buffer);  
        else if (!(localTid % 2)) 
            printf(ANSI_COLOR_YELLOW "\b%c[2K\r%s\n" ANSI_COLOR_RESET, 27, buffer);  
        else if (!(localTid % 1)) 
            printf(ANSI_COLOR_MAGENTA "\b%c[2K\r%s\n" ANSI_COLOR_RESET, 27, buffer);  
        else
            printf("\b%c[2K\r%s\n", 27, buffer);

	fflush(stdout);
	free(buffer);
}