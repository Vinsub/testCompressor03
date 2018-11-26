
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h> 

FILE *fileptr;
FILE *fileEncoded;
long filelen; //lunghezza file in byte
unsigned char *buffer;
long frequency[256] = { 0 }; //array contenete la frequenza dei simboli codificati a byte (256 valori) tipo char
long matrix[256][2] = { 0 };
long dictionaryMatrix[256][3] = { 0 };//contiene in [0] la key o in simbolo rappresentato, in [1] la codifica, in [2] la lunghezza della codifica in bits
long finalMatrix[256][2] = { 0 };

void encode();
void matrixValue(long frequency[256], long matrix[256][2]);
void sortedMatrix(long matrix[256][2]);
void sortedDictionaryMatrix(long dMatrix[256][3]);
void dictionary(int start, int end, long matrix[256][2], long dictionaryMatrix[256][3], int shift);
void print_bits(long x);
void print_bits_char(char x);

int main(int argc, char **argv)
{
	clock_t start = clock();

	printf("tempo inizio = %d\n", start); //stampa tempo iniziale

	if (argc >= 2)
	{
		fileptr = fopen(argv[1], "rb");
	}
	else
	{
		fileptr = fopen("C:\\Users\\Vincenzo\\source\\repos\\testCompressor03\\Debug\\files\\1.txt", "rb");
	}
	printf("argc = %d\n", argc); //stampa numero di argomenti

	for (int i = 0; i < argc; ++i) { //stampa tutti gli argomenti
		printf("argv[ %d ] = %s\n", i, argv[i]);
	}
	
	if (!fileptr)
	{
		printf("errore fopen");
		perror(fileptr);
	}
	else
	{
		encode();

	}
	clock_t end = clock();
	printf("tempo fine = %d\n", end); //stampa tempo fine
	printf("Tempo di esecuzione =  %f secondi \n", ((double)(end - start)) / CLOCKS_PER_SEC);
	/*for (int index = 0; index < 256; index++) {
		printf("pos = %d key = %d n. bits = %d codifica = ", index, dictionaryMatrix[index][0], dictionaryMatrix[index][2]);
		print_bits(dictionaryMatrix[index][1]);
	}*/
	_getch();
}
void encode() {
	fseek(fileptr, 0, SEEK_END);
	filelen = ftell(fileptr);
	rewind(fileptr);
	buffer = (unsigned char *)malloc((filelen + 1) * sizeof(unsigned char));//assegna a buffer dimensione del file(in numero di char/byte)
	printf("filelen = ");
	printf("%d\n", filelen);

	for (int index = 0; index < filelen; index++) //legge byte per byte il file ed inserisce frequenze dei byte in frequency
	{
		fread(buffer + index, 1, 1, fileptr);
		//value = *(buffer + index);
		//printf("value = %d\n", value);
		frequency[*(buffer + index)] = frequency[*(buffer + index)] + 1;
	}

	long somma = 0;

	for (int index = 0; index < 256; index++) // calcola somma frequenze
	{
		//printf("read = %d, %d\n", index, frequency[index]);
		somma = frequency[index] + somma;

	}
	printf("somma = %d\n", somma);

	matrixValue(frequency, matrix);
	//for (index = 0; index < 256; index++) // stampa frequenza dei byte
	//{
	//	printf("key = %d, frequency = %d\n", matrix[index][0], matrix[index][1]);

	//}
	sortedMatrix(matrix);
	for (int index = 0; index < 256 && matrix[index][1] != 0; index++) // stampa frequenza dei byte
	{
		printf("pos = %d key = %d, frequency = %d\n", index, matrix[index][0], matrix[index][1]);

	}



	fclose(fileptr); // chiudi file
	dictionary(0, 255, matrix, dictionaryMatrix, 31); 
	// crea matrice con ordine di numero di bit di codifica e ordine lessicografico
	sortedDictionaryMatrix(dictionaryMatrix);
	// fine crea matrice con ordine di numero di bit di codifica e ordine lessicografico
	for (int ind = 0; ind < 256; ind++) {
		printf("0 = %d 2 = %d 1 = ", dictionaryMatrix[ind][0], dictionaryMatrix[ind][2]);
		print_bits(dictionaryMatrix[ind][1]);
	}
	//controllo errori codifica dizionario
	//inserimento in finalMatrix
	for (int index = 0; index < 256; index++) {
		finalMatrix[dictionaryMatrix[index][0]][0] = dictionaryMatrix[index][2];
		finalMatrix[dictionaryMatrix[index][0]][1] = dictionaryMatrix[index][1];
		/*printf("index = %d finalMatrix_0 = %d  codifica = ", index, finalMatrix[index][0]);
		print_bits(finalMatrix[index][1]);*/
	}
	for (int index = 0; index < 256; index++) {
		if (dictionaryMatrix[index][2] != 0) {

		}
	}
	//for (int index = 0; index < 256; index++) {
		//printf("index = %d finalMatrix_0 = %d  codifica = ", index, finalMatrix[index][0]);
		//print_bits(finalMatrix[index][1]);
	//}
	// apre file codificato
	fileEncoded = fopen("C:\\Users\\Vincenzo\\source\\repos\\testCompressor03\\Debug\\files\\1_coded.txt", "a");
	char charToWrite = '0' - '0';
	int indexChar = 0;
	int byte = 0;
	//print_bits_char(charToWrite);
	//inizio codifica e salvataggio su file il dizionario
	for (int index = 0; index < 256; index++) {
		int numOfBit = finalMatrix[index][0];
		numOfBit = numOfBit + 8;
		
		
		for (int indexBits = 0; indexBits < numOfBit; indexBits++) {
			if (indexChar == 8) {
				fprintf(fileEncoded, "%c", charToWrite);
				//printf("byte = %d ", byte);
				//print_bits_char(charToWrite);
				byte++;
				indexChar = 0;
				charToWrite = '0' - '0';
			}

			if ((indexBits < 8)&&(indexChar!=8)) {
				unsigned char charAppoggio = finalMatrix[index][0] >> 7-indexBits;
				//print_bits(finalMatrix[index][0]);
				//print_bits(charAppoggio);
				//print_bits_char(charAppoggio);
				charAppoggio = charAppoggio << 7;
				//print_bits(charAppoggio);
				charAppoggio = charAppoggio >> indexChar;
				//print_bits(charAppoggio);
				//print_bits_char(charAppoggio);
				charToWrite = charToWrite | charAppoggio;
				indexChar++;
				
			}
			else if (indexChar!=8){
				unsigned char charAppoggio = finalMatrix[index][1] >> (39 - indexBits);
				//print_bits(finalMatrix[index][1]);
				//print_bits(charAppoggio);
				//print_bits_char(charAppoggio);
				charAppoggio = charAppoggio << 7;
				//print_bits(charAppoggio);
				charAppoggio = charAppoggio >> indexChar;
				//print_bits(charAppoggio);
				//print_bits_char(charAppoggio);
				charToWrite = charToWrite | charAppoggio;
				indexChar++;
			}
		}
	}
	//fine codifica e salvataggio su file il dizionario
	//inizio codifica del file e salvataggio
	charToWrite = '0' - '0';
	indexChar = 0;
	for (int index = 0; index < filelen; index++) //legge byte per byte il file 
	{
		unsigned char charToEncode = buffer[index];
		int numOfBit = finalMatrix[*(buffer + index)][0];
		//value = *(buffer + index);
		//printf("value = %d\n", charToEncode);
		//printf("numOfBit = %d\n", numOfBit);
		for (int index2 = 0; index2 < numOfBit; index2++) 
		{
			if (indexChar == 8) 
			{
				fprintf(fileEncoded, "%c", charToWrite);
				//print_bits_char(charToWrite);
				indexChar = 0;
				charToWrite = '0' - '0';
			}
			unsigned char charAppoggio = finalMatrix[*(buffer + index)][1] >> (31 - index2);
			charAppoggio = charAppoggio << 7;
			charAppoggio = charAppoggio >> indexChar;
			charToWrite = charToWrite | charAppoggio;
			indexChar++;
			if (index2 == (numOfBit - 1) && index == (filelen - 1) && indexChar < 8)
			{//scrive tutti 0 alla fine dell'ultimo byte non completo
				unsigned char lastByte = 8 - indexChar;//assegna ad ultimo byte numero di bit non utili per penultimo byte
				for (int index3 = index2; index3 < 8; index3++)
				{
					unsigned char charAppoggio = '0'-'0';
					charAppoggio = charAppoggio << 7;
					charAppoggio = charAppoggio >> indexChar;
					charToWrite = charToWrite | charAppoggio;
					indexChar++;
				}
				fprintf(fileEncoded, "%c", charToWrite);
				fprintf(fileEncoded, "%c", lastByte);
			}
		}


		//frequency[*(buffer + index)] = frequency[*(buffer + index)] + 1;
	}

	fclose(fileEncoded);



}

