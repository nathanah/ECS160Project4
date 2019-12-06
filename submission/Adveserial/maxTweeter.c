#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_NAMES 10
#define NUM_LINE_MAX 20000
#define LINE_MAX 1024

struct linkedlist {
	struct node* head;
	struct node* tail;
};

struct node {
	char name[LINE_MAX + 1];
	int count;
	struct node* next;
};

//add a new node to list
void linkedlist_add(struct linkedlist* list, char* str) {
	struct node* new_node = malloc(sizeof(struct node));
	strcpy(new_node->name, str);
	new_node->count = 1;
	new_node->next = NULL;

	if (list->head == NULL) {
		// If no other nodes exist
		list->head = new_node;
		list->tail = new_node;
	} else {
		// If other nodes exist
		list->tail->next = new_node;
		list->tail = new_node;
	}
}

//increment a counter or create new node
void linkedlist_inc(struct linkedlist* list, char* str) {
	struct node* current = list->head;
	while (current != NULL) {
		if (strcmp(current->name, str) == 0) {
			current->count++;
			return;
		}

		current = current->next;
	}

	// If arrives here, need to add to linkedlist
	linkedlist_add(list, str);
}

//delete the list
void linkedlist_del(struct linkedlist* list) {
	struct node* current = list->head;
	while (current != NULL) {
		struct node* temp = current->next;
		free(current);
		current = temp;
	}

	free(list);
}

// Returns 1 if quoted
int check_quoted(char* str) {
	// If str has nothing, not quoted
	if (strlen(str) == 0) return 0;

	// Make sure quotes are matching
	if (strlen(str) >= 2 && str[0] == '"' && str[strlen(str) - 1] == '"') return 1;

	// Check for dangling quotes
	if (str[0] == '"' || str[strlen(str) - 1] == '"') return -1;

	// Otherwise not quoted
	return 0;
}

// Copies src into dest while removing quotes
void strcpy_unquoted(char* dest, char* src, int is_quoted) {
	if (is_quoted == 1){
		strcpy(dest, src + 1);
		dest[strlen(dest) - 1] = '\0';
	} else {
		strcpy(dest, src);
	}
}

int check_body_inc(char* str, struct linkedlist* list, int isquoted[], int counter, int name_loc) {
	if (isquoted[counter] == 0) {
		// Make sure str is not quoted
		int quoted = check_quoted(str);
		if (quoted != 0) return -1;
	} else if(isquoted[counter] == 1) {
		// Make sure str is quoted as well
		int quoted = check_quoted(str);
		if (quoted != 1) return -1;
	} else {
		return -1;
	}

	if (counter == name_loc) {
		char cur_name[LINE_MAX + 1];
		strcpy_unquoted(cur_name, str, check_quoted(str));
		linkedlist_inc(list, cur_name);
		return 0;
	}

	return 0;
}

// Returns -1 if invalid line
// Otherwise, counts the name
int parse_line(char* line, struct linkedlist* list, int name_loc, int isquoted[]) {
	if (line == NULL) {
		return -1;
	}

	int counter = 0;
	char* p;

	p = strtok(line, ",");
	if (p == NULL) return -1;
	if (check_body_inc(p, list, isquoted, counter, name_loc) == -1) return -1;
	counter++;

	while(p != NULL){
		p = strtok(NULL, ",");
		if (p == NULL) break;

		if (check_body_inc(p, list, isquoted, counter, name_loc) == -1) return -1;
		counter++;
	}

	return 0;
}

int insert_space(char* line){
	if (strlen(line) > LINE_MAX) { //the length of line shouldn't exceed 1024
		return -1;
	}

	for (int i = 1; i < strlen(line); i++) {
		if (line[i-1] == ',' && line[i] == ',') {
			if (i + 1 >= strlen(line)) {
				line[i] = ' ';
				line[i+1] = ',';
				line[i+2] = '\0';
			} else {
				line[i] = '\0';
				char* p = line + i + 1;
				char buf[LINE_MAX];
				strcpy(buf,p);
				strcat(line, " ,");
				strcat(line, buf);
				i++;
			}
		}
	}
	return 0;
}

