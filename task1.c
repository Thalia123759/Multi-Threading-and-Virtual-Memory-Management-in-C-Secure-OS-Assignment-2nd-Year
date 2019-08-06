// This code belongs to IT18350906
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; // Initializing the mutex

struct inputGlobal{ // The global structure that holds all the stuff
  int arr[128]; //This value 128 can be increased if needed
  int m;
  float answer; // Not used in the final build!
};



void* calculateAvg(void* args);   //This function is to calculate the average
void writeToFile(float r);    // This function prints it

int main(void)
{



/***********Counting the number of line to create an equal amount of threads************/
FILE *fileptr;
 int count_lines = 0;
 char filechar[40], chr;

 // printf("Enter file name: ");
 // scanf("%s", filechar);
 fileptr = fopen("dataset.txt", "r");
//extract character from file and store in chr
 chr = getc(fileptr);
 while (chr != EOF)
 {
     //Count whenever new line is encountered
     if (chr == '\n')
     {
         count_lines = count_lines + 1;
     }
     //take next character from file.
     chr = getc(fileptr);
 }
 fclose(fileptr); //close file.
 // printf("There are %d lines in %s  in a file\n", count_lines, filechar);

/**********End of that code*************************************************************/









FILE *in = fopen("dataset.txt", "r"); //reading the file
if (in == NULL){
  printf("Unable to open dataset.txt\n"); //Error handling
  exit(0);
}
if (in != NULL) {
    char line[BUFSIZ]; // declaring variables
    int k,i;
    int j=0; // to track the thread id

    pthread_t  threadID[count_lines+1];

    struct inputGlobal buffer[count_lines+1];

while (fgets(line, sizeof line, in) != NULL) {
              int field1;
              int tmp = 0;
              char *start1 = line;
              int k=0;
              j++;


              while (sscanf(start1, "%d%n", &field1, &tmp) == 1) {
                      buffer[j].arr[k] = field1;
                      start1 += tmp; //The FLAG
                      k++;
                    }

                    buffer[j].m = k;

// Creating the threads
                    pthread_create(&threadID[j], NULL, calculateAvg, &buffer[j]);

}
// joining the threads
for(i=0; i<j; i++){
  pthread_join(threadID[i], NULL);
}

fclose(in);
pthread_mutex_destroy(&lock);

return 0;
}
}


// Function Declaration

void writeToFile(float r){

  FILE *out = fopen("average.txt","a");

    fprintf(out, "%.3f\n", r);
    fclose(out);

}


void* calculateAvg(void* args)
{
    pthread_mutex_lock(&lock);
    struct inputGlobal *buffer2_ptr = (struct inputGlobal*) args;

    int sum = 0;   // declaring variables
    float avg = 0;
    int x;
    int g = 0;
    g = (buffer2_ptr->m);

    for(x=0; x < g; x++){
      sum = sum + (buffer2_ptr->arr[x]);
    }

    printf("Sum is %d\n", sum);

    avg = sum / g;

    printf("Average is %f\n", avg);

    writeToFile(avg); //Passing the calculated value to print function


    pthread_mutex_unlock(&lock);
    pthread_exit(0);
}
