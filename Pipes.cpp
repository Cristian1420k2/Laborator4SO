#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <iostream>
using namespace std;

// Vector that stores prime numbers in an interval
int v[300];

// Function that checks if a number is prime
bool prime(int n)
{
    int i;
    
    if (n == 0 || n == 1)
    {
        return false;
    }
    else
    {
        for (i = 2; i <= n / 2; ++i)
        {
            if (n % i == 0)
            {
                return false;
            }
        }
    }
    return true;
}

// Child process
void childProc(int readPipe, int writePipe)
{
    int i, a, b;  //a b represents the ends of the interval
    int vi = 0;
    
    // Initalize the vector with 0 for each process
    for(int j = 0; j < 300; j++)
    {
        v[j] = 0;
    }
     
    // Read from pipe the number of the process
    read(readPipe, &i, sizeof(int));
    
    // Create the interval
    if(i == 1)
    {
        a = 1;
        b = 1000;
    }   
    
    if(i >= 2)
    {
        a = (i-1) * 1000 + 1;
        b = i * 1000;        
    }
    
    // Check if the numbers form the interval are prime,
    // if they are prime put them in a vector
    for(int j = a; j <= b; j++)
	{
	    if(prime(j) == true)
		{
		    v[vi++] = j;
	    }
    }
    
    // Send the vector to the parent prcess thrugh the pipe
    write(writePipe, &v, sizeof(v));
	exit(1);
}

// Parent process
void parentProc(int readPipe, int writePipe)
{
    int val;
      
    // For each process write the number of the process through the pipe
    for(int i  = 1; i <= 10; i++)
        write(writePipe, &i, sizeof(int));
    
    // For each process, read the vector from the pipe
    // and print the prime numbers 
    for(int i = 1; i <= 10; i++)
    {
        read(readPipe, &v, sizeof(v));
        for(int j = 0; j < 300; j++)
        {
            if(v[j] != 0)
            {
                printf("%d ", v[j]);
            }
        }
    }
    wait(NULL);
}
// R: 0 - read 
//    1 - write
int main()
{
    int i;
    int firstPipe[2]; 
    int secondPipe[2];
    int pid = 1;

    // Create the 2 pipes
    if (pipe(firstPipe))
    {
        printf("Error creating the pipe!\n");
        exit(-1);
    }
	
    if (pipe(secondPipe)) 
    {
        printf("Error creating the pipe!\n");
        exit(-1);
    }
    
    // Create 10 processes
	for(i = 0; i < 10; i++)
    {
        pid = fork();
        if (pid == 0){
          
		childProc(firstPipe[0], secondPipe[1]);
        }
        else
        {
              if (pid == -1) {
                printf("Error creating child process!\n");
                exit(-1);
            }
	}
    }
	parentProc(secondPipe[0],firstPipe[1]);
}