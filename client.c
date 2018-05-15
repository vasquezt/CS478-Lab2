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

struct sig{
	char **substring;
	int *pointers;
};

typedef struct sig sig;
typedef struct key key;

/* DECLARATIONS */
void error(const char *msg) { perror(msg); exit(0); } // Error function used for reporting issues
char* buildMerkelTree(char *);
void HorsKeygen(int, int, int, key *, key *);
sig* HorsSign(key *, char *);
void HorsVer(key *, char *, sig *);
int exponentFunc(int, int);

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
	miracl *mip=mirsys(32,0);	
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
	struct sig *THETA;
	HorsKeygen(80, 16, 1024, &PK, &SK);
	THETA = HorsSign(&SK, content);
	HorsVer(&PK, content, THETA);


}

/*
* Name:  HorsKeygen
* Input: length of substrings, 
* Goal:  t = numb of substrings, l is length, k is length of signitures 
		 PK and SK key pointers
*/

void HorsKeygen(int l, int k, int t, key *PK, key *SK){
	sha256 sh;
	shs256_init(&sh);	
	int i, j, r;
	SK->k = k;
	SK->t = t;
	PK->t = t;
	PK->k = k;
	SK->substring = malloc(t*sizeof(char*));
	PK->substring = malloc(t*sizeof(char*));
	for(i = 0; i < t; i++){
		SK->substring[i] = malloc(l);
		PK->substring[i] = malloc(32); // to store hash return
		for(j = 0; j < l; j++){
			r = rand(); 
			SK->substring[i][j] = r; 
			if(i == 0 && j == 0){
			}
			shs256_process(&sh, r);
		}
		shs256_hash(&sh, PK->substring[i]);
		//printf("PK at %d: %s\n", i, PK->substring[i]);
	}
}

/*
* Name:  HorsSign
* Input: Takes a secret key (SK generated with HorsKeygen) 
*        and a message (char * array)
* Goal:  This function will sign 
*/ 

sig* HorsSign(key *SK, char *message){
	sig* THETA;
	THETA = malloc(sizeof(sig));
	THETA->pointers = malloc(sizeof(int) * SK->k);
	THETA->substring = malloc(sizeof(char*) * SK->k);
	char* hash;
	big bt;
	bt = mirvar(SK->t);
	hash = buildMerkelTree(message);
	int slice, i, base, j;
	slice = logb2(bt);
	slice--;
	base = exponentFunc(1, slice);
	//Bitwise shifiting to get pointers
	for (i = 0; i < SK->k; ++i)
	{
		j = ((long) hash >> (slice * i)) & base;
		THETA->pointers[i] = j;
		printf("value of j %d\n", THETA->pointers[i]);
	}
	//Getting values from secret key that match the pointer location
	for(i = 0; i < SK->k; i++){
		THETA->substring[i] = SK->substring[THETA->pointers[i]]; 
		printf("%d == %d\n", THETA->substring[i], SK->substring[THETA->pointers[i]]);		
	}


	return THETA;
}

void HorsVer(key *PK, char *message, sig *THETA){
	char* hash;
	big bt, val1, val2;
	bt = mirvar(PK->t);
	hash = buildMerkelTree(message);
	int slice, i, base, j;
	int *pointers;
	slice = logb2(bt);
	slice--;
	base = exponentFunc(1, slice);
	//Bitwise shifting
	pointers = malloc(sizeof(int) * PK->k);
	for (i = 0; i < PK->k; i++)
	{
		j = ((long) hash >> (slice * i)) & base;
		pointers[i] = j;
	}
	//Verification Step
	int verified = 1;
	printf("about to do some comparisions\n");
	for(i = 0; i < PK->k; i++){
		j = pointers[i];
		bytes_to_big(32, THETA->substring[i], val1); 		
		bytes_to_big(32, PK->substring[j], val2); //Val 2 is vsubisubj	
	}/*
       //Val 1 is f(s(prime)subj)

		if(mr_compare(val1, val2) != 0){
			printf("MISSED\n");
			verified = 0;
		}
	}
	if(verified == 1){
		printf("The message is verified\n");
	}else{
		printf("The message was NOT verified\n");
	}*/
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

int exponentFunc(int prev, int num){ //Always put one as prev on base
	if(num != 1){
		return (prev + exponentFunc((prev * 2), num - 1));
	}else{
		return prev;
	}
}