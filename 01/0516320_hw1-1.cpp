#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/types.h> 

int status;

void  ChildProcess(void);                /* child process prototype  */
void  ParentProcess(void);               /* parent process prototype */

int main(void)
{
    pid_t pid; 
    pid = fork();
    if (pid < 0) {
		printf("Fork Error.\n");
		return 1;
	}
    else if (pid == 0) 
        ChildProcess();
	else 
        ParentProcess();
    return 0;

}

void ChildProcess(void)
{
    printf("This is Child process PID : %d.  My parent's PID = %d\n",getpid(),getppid());	
}

void ParentProcess(void)
{
    printf("This is Parent process PID : %d, waiting for my child\n", getpid());

    printf("This is Parent process,  catch my child, PID = %d\n",wait(&status));
}
