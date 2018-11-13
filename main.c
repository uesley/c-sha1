#include <stdio.h>
#include <stdlib.h>

#define _message "CRYPTO";

typedef unsigned int Word;
typedef Word * Block;

Word * createBlock(){
	Word * block = malloc(sizeof(Word) * 80);
	int i;
	for (i = 0; i < 80; i++){
		block[i] = 0;
	}
	return block;
}

Block addLengthAtEndOfBlock(Block block, unsigned long int tam){
	tam = tam * 8;
	int p1 = (unsigned int) (tam >> 32);
	int p2 = (unsigned int) tam;
	block[14] = p1;
	block[15] = p2;
	return block;
}

Block text2NumericArray(char * text, int tam){
	int charactere;
	int block_pos;
	unsigned int diff;
	int over = 1;
	Word word;
	Block block = (Block) createBlock();

	for (charactere = 0; charactere < tam; charactere ++){
		word = 0;
		block_pos = charactere / 4;
		do{
			word = word << 8;
			word = word + (int) text[charactere]; 
			charactere ++;
		}while(charactere % 4);
		block[block_pos] = word;
	}

	diff = 4 - tam % 4;
	over = over << (diff * 8 -1);
	if (!diff){
		block_pos += 1;
	}
	block[block_pos] += over;

	addLengthAtEndOfBlock(block, tam);

	return block;
}

Block complete80(Block block){
	int pos = 0;
	do{ 
		block[16 + pos] = block[13 + pos] ^ block[8 + pos];
		block[16 + pos] = block[16 + pos] ^ block[2 + pos];
		block[16 + pos] = block[16 + pos] ^ block[pos];
		block[16 + pos] = (block[16 + pos] >> 31) | ( block[16 + pos] << 1); 
		pos ++;
	}while((pos + 16) < 80);
	return block;
}

int Choose(unsigned int x, unsigned int y, unsigned int z){
	return (x & y) ^ (~x & z);
}

int Parity(unsigned int x, unsigned int y, unsigned int z){
	return (x ^ y) ^ z;
}

int Major(unsigned int x, unsigned int y, unsigned int z){
	return (x & y) ^ (x & z) ^ (y & z);
}

int f_t(int t, unsigned int x, unsigned int y, unsigned int z){
	if (t < 20){
		return Choose(x, y, z);
	}
	if (t < 40){
		return Parity(x, y, z);
	}
	if (t < 60){
		Major(x, y, z);
	}
	return Parity(x, y, z);
}

int K_t(int t){
	if (t < 20)
		return 0x5a827999;
	if (t < 40)
		return 0x6ed9eba1;
	if (t < 60)
		return 0x8f1bbcdc;
	return 0xca62c1d6;			
} 

unsigned int * intitializeH(){
	unsigned int * H = malloc(sizeof(unsigned int)*5);
	H[0] = 0x67452301;
	H[1] = 0xefcdab89;
	H[2] = 0x98badcfe;
	H[3] = 0x10325476;
	H[4] = 0xc3d2e1f0;
	return H;
}

unsigned int * computeHsValues(Block block, unsigned int * H){
	unsigned int T;
	unsigned int a = H[0];
	unsigned int b = H[1];
	unsigned int c = H[2];
	unsigned int d = H[3];
	unsigned int e = H[4];
	int t;

	for (t = 0; t < 80; t++){		
		T  = (a >> 27) | (a << 5); // rotate left 5 bits
		T  = T + f_t(t, b, c, d);
		T  = T + e;
		T  = T + K_t(t);
		T  = T + block[t];
		e  = d;
		d  = c;
		c  = (b >> 2) | (b << 30); // rotate left 30 bits  /// testar isso
		b  = a;
		a  = T;
		printf("[%i] :  a: %u , b: %u , c: %u , d: %u , e: %u \n", t, a, b, c, d, e);

	}
	H[0] += a;
	H[1] += b;
	H[2] += c;
	H[3] += d;
	H[4] += e;

	return H;
}


void printBlock(Block block){
	// Block block = b;
	int i;
	// \n[ 0] : 1094877184	
	printf("\nBlock: \n");
	for (i = 0; i < 80; i++){
		printf("[%2i] : %08u\n", i, block[i]);
	}
}

void printDigest(unsigned int * H){
	int i;
	for(i = 0; i < 5; i++)
		printf("%x",H[i]);
	printf("\n");	
}


int main(int argc, char** argv) {
	Block block = text2NumericArray("CRYPTO",6);
	// block = addLengthAtEndOfBlock(block,2); 
	block = complete80(block);
	
	// printBlock(block);
	// printf("\n\nDigest: ");	
	
	unsigned int * H = intitializeH(); 
	H = computeHsValues(block, H);
	
	printf("\nDigest: ");
	printDigest(H);

    return (EXIT_SUCCESS);
}

