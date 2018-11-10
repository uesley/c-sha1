#include <stdio.h>
#include <stdlib.h>

#define H1 0x67452301;
#define H2 0xefcdab89;
#define H3 0x98badcfe;
#define H4 0x10325476;
#define H5 0xc3d2e1f0;

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
	int p1 = (int) (tam >> 32);
	int p2 = (int) tam;
	block[14] = p1;
	block[15] = p2;
	return block;
}

Block text2NumericArray(char * text, int tam){
	int charactere;
	int block_pos;
	int diff;
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
	int i;
	int t1, t2;
	do{ 
		block[16 + pos] = block[13 + pos] ^ block[8 + pos];
		block[16 + pos] = block[16 + pos] ^ block[2 + pos];
		block[16 + pos] = block[16 + pos] ^ block[pos];	
		// t1 = t1 << 1;
		block[16 + pos] = (block[16 + pos] >> 31) | ( block[16 + pos] << 1); 
		pos ++;
	}while((pos + 16) < 80);
	return block;
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



int main(int argc, char** argv) {
	Block block = text2NumericArray("AB",2);
	// block = addLengthAtEndOfBlock(block,2); 
	block = complete80(block);
	
	printBlock(block);
    return (EXIT_SUCCESS);
}

