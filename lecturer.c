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
#include <pthread.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

/*Basic Function*/
int downloadCase();
int uploadCase();
int deleteCase();
void listAll();
int updateCase();
void editCase();

/*Functions to access Shared Memory*/
char* getSharedMemory(int k);
int addToSharedMemory(char caseid[], int id);
int validateSharedMemory(int k);

/*Functions to modify the data*/
char *getTitle(char * fblock);
char *getContent(char * fblock);
char *combineStrings(const char *oldstring, const char c);





#define SHMSIZE 1024  // 1K shared memory segment for each record!

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
      if (strcmp(argv[1], "upload")== 0){
      uploadCase();
      }
	  if (strcmp(argv[1], "download")== 0){
      downloadCase();
      }
	  if (strcmp(argv[1], "delete")== 0){
       deleteCase();
     }
		if (strcmp(argv[1], "update")== 0){
      updateCase();
      }
      if (strcmp(argv[1], "edit")== 0){
        editCase();
        }
    }
pthread_mutex_destroy(&lock);
    return 0;

}

void editCase(){


    char caseid[5];
    FILE *fp1;
    FILE *fp3;
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

    printf("File Downloaded Successfully and ready to be edited\n");
    printf("To update the content, use the update function\n");

    char *args[] = {"vi",filename,NULL};
    execvp("vi",args);

		printf("Edited Case is stored in your downloads folder\n");
}

int updateCase(){

  int k;
	int shmid;
	key_t key;
	char *shm;

	char filename[30];

	char ctitle[24];
	char *title;
	char *body;
	char *data;
	char str[999];

  printf("Enter case ID to update Case: ");
  scanf("%d", &k);

  key = k;

	FILE *fpr;

	printf("Enter path to the edited file: \n");
	scanf("%s", filename);

	// chdir("downloads/");

	fpr = fopen(filename , "r");

	 if (fpr == NULL){
        printf("Could not open file %s",filename);
        return 1;
    }

	printf("Enter the case study title: ");
	scanf("%s", ctitle);

	while (fgets(str, 1000, fpr) != NULL)

	printf("%s\n", str);

	title = combineStrings(ctitle,'*');
	body = combineStrings(str,'#');

	data = strcat(title,body);

	if ((shmid = shmget(key, SHMSIZE, 0666)) < 0)
	{
		printf("shmget failed\n");
		exit(1);
	}

	if ((shm = shmat(shmid, NULL, 0)) == (char *) -1)
	{
		printf("shmat failed\n");
		exit(1);
	}

	memcpy(shm,data,strlen(data));

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
int uploadCase(){

   pthread_mutex_lock(&lock);

  char filename[50];
	FILE *fp2;
	char block[999];
	char tag[30];
	char *title;
	char *content;
	char *data;

  int jkey;
  int count; // Important Variables

  //Asking the user to enter the path of the file

  printf("Enter path to case study to upload: ");
  scanf("%s", filename);

  jkey = shareKey; //Passing the initializer key to temp variable
  count = atoi(getSharedMemory(itemKey)); //Getting the current count of existing records
  jkey += count; //Adding the number of records to the initial key to get a new key value
  count++; //Increasing the count by 1

	fp2 = fopen(filename , "r");

	 if (fp2 == NULL){
        printf("Could not open file %s",filename);
        return 1;
    }

// Asking the user to enter a name for the case study to be saved in the shared memory
	printf("Enter the name for case study : ");
	scanf("%s", tag);

	while (fgets(block, 1000, fp2) != NULL)

	printf("%s\n", block);

//Combining the Name of the case study and it's content together
// A unique delimeter is used to seperate the two later
	title = combineStrings(tag,'*');
  content = combineStrings(block,'#');
	data = strcat(title,content);

//Adding data to the shared memory
	addToSharedMemory(data,jkey);

/* Updating the count of records in shared memory */
	char string1[]="";
  sprintf(string1,"%d",count);
  addToSharedMemory(string1,itemKey);
/*Updating Finished*/

	printf("Case ID is - %i\n",jkey);
  printf("Please use the case ID to access the files\n");

	fclose(fp2);

	return 0;
  pthread_mutex_unlock(&lock);
  pthread_exit(0);
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

	return 1;  pthread_mutex_unlock(&lock);
  pthread_exit(0);


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
int deleteCase()
{

  pthread_mutex_lock(&lock);
  char caseid[4];

  printf("Enter case ID to delete: ");
  scanf("%s", caseid);

	int shmid;
	key_t key;
	char *shm;
	key = atoi(caseid);

	if (validateSharedMemory(key)==0) {
		printf("Invalid Case ID\n");
		return 0;
	}

	if ((shmid = shmget(key, SHMSIZE, 0666)) < 0)
	{
		printf("shmget failed\n");
		exit(1);
	}

	shmctl(shmid, IPC_RMID, NULL);

  printf("Case Deleted Successfully\n");

	return 1;
   pthread_mutex_unlock(&lock);
    pthread_exit(0);

}
void listAll() //This section is explained in the report --  please read it.
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