void matrixValue(long frequency[256], long matrix[256][2]) {
	for (int index = 0; index < 256; index++) {//popola matrice tipo dizionario con campo 0 come chiave, campo 1 come ricorrenze
		matrix[index][0] = index;
		matrix[index][1] = frequency[index];
		//printf("key = %d, frequency = %d\n", matrix[index][0], matrix[index][1]);
	}
}

void sortedMatrix(long matrix[256][2]) {
	//long sMatrix[256][2];
	int i, j;
	int key;
	long value;
	for (i = 1; i < 256; i++) {
		key = matrix[i][0];
		value = matrix[i][1];
		for (j = i - 1; (j >= 0) && (matrix[j][1] < value); j--) {
			matrix[j + 1][0] = matrix[j][0];
			matrix[j + 1][1] = matrix[j][1];
		}
		matrix[j + 1][0] = key;
		matrix[j + 1][1] = value;
	}
}
// crea matrice con ordine di numero di bit di codifica e ordine lessicografico
void sortedDictionaryMatrix(long dictionaryMatrix[256][3]) {
	int i, j;
	int key = 0;
	long value = 0;
	long nBit = 0;
	for (i = 1; i < 256; i++) {
		printf("i = %d \n", i);
		key = dictionaryMatrix[i][0];
		printf("0 = %d ", key);
		value = dictionaryMatrix[i][1];
		printf("1 = %d ", value);
		nBit = dictionaryMatrix[i][2];
		printf("2 = %d \n", nBit);
		for (j = i - 1; (j >= 0) && ((dictionaryMatrix[j][2] > nBit) || (dictionaryMatrix[j][2] == nBit && dictionaryMatrix[j][0] > key)); j--) {
			dictionaryMatrix[j + 1][0] = dictionaryMatrix[j][0];
			dictionaryMatrix[j + 1][1] = dictionaryMatrix[j][1];
			dictionaryMatrix[j + 1][2] = dictionaryMatrix[j][2];
		}
		dictionaryMatrix[j + 1][0] = key;
		dictionaryMatrix[j + 1][1] = value;
		dictionaryMatrix[j + 1][2] = nBit;
		key = dictionaryMatrix[i][0];
		printf("0 = %d ", key);
		value = dictionaryMatrix[i][1];
		printf("1 = %d ", value);
		nBit = dictionaryMatrix[i][2];
		printf("2 = %d \n", nBit);
	}
}// fine crea matrice con ordine di numero di bit di codifica e ordine lessicografico

