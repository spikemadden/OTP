#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int check_valid_characters (FILE* fp) {

	int character;

	while ((character = fgetc(fp)) != EOF) {

		if ((char)character >= 'A' && (char)character <= 'Z' || (char)character == ' ' || (char)character == '\n')
			continue;

		else {
			fprintf(stderr, "Invalid character found.\n");
			return 1;
		}

	}

	return 0;
}


char* text_from_file (char* file_name) {

	FILE* fp;
	int test;

	long lSize;
	char* buffer;

	struct stat file_info;

	fp = fopen(file_name, "rb");

	if (fp == NULL) {
		printf("Error opening file.\n");
		exit(1);
	}

	test = check_valid_characters(fp);

	if (test) {
		fprintf(stderr,"Exiting the program.\n");
		exit(1);
	}

	fseek (fp, 0, SEEK_END);
	lSize = ftell(fp);
	rewind(fp);


	buffer = (char*) malloc (sizeof(char)*lSize);
	
	if (buffer == NULL) {
		fputs ("Memory error", stderr);
		exit(2);
	}

	fread(buffer, 1, lSize, fp);
	
	/*
	int j = 0;
	for (j = 0; j < strlen(buffer); j++) {
		printf("the character in position %d is %d\n", j, buffer[j]);
	}
	*/

	if (buffer[strlen(buffer) - 1] == '\n') {
		buffer[strlen(buffer) - 1] = '\0';
	}

	return buffer;
}

int main (int argc, char *argv[]) {
     
	int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    long key_plaintext_ciphertext_length;

    char* server_name;
    char* colon_pointer;
    char* file_contents;
    char* whole_key;
    char* key;

    FILE* fp;

    //char buffer[256];

    
    if (argc != 4) {
       fprintf(stderr,"usage %s plaintext key port\n", argv[0]);
       printf("argc is %d\n", argc);
       exit(0);
    }

    file_contents = text_from_file(argv[1]);

    whole_key = text_from_file(argv[2]);

    key = strndup(whole_key, strlen(file_contents));



    //key = argv[2];
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
    if (sockfd < 0) 
        error("ERROR opening socket");
    

    if (colon_pointer = index(argv[3], ':')) {
    	server_name = strndup(argv[3], strlen(argv[3]) - strlen(colon_pointer));
    	portno = atoi(colon_pointer + 1);
    }

    else {
    	server_name = "localhost";
    	portno = atoi(argv[3]);
    }

    server = gethostbyname(server_name);

    
    bzero((char *) &serv_addr, sizeof(serv_addr));
    
    serv_addr.sin_family = AF_INET;
    
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
   
    serv_addr.sin_port = htons(portno);
    
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");


    key_plaintext_ciphertext_length = strlen(file_contents);

    char* buffer = malloc(key_plaintext_ciphertext_length);
    char server_type;

    
    //Reading what the server type is.
    n = read(sockfd, &server_type, 1);

    if (server_type != 'd') {

        fprintf(stderr, "You connected to the wrong port.\n");
        exit(2);

    }


    //key_plaintext_ciphertext_length = htonl(key_plaintext_ciphertext_length);

    n = write(sockfd, &key_plaintext_ciphertext_length, sizeof(long));

    if (n != sizeof(long)) 
         error("ERROR sending size of file.\n");

    n = write(sockfd, file_contents, key_plaintext_ciphertext_length);

    if (n != strlen(file_contents))
    	error("ERROR sending plaintext file content.\n");

    n = write(sockfd, key, key_plaintext_ciphertext_length);

    if (n != strlen(key))
    	error ("ERROR sending key.\n");

    n = read(sockfd, buffer, key_plaintext_ciphertext_length);

    buffer[key_plaintext_ciphertext_length] = '\0';

    if (n < 0) 
        error("ERROR reading from socket");
    
    printf("%s\n", buffer);
    
    close(sockfd);

    
    return 0; 
}