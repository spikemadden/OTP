#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>



int main(int argc, char* argv[]) {

	srand(time(NULL));


	if (argc != 2) {
		printf("Please enter the length of the key.\n");
		exit(1);
	}


	int key_length = atoi(argv[1]);

	char* key = malloc(key_length + 1);
	
	int i = 0;

	for (i = 0; i < key_length; i++)
		key[i] = 'A' + random() % 26;

	key[key_length] = '\0';
	

	printf("%s\n", key);

	return 0;

}
