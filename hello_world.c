#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

// Linked list object
typedef struct s_word_object word_object; //make word_object = struct s_word_object

//Define a structure called s_word_object containing a string (char) and a pointer containing the address of the next box of this linked list (the next box is of the same type)
struct s_word_object{
	char *word;
	word_object *next;
};
//Create a pointer to store the memory location of the current variable
static word_object *list_head;

//Function to add object to list by taking the data stored in the memory location 'word'
static void add_to_list(char *word) {
//Create a pointer to store the memory location of the last word
	word_object *last_object;

	if (list_head == NULL){
		last_object = malloc(sizeof(word_object));
		list_head = last_object;	
	}
	else {
		last_object = list_head;
		while(last_object->next){
			last_object = last_object->next;
		}
		last_object->next = malloc(sizeof(word_object));
		last_object = last_object->next;
	}
	printf("Next Object Address: %p\n",&last_object->next);
	printf("Word Object Address: %p\n",&last_object->word);
	last_object->word = strdup(word);
	last_object->next = NULL;
}

//Print and free objects function
void print_and_free(void){
//Two pointers are required to print and free the objects. 
	word_object *current_object;
	word_object *old_object;
//The memory address of the current object is the list head
	current_object = list_head;
	while(1){
		printf("%s\n", current_object->word);
		printf("Current Object Address: %p\n",&current_object->next);
		free(current_object->word);
		old_object = current_object;
		printf("Old Object Address: %p\n",&old_object->word);
		
		if(current_object->next){
			current_object = current_object->next;
			free(old_object);
	//if(current_object->next!=NULL){
		//printf("New Current Object Address: %p\n",&current_object->next);}
	//else{printf("No Current Object Adress")};
	}
		else{
			free(old_object);
			break;	
		}
	}
}




int main(int argc, char **argv) {
	char input_word[256];
	int c;
	int option_index = 0;
	int count = -1;
	static struct option long_options[] = {
		{"count",  required_argument, 0, 'c'},
		{0,         0,                 0,  0 }
	};

	while(1){
		c = getopt_long(argc, argv, "c:", long_options, &option_index);
		if(c==-1){break;}
	
		switch(c){
		case'c':
			count = atoi(optarg);
			break;
		}
	}

	fprintf(stderr, "Accepting %i input strings\n",count);

	while(scanf("%256s", input_word) != EOF){
		add_to_list(input_word);
		if (!--count) break;
	}

//Print and free objects
print_and_free();
	return 0;
}

