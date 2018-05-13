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

	char *content;
	char *hash;
	fseek(textp, 0, SEEK_SET);
	content = malloc(lengthOfText);
	fread(content, 1, lengthOfText, textp);
	//printf("lengthOfText = %d\n", lengthOfText);	

	hash = buildMerkelTree(content);
	printf("hash: %ld\n", (long) hash);
	return 0;	
}

/*
* Name:  buildMerkelTree
* Input: char* array
* Goal:  breaks file into merkel tree using 256 bit leafs
*/


char* buildMerkelTree(char *content){
	sha256 sh;
	shs256_init(&sh);
	int lengthOfText = strlen(content);
	char* hash;
	hash = malloc(32);
	int i;
	if(lengthOfText > 256){
		//split array into two seperate arrays
		int midpoint = lengthOfText/2;
		char *arr1, *arr2, *h1, *h2;
		arr1 = malloc(midpoint);
		arr2 = malloc(lengthOfText - midpoint); // to account for integer math
		memcpy(arr1, content, midpoint);
		memcpy(arr2, content + midpoint - 1, lengthOfText - midpoint);
		//call the merkel tree function recursivly
		h1 = buildMerkelTree(arr1);
		h2 = buildMerkelTree(arr2);
		//concatinate the two
		for(i = 0; i < 32; i++){
			shs256_process(&sh, &h1[i]);
			shs256_process(&sh, &h2[i]);
		}
		//hash and return
		shs256_hash(&sh, &hash);
	}else{
		for(i = 0; i < lengthOfText; i++){
			shs256_process(&sh, content[i]);
		}
		shs256_hash(&sh, &hash);
	}
	return hash;
}