void dictionary(int start, int end, long matrix[256][2], long dictionaryMatrix[256][3], int shift) {
	int index = start;
	long len = 0;
	for (int indexLen = start; indexLen <= end; indexLen++) {
		len = len + matrix[indexLen][1];
	}
	long halfLength = len / 2;
	long sumValue = 0;
	halfLength = floor(halfLength);
	//printf("index = %d end = %d shift = %d \n", start, end, shift);
	if (end - start == 0) {
		return;
	}
	if (end - start == 1) {
		dictionaryMatrix[start][0] = matrix[start][0];
		long dicValue = 0 - 0;
		dicValue = dicValue << shift - dictionaryMatrix[start][2];
		dictionaryMatrix[start][1] = dictionaryMatrix[start][1] | dicValue;
		dictionaryMatrix[start][2] = dictionaryMatrix[start][2] + 1;
		//print_bits(dictionaryMatrix[start][1]);
		dicValue = 1 - 0;
		dicValue = dicValue << shift - dictionaryMatrix[end][2];
		dictionaryMatrix[end][1] = dictionaryMatrix[end][1] | dicValue;
		dictionaryMatrix[end][2] = dictionaryMatrix[end][2] + 1;
		//print_bits(dictionaryMatrix[end][1]);
		return;
	}
	
	while (sumValue < halfLength) {
		sumValue = sumValue + matrix[index][1];
		dictionaryMatrix[index][0] = matrix[index][0];
		long dicValue = 0-0;
		dicValue = dicValue << shift - dictionaryMatrix[index][2];
		dictionaryMatrix[index][1] = dictionaryMatrix[index][1] | dicValue;
		dictionaryMatrix[index][2] = dictionaryMatrix[index][2] + 1;
		//print_bits(dictionaryMatrix[index][1]);
		index++;
	}
	int tempIndex = index -1;
	
	while ((matrix[index][1] != 0)&&(index<=end)) {
		dictionaryMatrix[index][0] = matrix[index][0];
		long dicValue = 1;
		dicValue = dicValue << shift - dictionaryMatrix[index][2];
		dictionaryMatrix[index][1] = dictionaryMatrix[index][1] | dicValue;
		dictionaryMatrix[index][2] = dictionaryMatrix[index][2] + 1;
		//print_bits(dictionaryMatrix[index][1]);
		index++;
	}
	int tempIndexend = index - 1;
	//ricorsione sulle 2 metà appena ciclate
	if (tempIndex - start > 0) {
		dictionary(start, tempIndex, matrix, dictionaryMatrix, shift);
	}
	if (tempIndexend - tempIndex - 1 > 0) {
		dictionary(tempIndex + 1, tempIndexend, matrix, dictionaryMatrix, shift);
	}

}

void print_bits(long x)
{
	int i;
	for (i = 8 * sizeof(x) - 1; i >= 0; i--) {
		(x & (1 << i)) ? putchar('1') : putchar('0');
	}
	printf("\n");
}
void print_bits_char(char x)
{
	int i;
	for (i = 8 * sizeof(x) - 1; i >= 0; i--) {
		(x & (1 << i)) ? putchar('1') : putchar('0');
	}
	printf("\n");
}