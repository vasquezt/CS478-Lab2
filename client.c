#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
//#include <mirdef.h> 
//#include <miracl.h>
//#include <libmiracl.a>
#include "miracl.h"


//NOTE, compile with: gcc client.c -I ~/Libraries/MIRACL ~/Libraries/MIRACL/miracl.a -o client

void error(const char *msg) { perror(msg); exit(0); } // Error function used for reporting issues
char* buildMerkelTree(char *);


int main(int argc, char *argv[])
{
	if (argc < 2) { fprintf(stderr,"USAGE: client [filename]\n"); exit(1); } // Check usage & args
	FILE *textp, *keyp;
	//argv[1] needs to be the plain text
	textp = fopen(argv[1], "rb");
	if(textp == NULL){
		error("Plain text file not found");
		exit(1);
	}
	//argv[2] needs to be the key
	//key > plain text
	fseek(textp, 0, SEEK_END);
	int lengthOfText = ftell(textp);

	char *textFileContents;
	char *hash;
	fseek(textp, 0, SEEK_SET);
	textFileContents = malloc(lengthOfText);
	fread(textFileContents, 1, lengthOfText, textp);
	printf("lengthOfText = %d\n", lengthOfText);	

	hash = buildMerkelTree(textFileContents);

	return 0;	
}

/*
* Name:  buildMerkelTree
* Input: char* array
* Goal:  breaks file into merkel tree using 256 bit leafs
*/


char* buildMerkelTree(char *textFileContents){
	sha256 pointer;
	shs256_init(&pointer);
	int lengthOfText = strlen(textFileContents);
	int midpoint = lengthOfText/2;
	printf("midpoint %d\n", midpoint);
	char *hash;
	hash = malloc(sizeof(32));
	int i;

	if(lengthOfText > 256){ // Should we break into multiple leafs
		char *array1, *array2, *hash1, *hash2, *concatHash;
		array1 = malloc(midpoint);
		array2 = malloc(midpoint);
		memcpy(array1, textFileContents, midpoint);
		memcpy(array2, textFileContents + midpoint - 1, midpoint);
		printf("array1: %d\n", strlen(array1));
		printf("array2: %d\n", strlen(array2));		
		// recursivly call function twice for right and left half
		hash1 = buildMerkelTree(array1);
		hash2 = buildMerkelTree(array2);
		printf("returned second hash\n");
		// process both and hash them 	
		for(i = 0; i < 32; i++){
			shs256_process(&pointer, hash1[i]);
		}
		for(i = 0; i < 32; i++){
			shs256_process(&pointer, hash2[i]);
		}
		printf("hash result for 1 = %s\n", hash1);
		printf("hash result for 2 = %s\n", hash2);		

		shs256_hash(&pointer, &hash);
	}else{ // We are at the bottom
		int padding = 256 - lengthOfText;
		for(i = 0; i < lengthOfText - 1; i++){
			shs256_process(&pointer, textFileContents[i]);
		}
		//for(i = 0; i < padding; i++){
		//	shs256_process(&pointer, '0');
		//}
		//hash here	
		shs256_hash(&pointer, &hash);
	}
	//return hash string
	printf("%d\n", hash);
	return hash;
}
