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
	int l;
	char **substring;
};

struct sig{
	char **substring;
	int *pointers;
};

struct merkelTree{
	char* hash;
	struct merkelTree *rchild;
	struct merkelTree *lchild;
};

typedef struct merkelTree merkelTree;
typedef struct sig sig;
typedef struct key key;

/* DECLARATIONS */
void error(const char *msg) { perror(msg); exit(0); } // Error function used for reporting issues
char* getMerkelTreeRoot(char *);
void HorsKeygen(int, int, int, key *, key *);
sig* HorsSign(key *, char *);
void HorsVer(key *, char *, sig *);
void DtimeHorsKeygen(int, int, int, key *, key *);
sig* DtimeHorsSign(key *, char *);
void DtimeHorsVer(key *, char *, sig *);
int exponentFunc(int, int);
merkelTree* buildMerkelTreeRoot(char*);

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
	miracl *mip = mirsys(100, 10);	
	srand(time(0));
	//argv[2] needs to be the key
	//key > plain text
	fseek(textp, 0, SEEK_END);
	int lengthOfText = ftell(textp);

	char *content;
	char *hash, *hash2;
	fseek(textp, 0, SEEK_SET);
	content = malloc(lengthOfText);
	fread(content, 1, lengthOfText, textp);

	getMerkelTreeRoot(content);

	struct key PK, SK;
	struct sig *theta;
	HorsKeygen(80, 16, 1024, &PK, &SK);
	theta = HorsSign(&SK, content);
	//content[0] = '!';
	HorsVer(&PK, content, theta);



}

/*
* Name:  DtimeHorsKeygen
* Input: length of substrings, 
* Goal:  t = numb of substrings, l is length, k is length of signitures 
		 PK and SK key pointers
*/

void DtimeHorsKeygen(int l, int k, int t, key *PK, key *SK){
	//get input, declare variables
	sha256 sh;
	shs256_init(&sh);	
	int i, j, r, lbits;
	SK->k = k; SK->t = t; SK->l = l;
	PK->k = k; PK->t = t; PK->l = l;
	SK->substring = malloc(t*sizeof(char*));
	PK->substring = malloc(t*sizeof(char*));
	lbits = l/8; //CONVERTED BYTES TO BITS
}

/*
* Name:  DtimeHorsSign
* Input: Takes a secret key (SK generated with HorsKeygen) 
*        and a message (char * array)
* Goal:  This function will sign with SK and the message
*/ 

sig* DtimeHorsSign(key *SK, char *message){
	sig* theta;
	theta = malloc(sizeof(sig));
	theta->pointers = malloc(sizeof(int) * SK->k);
	theta->substring = malloc(sizeof(char*) * SK->k);
	char* hash;
	big bt;
	bt = mirvar(SK->t);

}

/*
* Name:  DtimeHorsSign
* Input: Takes a secret key (SK generated with HorsKeygen) 
*        and a message (char * array)
* Goal:  This function will Verify the signiture for the message with PK 
*/ 

void DtimeHorsVer(key *PK, char *message, sig *theta){
	char* hash;
	big bt, val1, val2;
	bt = mirvar(PK->t);

}























/*
* Name:  HorsKeygen
* Input: length of substrings, 
* Goal:  t = numb of substrings, l is length, k is length of signitures 
		 PK and SK key pointers
*/