int get_name_location(char* header) {
	// Must have "name" column
	if(header == NULL){
		return -1;
	}

	int comma_count = 0;
	for (int i = 0; i < LINE_MAX; i++) {
		if (header[i] == '\0') return -1;
		if (header[i] == ',') comma_count++;
		if (strncmp(header + i, "name", 4) == 0 &&
			i < LINE_MAX - 4) return comma_count;
		if (strncmp(header + i, "\"name\"", 6) == 0 &&
			i < LINE_MAX - 6) return comma_count;
	}
	return -1;
}

// check if the header is valid
int header_check(char* header, int isquoted[]){
	if(header == NULL){
		return -1;
	}
	if (strlen(header) > LINE_MAX) { //the length of line shouldn't exceed 1024
		return -1;
	}

	int counter = 0;
	char columns[LINE_MAX][LINE_MAX + 1];
	char headercp[LINE_MAX + 1];
	char* p;

	strcpy(headercp,header);

	p = strtok(headercp, ",");
	if (p == NULL) return -1;

	isquoted[counter] = check_quoted(p);
	if (isquoted[counter] == -1) return -1;
	strcpy_unquoted(columns[counter], p, isquoted[counter]);
	counter++;

	while (p != NULL) {
		p = strtok(NULL, ",");
		if (p == NULL) break;

		isquoted[counter] = check_quoted(p);
		if (isquoted[counter] == -1) return -1;
		strcpy_unquoted(columns[counter], p, isquoted[counter]);
		counter++;
	}

	// Look for duplicate headers
	for(int i=0;i < counter; i++){
		for(int j=i+1; j<counter; j++){
			if(strcmp(columns[i], columns[j]) == 0){
				return -1;
			}
		}
	}

	return 0;
}

//sort the list
void sortlist(struct linkedlist* list){
	struct node* current = list->head;
	while(current != NULL){
		struct node* max = current->next;
		struct node* prevmax = current;
		while(max != NULL){
			if(max->count > prevmax->count){
				prevmax = max;
			}
			max = max->next;
		}

		// Swap
		int temp = current->count;
		char temp_name[LINE_MAX + 1];
		strcpy(temp_name, current->name);
		current->count = prevmax->count;
		strcpy(current->name, prevmax->name);
		prevmax->count = temp;
		strcpy(prevmax->name, temp_name);
		current = current->next;
	}
}

void rem_newline(char* str) {
	// Remove any new line
	if (*(str + strlen(str) - 1) == '\n') {
		*(str + strlen(str) - 1) = '\0';
	}
}

int main(int argc, char** argv) {
	// Check for argument
	if (argc != 2){
		printf("There should be 2 arguments\n");
		printf("Usage: ./maxTweeter <file path>");
		return -1;
	} 

	FILE * csv = fopen(argv[1], "r");

	// Check that file was opened
	if (!csv) { 
		printf("Invalid Input Format\n");
		return -1;
	}

	// Parse header
	char line[2*LINE_MAX];
	int isquoted[LINE_MAX];
	//isquoted = 1 -> quoted
	//isquoted = 0 -> not quoted
	//isquoted = -1 -> out of range
	memset(isquoted, -1, LINE_MAX);

	fgets(line, 2*LINE_MAX, csv);
	rem_newline(line);

	// Check for valid header
	if(header_check(line, isquoted) == -1){
		printf("Invalid Input Format\n");
		return -1;
	}

	// Check header for name location
	int name_loc = get_name_location(line);
	if (name_loc == -1) {
		printf("Invalid Input Format\n");
		return -1;
	}

	// Store in some data structure (linked list for now)
	struct linkedlist* list = malloc(sizeof(struct linkedlist));
	int linecount = 1;

	// Add each line to list
	while (fgets(line, 2*LINE_MAX, csv) != NULL) {
		rem_newline(line);
		if(insert_space(line) == -1){
			printf("Invalid Input Format\n");
			return -1;
		}
		if (parse_line(line, list, name_loc, isquoted) == -1) {
			printf("Invalid Input Format\n");
			return -1;
		}
		linecount++;
	}

	if (linecount > NUM_LINE_MAX) { //length of the file shouldn't exceed 20000 lines
		printf("Invalid Input Format\n");
		return -1;
	}

	// sort the list
	sortlist(list);
	struct node* current = list->head;
	int count = 0;
	
	// Output the list
	while(current != NULL && count < NUM_NAMES){
		printf("%s: %d\n" ,current->name, current->count);
		current = current->next;
		count++;
	}

	linkedlist_del(list);
	
	exit(0);
}
