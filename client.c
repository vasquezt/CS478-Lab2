#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include "miracl.h"

//NOTE, compile with: gcc client.c -I ~/Libraries/MIRACL ~/Libraries/MIRACL/miracl.a -o client

/* STRUCTS */
struct key{
	int k;
	int t;
	char **substring;
};

typedef struct key key;

/* DECLARATIONS */
void error(const char *msg) { perror(msg); exit(0); } // Error function used for reporting issues
char* buildMerkelTree(char *);
void HorsKeygen(int, int, int, key *, key *);
void HorsSign(key *, char *);

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
	miracl *mip=mirsys(100,0);	
	srand(time(0));
	//argv[2] needs to be the key
	//key > plain text
	fseek(textp, 0, SEEK_END);
	int lengthOfText = ftell(textp);

	char *content;
	char *hash;
	fseek(textp, 0, SEEK_SET);
	content = malloc(lengthOfText);
	fread(content, 1, lengthOfText, textp);

	struct key PK, SK;
	HorsKeygen(32, 32, 32, &PK, &SK);
	HorsSign(&SK, content);
}

/*
* Name:  HorsKeygen
* Input: length of substrings, 
* Goal:  t = numb of substrings, l is length, k is length of signitures 
		 PK and SK key pointers
*/

void HorsKeygen(int l, int k, int t, key *PK, key *SK){
	/*csprng rng;            //  |
	char *seed = "seed";     //  |
	mr_unsign32 tod;         //  |
	time(&tod); //Needs better time and seed to be truely random
	strong_init(&rng, 4, seed, tod);*/
	sha256 sh;
	shs256_init(&sh);	
	int i, j, r;
	SK->k = k;
	SK->t = t;
	PK->k = k;
	SK->substring = malloc(t*sizeof(char*));
	PK->substring = malloc(t*sizeof(char*));
	for(i = 0; i < t; i++){
		SK->substring[i] = malloc(l);
		PK->substring[i] = malloc(32); // to store hash return
		for(j = 0; j < l; j++){
			//r = strong_rng(&rng);
			r = rand(); 
			SK->substring[i][j] = r; 
			if(i == 0 && j == 0){
			}
			shs256_process(&sh, r);
		}
		shs256_hash(&sh, PK->substring[i]);
	}
}

/*
* Name:  HorsSign
* Input: Takes a secret key (SK generated with HorsKeygen) 
*        and a message (char * array)
* Goal:  This function will sign 
*/ 

void HorsSign(key *SK, char *message){
	//miracl *mip=mirsys(100,0);
	char* hash;
	big bt;
	bt = mirvar(SK->t);
	hash = buildMerkelTree(message);
	int slice;
	slice = logb2(bt);
	printf("%d value of logb2 of t\n", slice);
}


/*
* Name:  buildMerkelTree
* Input: char* array (content you want to be hashed)
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

