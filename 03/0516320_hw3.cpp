#include<stdio.h> 
#include<stdlib.h>
#include<iostream>
#include<string.h>

// Using Linear Search. 

int FIFO_HITS[4];
int FIFO_MISS[4];
int LRU_HITS[4];
int LRU_MISS[4];
char FIFO_64[64][10];
char FIFO_128[128][10];
char FIFO_256[256][10];
char FIFO_512[512][10];
char LRU_64[64][10];
char LRU_128[128][10];
char LRU_256[256][10];
char LRU_512[512][10];

void init(){
	for(int i=0;i<4;i++){
		FIFO_HITS[i]=0;
		FIFO_MISS[i]=0;
		LRU_HITS[i]=0;
		LRU_MISS[i]=0;	
	}
	for(int i=0;i<64;i++){
		for(int j=0;j<10;j++){
			FIFO_64[i][j]='\0';
			LRU_64[i][j]='\0';
		}
	}
	for(int i=0;i<128;i++){
		for(int j=0;j<10;j++){
			FIFO_128[i][j]='\0';
			LRU_128[i][j]='\0';
		}
	}
	for(int i=0;i<256;i++){
		for(int j=0;j<10;j++){
			FIFO_256[i][j]='\0';
			LRU_256[i][j]='\0';
		}
	}
	for(int i=0;i<512;i++){
		for(int j=0;j<10;j++){
			FIFO_512[i][j]='\0';
			LRU_512[i][j]='\0';
		}
	}
}

int FIFO_search(char *addin, char cache[][10], int FRAME_SIZE){
	int location;
	int i;
	for(i = 0;i < FRAME_SIZE; i++){
	    if (strcmp(cache[i], addin) == 0){
	      location = i;
	      break;
	    }
  	}
  	if (i == FRAME_SIZE){
		for(int j=FRAME_SIZE-2;j>=0;j--){
			for(int k=0;k<5;k++){
				cache[j+1][k]=cache[j][k];
			}
		}
		for(int k=0;k<5;k++){
			cache[0][k]=addin[k];
		}
		for(int k=6;k<10;k++){
			cache[0][k]='\0';
		}	  		  	
		location = 999;
	}
	
	return location;
}


int LRU_search(char *addin, char cache[][10], int FRAME_SIZE){
	int location;
	char buff[10];
	int i=0;
	for(i = 0;i < FRAME_SIZE; i++){
	    if (strcmp(cache[i], addin) == 0){
			location = i;
			for(int j=0;j<10;j++){
			buff[j]=addin[j];	
			}
			for(int j=location-1;j>=0;j--){
				for(int k=0;k<5;k++){
					cache[j+1][k]=cache[j][k];
				}
			}
			for(int k=0;k<5;k++){
				cache[0][k]=addin[k];
			}
			for(int k=6;k<10;k++){
				cache[0][k]='\0';
			}
			break;
	    }
  	}
  	if (i == FRAME_SIZE){
		for(int j=FRAME_SIZE-2;j>=0;j--){
			for(int k=0;k<5;k++){
				cache[j+1][k]=cache[j][k];
			}
		}
		for(int k=0;k<5;k++){
			cache[0][k]=addin[k];
		}
		for(int k=6;k<10;k++){
			cache[0][k]='\0';
		}
		location = 999;
	}
	
	return location;
}

void getFIFOcache(char name[], char cache[][10],int FRAME_SIZE){
	int location;
	int k;
	location = FIFO_search(name,cache,FRAME_SIZE);
	switch(FRAME_SIZE) { 
        case 64: 
            k=0;break; 
        case 128: 
            k=1;break;  
        case 256: 
            k=2;break;  
        case 512: 
            k=3;break;  
        default: 
            k=0;
    } 
	if(location!=999)FIFO_HITS[k]++;
	else FIFO_MISS[k]++;
}

void getLRUcache(char name[], char cache[][10],int FRAME_SIZE){
	int location;
	int k;
	location = LRU_search(name,cache,FRAME_SIZE);
	switch(FRAME_SIZE) { 
        case 64: 
            k=0;break; 
        case 128: 
            k=1;break;  
        case 256: 
            k=2;break;  
        case 512: 
            k=3;break;  
        default: 
            k=0;
    } 
	if(location!=999)LRU_HITS[k]++;
	else LRU_MISS[k]++;
}

/*printf("%s ",name);
printf("%d \n",location);
for(int k=0;k<FRAME_SIZE;k++){
	printf("%s ",cache_address[k]);
}
printf("\n");
system("pause");*/
	
int main()
{

	char name[10];

	FILE *fp;
	char ch;
	fp=fopen("trace.txt","r"); 
	
	init();
	int count=0;
		
	while((fscanf(fp,"%s",name))!=EOF){
		for(int i=5;i<10;i++)name[i]='\0';
		getFIFOcache(name,FIFO_64,64);
		getFIFOcache(name,FIFO_128,128);
		getFIFOcache(name,FIFO_256,256);
		getFIFOcache(name,FIFO_512,512);
		getLRUcache(name,LRU_64,64);
		getLRUcache(name,LRU_128,128);
		getLRUcache(name,LRU_256,256);
		getLRUcache(name,LRU_512,512);
		count++;
		printf("%d \n",count);
	}
	
	fclose(fp);
	printf("\n\n\n");
	printf("FIFO---\n");
	printf("size    miss      hit          page fault ratio\n");//4 6 10
	printf("%3d     %5d     %8d     %1.9f\n",64,FIFO_MISS[0],FIFO_HITS[0],(double)FIFO_MISS[0]/(double)(FIFO_MISS[0]+FIFO_HITS[0]));
	printf("%3d     %5d     %8d     %1.9f\n",128,FIFO_MISS[1],FIFO_HITS[1],(double)FIFO_MISS[1]/(double)(FIFO_MISS[1]+FIFO_HITS[1]));
	printf("%3d     %5d     %8d     %1.9f\n",256,FIFO_MISS[2],FIFO_HITS[2],(double)FIFO_MISS[2]/(double)(FIFO_MISS[2]+FIFO_HITS[2]));
	printf("%3d     %5d     %8d     %1.9f\n",512,FIFO_MISS[3],FIFO_HITS[3],(double)FIFO_MISS[3]/(double)(FIFO_MISS[3]+FIFO_HITS[3]));
	printf("LRU---\n");
	printf("size    miss      hit          page fault ratio\n");//4 6 10
	printf("%3d     %5d     %8d     %1.9f\n",64,LRU_MISS[0],LRU_HITS[0],(double)LRU_MISS[0]/(double)(LRU_MISS[0]+LRU_HITS[0]));
	printf("%3d     %5d     %8d     %1.9f\n",128,LRU_MISS[1],LRU_HITS[1],(double)LRU_MISS[1]/(double)(LRU_MISS[1]+LRU_HITS[1]));
	printf("%3d     %5d     %8d     %1.9f\n",256,LRU_MISS[2],LRU_HITS[2],(double)LRU_MISS[2]/(double)(LRU_MISS[2]+LRU_HITS[2]));
	printf("%3d     %5d     %8d     %1.9f\n",512,LRU_MISS[3],LRU_HITS[3],(double)LRU_MISS[3]/(double)(LRU_MISS[3]+LRU_HITS[3]));
	return 0;
	

}
