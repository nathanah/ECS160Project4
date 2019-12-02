
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define maxNameLength 1024
#define maxCountDigits 5 //assumes max of 99999 tweets from one tweeter
#define maxLines 20000
#define maxLineLength 1024


struct countNode{
  char name[maxNameLength];
  int count;
};

struct countNode nodes[maxLines];
struct countNode* top[10];

//Returns a string output of a name count pair


//Returns the number of commas before the name field
int getNameLoc(FILE *fp){
  char line[maxLineLength];
  fgets(line, maxLineLength, fp);

  int count = 0;
  char *token = strtok(line, ",");

  while(token != NULL){
    if(strcmp(token, "name")==0 || strcmp(token, "\"name\"")==0){
      return count;
    }
    token = strtok(NULL, ",");
    count++;
  }
  perror("name not found in header\n");
  exit(1);
}

//Updates the pointers to the top 10 tweeters given the index of a changed value
void updateTop(int index){
  int nameFind;
  for(nameFind=0; nameFind<10; nameFind++){
    if(strcmp(nodes[index]->name,top[nameFind]->name)==0){
      break;
    }
  }
  for(int i=nameFind; i>=0; i--){
    if(top[i]==NULL || (nodes[index].count > top[i]->count)) {
      if(i!=9){
        top[i+1] = top[i];
      }

      top[i] = &nodes[index];
      //printf("Updated top %s\n",toString(top[i]));
    }
  }
}

//Finds the given tweeter and increments their count
void incrementName(char *name){
  //Searching for matching name
  for(int i=0; i<maxLines; i++){
    //Matching name found
    if(strcmp(name, nodes[i].name)==0){
      nodes[i].count++;
      updateTop(i);
      return;
    }

    //End of nodes. Adds new node
    if(nodes[i].count == 0){
      strcpy(nodes[i].name, name);
      nodes[i].count = 1;
      updateTop(i);
      return;
    }
  }

}

//Parses csv to count tweeters
void parse(char *file){
  FILE *fp;
  char line[maxLineLength];

  //Opens file for reading
  fp = fopen(file, "r");
  //Checks if fopen worked
  if(fp == NULL){
    perror("File not found\n");
    exit(1);
  }

  //first read to find name column
  int nameLoc = getNameLoc(fp);

  //Reading lines
  while(fgets(line, maxLineLength, fp)){
    //get name col from line
    char *token = strtok(line, ",");
    for(int i=0; i<nameLoc; i++){
      token = strtok(NULL, ",");
      //if line doesn't have enough commas
      if(token==NULL){
        break;
      }
    }
    //increment name token
    incrementName(token);
  }
  fclose(fp);
}



int main(int argc, char *argv[]) {
  //Initialize top to NULL
  for(int i=0; i<10; i++){
    top[i] = NULL;
  }
  if(argc == 1){
    perror("No file given\n");
    exit(1);
  }
  else{
    char *file = argv[1];
    parse(file);

    //Print top 10
    for(int i = 0; i < 10; i++){
      if(top[i]){
        printf("%i\n",i);
        char countBuf[maxCountDigits];
        sprintf(countBuf,"%d",top[i]->count);
        char str[maxNameLength + 2 + maxCountDigits + 1];
        str[0] = '\0';
        strcat(strcat(strcat(str, top[i]->name), ": "), countBuf);
        printf("%s\n", str);
      }else{
        break;
      }

    }
  }
}
