#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>

/*Basic Functions*/
int downloadCase();
void listAll();
/*Functions to access Shared Memory*/
char* getSharedMemory(int k);
int addToSharedMemory(char caseid[], int id);
int validateSharedMemory(int k);

/*Functions to modify the data*/
char *getTitle(char * fblock);
char *getContent(char * fblock);
char *combineStrings(const char *oldstring, const char c);

#define SHMSIZE 1024  /* make it a 1K shared memory segment */

int itemKey = 6969;
int shareKey = 8800;

// char SEM_NAME[]= "vik";

int main(int argc,char *argv[])
{
  //Initializing the counter for firsttime use!
	if (validateSharedMemory(itemKey)==0)
	{
		addToSharedMemory("0",itemKey);
	}

    if(argc==1)
        printf("No arguments passed\n");
    if(argc==2)
    {
      if (strcmp(argv[1], "view")== 0){
        listAll();
      }
      if (strcmp(argv[1], "download")== 0){
        downloadCase();
      }

    }

    return 0;

}



int downloadCase()
{

  char caseid[5];
  FILE *fp1;
	char filename[50];

  printf("Enter case ID: ");  //Asking the user to input case ID
  scanf("%s", caseid);

  //Getting the content from the shared Memory related to caseID provided
	char *dataBlock = getSharedMemory(atoi(caseid));

  // Passing it through the seperation function to separate the title and it's contents
	char *title = getTitle(dataBlock);
	char *content = getContent(dataBlock);

	chdir("downloads/"); //cd into the downloads folder

	snprintf(filename, sizeof(filename), "%s", title); //Getting the case study name as filename
  strcat(filename,".txt"); // Adding the extension

	fp1 = fopen(filename, "w"); //Opening the file to write
  fprintf(fp1, "%s", content);
  fflush(fp1);
  fclose(fp1);

  printf("File Downloaded Successfully\n");
  return 0;
}

char *getTitle(char * fblock)
{
	 char *block1;
   char delim[] = "*"; //Delimiter used to seperate the name of the case study
	 char temp[50]; //Array to temporarily store the data

	strcpy(temp, fblock);  //  Copy the full block to array
	block1  = strtok(temp, delim); // Seperate by delimiter and passs to block1

	// printf("in func %s\n", block1);
  return block1;
}
char *getContent(char * fblock)
{
	char temp2[999]; // Array to temporarily store the full block from Shared Memory
  char delim1[] = "*";  // Delimiter to seperate the name of case study
	char delim2[] = "#"; // Delimiter to seperate the content

	strcpy(temp2, fblock);
	char *block2 = strtok(temp2, delim1);
	// printf("in func 2 %s\n", block2);  //This printf prints out the title
	// printf("in func 2 %s\n", temp2);

	block2 = strtok(NULL,delim2);
	// printf("in func2 %s\n", block2);
	return block2;
}
char *combineStrings(const char *oldstring, const char c)
{

    int result;
    char *newstring;

    /*Combines the special character used as the delimiter with the string passed into it*/
    result = asprintf(&newstring, "%s%c", oldstring, c);

    if (result == -1){
      newstring = NULL;
    }
    return newstring;
}
int addToSharedMemory(char data[], int id)
{

	int shmid;
	key_t key;
	char *shm;
	key = id;

	if ((shmid = shmget(key, SHMSIZE, IPC_CREAT | 0666)) < 0) //Creating the shared Memory block
	{
		printf("shmget failed\n");
		exit(1);
	}

	if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) //Attaching to shared Memory
	{
		printf("shmat failed\n");
		exit(1);
	}

	strncpy(shm, data, strlen(data));  //Passing the content

	return 1;

}
char* getSharedMemory(int k)
{
	int shmid;
	key_t key;
	char *shm;
	key = k;

	if ((shmid = shmget(key, SHMSIZE, 0666)) < 0) //Checking if the segment exists or not?
	{
		printf("shmget failed\n");
		exit(1);
	}

	if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) //Checking whether you can attach to it or not?
	{
		printf("shmat failed\n");
		exit(1);
	}

	return shm; //return the segment
}
int validateSharedMemory(int k)
{
	int shmid;
	key_t key;
	key = k;

	if ((shmid = shmget(key, SHMSIZE, 0666)) < 0) //Checking if the segment exists
	{
		return 0;
	}

	return 1;
}

void listAll()
{
	int item_count = atoi(getSharedMemory(itemKey));

  printf("Case ID  | Name of the Study\n");

	for(int i=shareKey;i<(shareKey+item_count);i++)
	{
		if (validateSharedMemory(i)==0){
      continue;
    }

	char *data = getSharedMemory(i);
	char *tag = getTitle(data);

	printf("%i        | %s\n", i, tag);
	}
}
