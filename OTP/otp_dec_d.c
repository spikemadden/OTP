/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

char* encrypt_decrypt (const char* message, const char* key, int choice) {


    int message_length = strlen(message);

    if (message_length != strlen(key)) {
        printf("Message and key are not the same length.\n");
        exit(1);
    }

    int encrypt = 0;

    if (choice == 1)
        encrypt = 1;

    int* message_and_key = malloc(sizeof(int) * (message_length + 1));
    int* modded = malloc(sizeof(int) * (message_length + 1));
    char* ciphertext_or_message = malloc(message_length + 1);



    int i = 0;

    for (i = 0; i < message_length; i++) {

        int message_letter_converted = message[i] - 'A';
        int key_letter_converted = key[i] - 'A';

        //message has a space in position i and key does not.
        if (message_letter_converted == -33 && key_letter_converted != -33) {

            message_letter_converted = 26;

            if (encrypt) {
                message_and_key[i] = 26 + key_letter_converted;
            }

            else {
                message_and_key[i] = 26 - key_letter_converted;
            }

        }
            


        //key has a space in position i and message does not.
        else if (message_letter_converted != -33 && key_letter_converted == -33) {
            
            key_letter_converted = 26;

            if (encrypt) {
                message_and_key[i] = message_letter_converted + 26;
            }

            else {
                message_and_key[i] = message_letter_converted - 26;
            }

        }
            


        //they both have spaces at position i.
        else if (message_letter_converted == -33 && key_letter_converted == -33) {


            if (encrypt) {
                message_and_key[i] = 26 + 26;
            }

            else {
                message_and_key[i] = 26 - 26;
            }

        }
        

        //handles all cases for capital letters only.
        else {
            if (encrypt) {
                message_and_key[i] = message_letter_converted + key_letter_converted;
            }

            else {
                message_and_key[i] = message_letter_converted - key_letter_converted;
            }

        }

    }

    message_and_key[message_length] = '\0';

    for (i = 0; i < message_length; i++) {

        if (message_and_key[i] < 0)
            message_and_key[i] += 27;

        
        modded[i] = message_and_key[i] % 27;
        

        //printf("modded[%d] has %d in it.\n", i, modded[i]);

    }

    modded[message_length] = '\0';

    for (i = 0; i < message_length; i++) {

        char converted_back = modded[i] + 'A';

        if (converted_back ==  '[')
            converted_back = ' ';

        //printf("character in position %d is %c.\n", i, converted_back);

        ciphertext_or_message[i] = converted_back;

    }

    free(message_and_key);
    free(modded);

    return ciphertext_or_message;


}

int main(int argc, char *argv[]) {
     int sockfd, newsockfd, portno;
     socklen_t clilen;
     char buffer[256];

     long length;


     struct sockaddr_in serv_addr, cli_addr;
     int n;
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
    

     while (1) {

         newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

         if (newsockfd == -1) {
            printf("error in accept.\n");
            exit(1);
        }

         int pid = fork();

         if (pid == -1) {
            printf("error in fork.\n");
            close(newsockfd);
            continue;
         }

         //Parent
         else if (pid > 0) {
            close(newsockfd);
            continue;
         }

         //Child
         else {

            close(sockfd);

            //sprintf(buffer, "you are talking to child number %d\n", getpid());

            //n = write(newsockfd, buffer, strlen(buffer));


            bzero(buffer,256);

            //Telling client we are an encryption server.
            n = write(newsockfd, "d", 1);
            
            //Reading length of key/file contents/ciphertext
            n = read(newsockfd, &length, sizeof(long));

            //length = ntohl(length);

            //fprintf(stderr, "The length receieved is %ld\n", length);

            char* file_contents = malloc(length);

            //Reading file contents
            n = read(newsockfd, file_contents, length);

            if (n != length) {
                error("ERROR reading file contents.");
            }

            char* key = malloc(length);

            //Reading key
            n = read(newsockfd, key, length);

             if (n != length) {
                error("ERROR reading key.");
            }

            char* message = malloc(length);

            message = encrypt_decrypt(file_contents, key, 0);

            //Writing ciphertext to client.
            n = write(newsockfd, message, length);
           

            if (n != length) {
                error("ERROR writing message.");
            }


            close(newsockfd);
            break;


         }


     }

    
     return 0; 
}