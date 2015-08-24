#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <pthread.h>

// Linked list object
typedef struct s_word_object word_object; //make word_object = struct s_word_object

//Define a structure called s_word_object containing a string (char) and a pointer containing the address of the next box of this linked list (the next box is of the same type)
struct s_word_object{
	char *word;
	word_object *next;
};
//Only shared resource between two threads, must be accessed with list_lock
static word_object *list_head;
//Create list lock
pthread_mutex_t list_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  list_data_ready = PTHREAD_COND_INITIALIZER;
pthread_cond_t  list_data_flush = PTHREAD_COND_INITIALIZER;

//Function to add object to list by taking the data stored in the memory location 'word'
static void add_to_list(char *word) {
//Create a pointer to store the memory location of the last word
	word_object *last_object, *tmp_object;

	char *tmp_string = strdup(word);
	tmp_object = malloc(sizeof(word_object));

	pthread_mutex_lock(&list_lock); //Put lock on

	if (list_head == NULL){
		last_object = tmp_object;
		list_head = last_object;	
		//Unlock here
		pthread_mutex_unlock(&list_lock);
	}
	else {
		last_object = list_head;
		while(last_object->next){
			last_object = last_object->next;
		}
		last_object->next = tmp_object;
		last_object = last_object->next;
	}
	last_object->word = tmp_string;
	last_object->next = NULL;

	pthread_mutex_unlock(&list_lock);
	pthread_cond_signal(&list_data_ready);
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
		free(current_object->word);
		old_object = current_object;
		if(current_object->next){
			current_object = current_object->next;
			free(old_object);
	}
		else{
			free(old_object);
			break;	
		}
	}
}

word_object *list_get_first(void){
	word_object *first_object;

	first_object = list_head;
	list_head = list_head->next; //move list head along one

	return first_object;
}


static void *print_func(void *arg){
 	word_object *current_object;

 	fprintf(stderr, "Print Thread Starting\n");

	while(1){
		pthread_mutex_lock(&list_lock); //Need a lock before cond_wait is called

		while(list_head == NULL){
			pthread_cond_wait(&list_data_ready, &list_lock); //Wait for signal saying there is data ready
		}
		//There is data in list_head before the following commands are 
		
		current_object = list_get_first(); //Get first piece of data

		pthread_mutex_unlock(&list_lock); //Unlock the list

		printf("Print Thread: %s\n",current_object->word); //Print the 
		free(current_object->word);
		free(current_object);

		pthread_cond_signal(&list_data_flush); //Signals to flusher

	}
	return arg; //Useless statement, stops warning from compiler
 }

static void list_flush(void){

	pthread_mutex_lock(&list_lock);
	
		while (list_head != NULL){
		pthread_cond_signal(&list_data_ready); //signal to printer there is data ready
		pthread_cond_wait(&list_data_flush, &list_lock);
	}

	pthread_mutex_unlock(&list_lock);
}

int main(int argc, char **argv) {
	char input_word[256];
	int c;
	int option_index = 0;
	int count = -1;
	pthread_t print_thread; //Stored in stack location
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

	//Start new thread for printing
	pthread_create(&print_thread, NULL, print_func, NULL); //& is the address operator as function needs address of print_thread


	fprintf(stderr, "Accepting %i input strings\n",count);

	while(scanf("%256s", input_word) != EOF){
		add_to_list(input_word);
		if (!--count) break;
	}
	list_flush();

	return 0;


}
