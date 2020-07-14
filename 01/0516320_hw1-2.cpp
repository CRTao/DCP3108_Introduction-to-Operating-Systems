#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <stdlib.h>
#include <time.h>

int N; 

void getCofactor(int mat[],int temp[], int p, int q, int n) { 
    int i = 0, j = 0; 

    for (int row = 0; row < n; row++) { 
        for (int col = 0; col < n; col++) { 
            if (row != p && col != q) { 
                temp[i*N+j] = mat[row*N+col]; 
  				j++;
                if (j == n - 1) { 
                    j = 0; 
                    i++; 
                } 
            } 
        } 
    } 
} 

int determinantOfMatrix(int mat[], int n, int start, int end,int check) { 
    int D = 0;
    if (n == 1) 
        return mat[0]; 
  
    int temp[N*N];
    int sign = 1;
    if(check==0){if(start%2==0)sign=1;else sign=-1;}
    check = 1;
    for (int f = start; f < end; f++) { 
        getCofactor(mat, temp, 0, f, n);
        D += sign * mat[f] * determinantOfMatrix( temp, n-1, 0, n-1, check); 
        sign = -sign; 
    } 
    return D; 
} 
  
void display(unsigned int mat[], int row, int col) { 
    for (int i = 0; i < row; i++) { 
        for (int j = 0; j < col; j++) 
            printf(" %u", mat[i*N+j]); 
        printf("\n"); 
    } 
} 
  
