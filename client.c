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

typedef struct sig sig;
typedef struct key key;

/* DECLARATIONS */
void error(const char *msg) { perror(msg); exit(0); } // Error function used for reporting issues
char* buildMerkelTree(char *);
void HorsKeygen(int, int, int, key *, key *);
sig* HorsSign(key *, char *);
void HorsVer(key *, char *, sig *);
void DtimeHorsKeygen(int, int, int, key *, key *);
sig* DtimeHorsSign(key *, char *);
void DtimeHorsVer(key *, char *, sig *);
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
	miracl *mip = mirsys(100, 10);	
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

	buildMerkelTree(content);


	struct key PK, SK;
	struct sig *theta;
	HorsKeygen(80, 16, 1024, &PK, &SK);
	theta = HorsSign(&SK, content);
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
	//get input, declare variables
	sha256 sh;
	shs256_init(&sh);	
	int i, j, r, lbits;
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
			r = rand(); 
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
	sig* theta;
	theta = malloc(sizeof(sig));
	theta->pointers = malloc(sizeof(int) * SK->k);
	theta->substring = malloc(sizeof(char*) * SK->k);
	char* hash;
	big bt;
	bt = mirvar(SK->t);
	//Use the merkel hash return as our 32 byte hash value;
	hash = buildMerkelTree(message);
	//Do math for bitwise operations
	int slice, i, base, j;
	slice = logb2(bt);
	slice--;
	base = exponentFunc(1, slice);
	//Bitwise shifiting to get pointers
	for (i = 0; i < SK->k; ++i)
	{
		j = ((int) hash >> (slice * i)) & base; //NEED NEW WAY TO CUT UP FOR CHAR *
		theta->pointers[i] = j;
		printf("1 value of %d, %d\n", i, j);		
	}
	//Getting values from secret key that match the pointer location
	for(i = 0; i < SK->k; i++){
		theta->substring[i] = SK->substring[theta->pointers[i]]; 
		printf("%d == %d\n", theta->substring[i], SK->substring[theta->pointers[i]]);		
	}

	return theta;
}

void HorsVer(key *PK, char *message, sig *theta){
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
	for (i = 0; i < PK->k; ++i)
	{
		j = ((int) hash >> (slice * i)) & base; //NEED NEW WAY TO CUT UP FOR CHAR *
		pointers[i] = j;
		printf("2 value of %d, %d\n", i, j);
	}
	//Verification Step
	int verified = 1;
	printf("about to do some comparisions\n");
	printf("%d == %d", theta->substring[0], PK->substring[pointers[0]]);
	for(i = 0; i < PK->k; i++){
		j = pointers[i];
		bytes_to_big(32, theta->substring[i], val1); 		
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
		h1 = buildMerkelTree(arr1);
		h2 = buildMerkelTree(arr2);
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

int exponentFunc(int prev, int num){ //Always put one as prev on base
	if(num != 1){
		return (prev + exponentFunc((prev * 2), num - 1));
	}else{
		return prev;
	}
}