void HorsKeygen(int l, int k, int t, key *PK, key *SK){
	printf("Generating keys\n");
	//get input, declare variables
	sha256 sh;
	shs256_init(&sh);	
	int i, j, lbits;
	char r;
	SK->k = k; SK->t = t; SK->l = l;
	PK->k = k; PK->t = t; PK->l = l;
	SK->substring = malloc(t*sizeof(char*));
	PK->substring = malloc(t*sizeof(char*));
	lbits = l/8; //CONVERTED BYTES TO BITS
	//for each array, malloc either space for a hash or l bytes;
	for(i = 0; i < t; i++){
		SK->substring[i] = malloc(lbits); 
		PK->substring[i] = malloc(32 * sizeof(char));
		//Generate values for SK byte by byte
		//With those bytes, seed one way function for PK
		for(j = 0; j < lbits; j++){
			r = rand(); //
			SK->substring[i][j] = r; 
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
	printf("Developing Signiture\n");
	sig* theta;
	theta = malloc(sizeof(sig));
	theta->pointers = malloc(sizeof(int) * SK->k);
	theta->substring = malloc(sizeof(char*) * SK->k);
	char* hash;
	big bt;
	bt = mirvar(SK->t);
	//Use the merkel hash return as our 32 byte hash value;
	hash = getMerkelTreeRoot(message);
	//Do math for bitwise operations
	int slice, i;
	long base, maskreturn, j;
	slice = logb2(bt);
	slice--;
	base = exponentFunc(1, slice);
	//Bitwise shifiting to get pointers	
	big h1, h2;
	h1 = mirvar(0); h2 = mirvar(0);
	bytes_to_big(32, hash, h1);
	char* temp = malloc(32);
	for (i = 0; i < SK->k; ++i)
	{
		//theta->pointers[i] = i;


		sftbit(h1, (slice * -i), h2); //Shift bits 		  	
		big_to_bytes(32, h2, temp, TRUE); 		
		j = ((long) *temp) & base; //Mask the bytes with base value
		theta->pointers[i] = j;
		//printf("1 value of %d, %d\n", i, j);		
/*
		j = ((long) hash >> (slice * -i)) & base; //NEED NEW WAY TO CUT UP FOR CHAR *
		theta->pointers[i] = j;
		printf("1 value of %d, %d\n", i, j);
*/	
	}
	//Getting values from secret key that match the pointer location
	for(i = 0; i < SK->k; i++){
		theta->substring[i] = SK->substring[theta->pointers[i]]; 
	}

	return theta;
}

void HorsVer(key *PK, char *message, sig *theta){
	printf("Verifying Signiture\n");
	sha256 sh;
	shs256_init(&sh);	
	char *hash, *hashver;
	big bt, val1, val2;

	bt = mirvar(PK->t);
	val1 = mirvar(0);
	val2 = mirvar(0);
	hash = getMerkelTreeRoot(message);
	int slice, i, k;
	int *pointers;
	long base, j;
	slice = logb2(bt);
	slice--;
	base = exponentFunc(1, slice);
	//Bitwise shifting
	pointers = malloc(sizeof(int) * PK->k);
	big h1, h2;
	h1 = mirvar(0); h2 = mirvar(0);
	bytes_to_big(32, hash, h1);
	char* temp = malloc(32);
	for (i = 0; i < PK->k; ++i)
	{
		//pointers[i] = i;	

		sftbit(h1, (slice * -i), h2); //Shift bits	 		  	
		big_to_bytes(32, h2, temp, TRUE); 
		j = ((long) *temp) & base; //Mask the bytes with base value
		pointers[i] = j;
		//printf("1 value of %d, %d\n", i, j);	

/*
		j = ((long) hash >> (slice * -i)) & base; //NEED NEW WAY TO CUT UP FOR CHAR *
		pointers[i] = j;
		printf("2 value of %d, %d\n", i, j);
*/	
	}
	//Verification Step
	int verified = 1;
	printf("Comparing Signature to Public Key\n");
	for(i = 0; i < PK->k; i++){
		for(k = 0; k < (PK->l / 8); k++){ //Hashing theta to match public key
			shs256_process(&sh, theta->substring[i][k]);
		}
		hashver = malloc(32);
		shs256_hash(&sh, hashver);
		j = pointers[i];
		bytes_to_big(32, hashver, val1);          //Val 1 is f(s(prime)subj)			
		bytes_to_big(32, PK->substring[j], val2); //Val 2 is vsubisubj	

		if(mr_compare(val1, val2) != 0){
			printf("%d MISSED\n", i);
			cotnum(val1, stdout);		
			cotnum(val2, stdout);				
			verified = 0;
		}
	}
	if(verified == 1){
		printf("The message is verified\n");
	}else{
		printf("The message was NOT verified\n");
	}
}


/*
* Name:  getMerkelTreeRoot
* Input: char* array (content you want to be hashed)
* Goal:  breaks file into merkel tree using 256 bit leafs
*/

char* getMerkelTreeRoot(char *content){
	sha256 sh;
	shs256_init(&sh);
	int lengthOfText = strlen(content);
	char* hash;
	hash = malloc(32 * sizeof(char));
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
		h1 = getMerkelTreeRoot(arr1);
		h2 = getMerkelTreeRoot(arr2);
		//concatinate the two
		for(i = 0; i < 32; i++){
			shs256_process(&sh, h1[i]);
			shs256_process(&sh, h2[i]);
		}
		//hash and return
	}else{
		for(i = 0; i < lengthOfText; i++){
			shs256_process(&sh, content[i]);
		}
	}
	shs256_hash(&sh, hash);	
	/*Print the hash value
	big bt;
	bytes_to_big(32, hash, bt);
	cotnum(bt, stdout);
	*/
	return hash;
}

/*
* Name:  buildMerkelTreeRoot
* Input: char* array (content you want to be hashed)
* Goal:  breaks file into merkel tree using 256 bit leafs
*/

merkelTree* buildMerkelTreeRoot(char *content){
	sha256 sh;
	shs256_init(&sh);
	int lengthOfText = strlen(content);
	char* hash;
	hash = malloc(32 * sizeof(char));
	merkelTree *newTree;
	newTree = malloc(sizeof(merkelTree));
	int i;
	if(lengthOfText > 256){
		//split array into two seperate arrays
		int midpoint = lengthOfText/2;
		char *arr1, *arr2;
		newTree->rchild = malloc(sizeof(merkelTree));
		newTree->lchild = malloc(sizeof(merkelTree));
		arr1 = malloc(midpoint);
		arr2 = malloc(lengthOfText - midpoint); // to account for integer math
		memcpy(arr1, content, midpoint);
		memcpy(arr2, content + midpoint - 1, lengthOfText - midpoint);
		//call the merkel tree function recursivly
		newTree->rchild = getMerkelTreeRoot(arr1);
		newTree->lchild = getMerkelTreeRoot(arr2);
		//concatinate the two
		for(i = 0; i < 32; i++){
			shs256_process(&sh, newTree->hash[i]);
			shs256_process(&sh, newTree->hash[i]);
		}
		//hash and return
	}else{
		newTree->rchild = NULL;
		newTree->lchild = NULL;
		for(i = 0; i < lengthOfText; i++){
			shs256_process(&sh, content[i]);
		}
	}
	shs256_hash(&sh, newTree->hash);	
	/*Print the hash value
	big bt;
	bytes_to_big(32, hash, bt);
	cotnum(bt, stdout);
	*/
	return newTree;
}



int exponentFunc(int prev, int num){ //Always put one as prev on base
	if(num != 1){
		return (prev + exponentFunc((prev * 2), num - 1));
	}else{
		return prev;
	}
}