int main(int argc, char *argv[])
{ 
	unsigned int mat[N*N];
    int tmp;

    int shmid ;
    int *shmaddr ;
    struct shmid_ds buf ;
    
    int flag = 0 ;
    int pid ;
	int N1,N2p1,N2p2,N3p1,N3p2,N3p3,N4p1,N4p2,N4p3,N4p4;
	int sum1, sum2, sum3, sum4;

	FILE *fid;   
	fid = fopen(argv[1],"r");
	fscanf(fid,"%d",&N);
    
        N1 = N;
	switch(N%2){
        case 0:
            N2p1 = N2p2 = N/2;
            break;
        case 1:
            N2p1 = N/2 + 1;
            N2p2 = N/2;
            break;
    	}
	switch(N%3){
        case 0:
            N3p1 = N3p2 = N3p3 = N/3;
            break;
        case 1:
            N3p1 = N/3 + 1;
            N3p2 = N3p3 = N/3;
            break;
        case 2:
            N3p1 = N3p2 = N/3 + 1;
            N3p3 = N/3;
            break;
    	}
	switch(N%4){
        case 0:
            N4p1 = N4p2 = N4p3 = N4p4= N/4;
            break;
        case 1:
            N4p1 = N/4 + 1;
            N4p2 = N4p3 = N4p4 = N/4;
            break;
        case 2:
            N4p1 = N4p3 = N/4 + 1;
            N4p2 = N4p4 = N/4;
            break;
		case 3:
            N4p1 = N3p2 = N4p3 = N/4 + 1;
            N4p4 = N/4;
            break;
    	}
	int SIZE = N*N*4+4+8+12+16;
	shmid = shmget(IPC_PRIVATE, SIZE, IPC_CREAT|0600 ) ;
    if ( shmid < 0 ){perror("shmget ipc_id Error") ;  return -1 ; }
    pid = fork() ;
    if (pid == 0)  //£á¡Ò£g{¡±C >> Ax?s¡Âx¢X} 
    {
        shmaddr=(int*)shmat(shmid,NULL,0);
        if ((long)shmaddr==-1){   perror("shmat addr error") ;  return -1 ; }
		unsigned int r,c;
		for (r = 0; r < N; r++) { 
        	for (c = 0; c < N; c++) 
            	fscanf(fid,"%u",shmaddr+r*N+c);
    	} 
		shmdt( shmaddr ) ;
        return  0;
    } 
	else if ( pid > 0) { //£ál£g{?! >> 
        sleep(3) ;
		struct timeval start, end;
        
		flag = shmctl( shmid, IPC_STAT, &buf) ;
        if ( flag == -1 ){   perror("shmctl shm error") ;  return -1 ; }
        
		shmaddr = (int*) shmat(shmid, NULL, 0 ) ;
        if ( (long)shmaddr == -1 ){   perror("shmat addr error") ;  return -1 ; }
        
		//-------------------------  1 Process --------------------------------
		gettimeofday(&start,0);
		*(shmaddr+N*N+1) = determinantOfMatrix(shmaddr, N, 0, N, 0);
		sum1 = *(shmaddr+N*N+1); 
		gettimeofday(&end,0);
		printf("Calculating determinant using 1 process\n");
		int sec = end.tv_sec-start.tv_sec;
		int usec = end.tv_usec-start.tv_usec;
		printf("Elapsed time: %f sec, ",(sec+usec/1000000.0));
		printf("determinant : %d \n",sum1);
		//---------------------------------------------------------------------
		//-------------------------  2 Process --------------------------------
		gettimeofday(&start,0);
		pid_t pid21;
		pid21 = fork();
		if (pid21 < 0) {fprintf(stderr, "Fork Failed");return 1;}
		else if (pid21 == 0){
			//Part 1
			*(shmaddr+N*N+2) = determinantOfMatrix(shmaddr, N, 0, N2p1, 0);	
			return 0;
		}
		else {
			//Part 2
			*(shmaddr+N*N+3) = determinantOfMatrix(shmaddr, N, N2p1, N2p1+N2p2, 0);
			wait(NULL);
		}	
		gettimeofday(&end,0);
		sleep(1);
		sum2 = *(shmaddr+N*N+2)+*(shmaddr+N*N+3);
		printf("Calculating determinant using 2 processes\n");
		sec = end.tv_sec-start.tv_sec;
		usec = end.tv_usec-start.tv_usec;
		printf("Elapsed time: %f sec, ",(sec+usec/1000000.0));
		printf("determinant : %d \n",sum2);
		//---------------------------------------------------------------------
		//-------------------------  3 Process --------------------------------
		gettimeofday(&start,0);
		pid_t pid31,pid32;
		pid31 = fork();
		if (pid31 < 0) {fprintf(stderr, "Fork Failed");	return 1;}
		else if (pid31 == 0){
			pid32 = fork();
			if (pid32 < 0) {fprintf(stderr, "Fork Failed");return 1;}
			else if (pid32 == 0){
					//Part 1
					*(shmaddr+N*N+4) = determinantOfMatrix(shmaddr, N, 0, N3p1, 0);	
				return 0;
			}
			else {
					//Part 2
					*(shmaddr+N*N+5) = determinantOfMatrix(shmaddr, N, N3p1, N3p1+N3p2, 0);
					wait(NULL);
			}
			return 0;
		}
		else {
			//Part 3
			*(shmaddr+N*N+6) = determinantOfMatrix(shmaddr, N, N3p1+N3p2, N3p1+N3p2+N3p3, 0);
			wait(NULL);
		}
		gettimeofday(&end,0);
		sleep(1);
		sum3 = *(shmaddr+N*N+4)+*(shmaddr+N*N+5)+*(shmaddr+N*N+6);
		printf("Calculating determinant using 3 processes\n");
		sec = end.tv_sec-start.tv_sec;
		usec = end.tv_usec-start.tv_usec;
		printf("Elapsed time: %f sec, ",(sec+usec/1000000.0));
		printf("determinant : %d \n",sum3);
		//---------------------------------------------------------------------
		//-------------------------  4 Process --------------------------------
		gettimeofday(&start,0);
		pid_t pid41,pid42,pid43;
		pid41 = fork();
		if (pid41 < 0) {fprintf(stderr, "Fork Failed");	return 1;}
		else if (pid41 == 0){
			pid42 = fork();
			if (pid42 < 0) {fprintf(stderr, "Fork Failed");	return 1;}
			else if (pid42 == 0){
				pid43 = fork();
				if (pid43 < 0) {fprintf(stderr, "Fork Failed");return 1;}
				else if (pid43 == 0){
						//Part 1
						*(shmaddr+N*N+7) = determinantOfMatrix(shmaddr, N, 0, N4p1, 0);	
					return 0;
				}
				else {
						//Part 2
						*(shmaddr+N*N+8) = determinantOfMatrix(shmaddr, N, N4p1, N4p1+N4p2, 0);
						wait(NULL);
				}
				return 0;
			}
			else {
				//Part 3
				*(shmaddr+N*N+9) = determinantOfMatrix(shmaddr, N, N4p1+N4p2, N4p1+N4p2+N4p3, 0);
				wait(NULL);
			}
			return 0;
		}
		else {
			//Part 4
			*(shmaddr+N*N+10) = determinantOfMatrix(shmaddr, N, N4p1+N4p2+N4p3, N4p1+N4p2+N4p3+N4p4, 0);
			wait(NULL);
		}
		gettimeofday(&end,0);
		sleep(1);
		sum4 = *(shmaddr+N*N+7)+*(shmaddr+N*N+8)+*(shmaddr+N*N+9)+*(shmaddr+N*N+10);
		printf("Calculating determinant using 4 processes\n");
		sec = end.tv_sec-start.tv_sec;
		usec = end.tv_usec-start.tv_usec;
		printf("Elapsed time: %f sec, ",(sec+usec/1000000.0));
		printf("determinant : %d \n",sum4);
		//---------------------------------------------------------------------
		//AAcnaA?! 
		wait(NULL);
		shmdt( shmaddr );
        shmctl(shmid, IPC_RMID, NULL) ;
	}
	else{ //£á¡Ò£g{?! 
		perror("fork error") ;
        shmctl(shmid, IPC_RMID, NULL) ;
    }

    return 0; 
} 


