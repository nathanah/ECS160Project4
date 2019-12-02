
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
char* toString(struct countNode *cn){
  static char str[maxNameLength + 2 + maxCountDigits + 1];
  char countBuf[maxCountDigits];
  sprintf(countBuf,"%d",cn->count);
  strcat(strcat(strcat(str, cn->name), ": "), countBuf);
  return str;
}

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
  for(int i=9; i>=0; i--){
    if(top[i]==NULL || (nodes[index].count > top[i]->count)) {
      if(i!=9){
        printf("we found a new top\n");
        top[i+1] = top[i];
      }
      printf("Updated top\n");
      top[i] = &nodes[index];
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
    }else if(nodes[i].count == 0){
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
  struct countNode nodes[maxLines];
  struct countNode* top[10];
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
        printf("%s\n", toString(top[i]));
      }else{
        break;
      }

    }
  }
}
