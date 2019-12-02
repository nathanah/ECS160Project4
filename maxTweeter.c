
#include <stdio.h>
#include<stdlib.h>

#define maxNameLength 1024
#define maxCountDigits 5 //assumes max of 99999 tweets from one tweeter
#define maxLines 20000
#define maxLineLength 1024


struct countNode{
  char name[maxNameLength];
  int count;
}

//Returns a string output of a name count pair
char* toString(countNode *cn){
  char str[maxNameLength + 2 + maxCountDigits + 1];
  char countBuf[maxCountDigits];
  itoa(cn->count, countBuf, 10);
  strncat(strncat(strncat(str,cn->name), ": "), countBuf);
  return str;
}

//Returns the number of commas before the name field
int getNameLoc(FILE *fp){
  char line[maxLineLength];
  fgets(line, maxLineLength, fp);

  count = 0;
  char *token = strtok(line, ",");

  while(token != NULL){
    if(strcmp(token, "name")==0){
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
  for(int i=9; i>=0; i--){
    if(nodes[index].count > top[i]->count){
      if(i!=9){
        top[i+1] = top[i];
      }
      top[i] = &nodes[index]
    }
  }
}

//Finds the given tweeter and increments their count
void incrementName(char *name){
  for(int i=0; i<maxLines; i++){
    if(strcmp(name, nodes[i].name)==0){
      nodes[i].count++;
      updateTop(i);
      return;
    }
    if(nodes[i].count == 0){
      nodes[i].name = name;
      nodes[i].count = 1;
      updateTop(i);
      return;
    }
  }

}

//Parses csv to count tweeters
void parse(char *file){
  FILE *fp;
  char *line = NULL;
  size_t = len = 0;

  //Opens file for reading
  fp = fopen(file, "r");
  //Checks if fopen worked
  if(fp == NULL){
    perror("File not found\n");
    exit(1);
  }

  char line[maxLineLength];
  //first read to find name column
  int nameLoc = getnameLoc(fp);

  //Reading lines
  while(fgets(line, maxLineLength, fp)){
    //get name col from line
    char *token = strtok(line, ",");
    for(i=0; i<nameLoc; i++){
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
  countNode nodes[maxLines];
  countNode* top[10];
  if(argc == 0){
    perror("No file given\n");
    exit(1);
  }
  else{
    char *file = argv[0];
    parse(file);

    //Print top 10
    for(i = 0; i < 10; i++){
      if(top[i])
        printf(toString(top[i]));
    }
  }
}
