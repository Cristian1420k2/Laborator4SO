#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <ctime> 
#include <sys/shm.h>
#include <sys/stat.h>

sem_t mutex;
int aux = 1;

void* create_shared_memory(size_t size)
{
  // Our memory buffer will be readable and writable:
  int protection = PROT_READ | PROT_WRITE;

  // The buffer will be shared (meaning other processes can access it), but
  // anonymous (meaning third-party processes cannot obtain an address for it),
  // so only this process and its children will be able to use it:
  int visibility = MAP_SHARED | MAP_ANONYMOUS;

  // The remaining parameters to mmap() are not important for this use case,
  // but the manpage for mmap explains their purpose.
  return mmap(NULL, size, protection, visibility, -1, 0);
}
//the creation function for a shared memory I took it from the link
//https://stackoverflow.com/questions/5656530/how-to-use-shared-memory-with-linux-in-c
// or (int*)(mmap(NULL, 128, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0));
int main()
{   
    int *shmem = static_cast<int*> (create_shared_memory(128));
    sem_init(&mutex, 1, 1); // Initalise the semaphore
    int choice;
    srand(time(0));
    *shmem = aux;
    int pid = fork();
    
    while (*shmem <= 1000)
    {
        choice = rand() % 2 + 1;
        
        if(choice == 2 && *shmem <= 1000) // Parent process 
        { 
            //wait 
            sem_wait(&mutex); 
            
            printf("%d ", *shmem);
            *shmem =*shmem + 1;

            //signal 
            sem_post(&mutex); 
        } 
        
        if(pid == 0) // Child process
        {
            choice = rand() % 2 + 1;

            if(choice == 2 && *shmem <= 1000)
            {
                //wait 
                sem_wait(&mutex);
                
                printf("%d ", *shmem);
                *shmem =*shmem + 1;
                
                //signal 
                 sem_post(&mutex); 
            }
        }
        else
        {
            wait(NULL);
        }
    }
    printf("\n"); 
    sem_destroy(&mutex);
}
