// System Programming, 2022, DGIST
#include <stdio.h>
#include "lib_fastlz/fastlz.h"
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>

#pragma pack(push, 1)
typedef struct _Meta {
	char* name;
	int size;
} Meta;
#pragma pack(pop)

int main(int argc, char* argv[]) {
	// compression
	#ifdef COMP
	char* resultFile = argv[1];
	int n = 0, len = 0, fileSize = 0, resultSize = 0, totalFileSize = 0;
	Meta* metaList[argc-2];
	while (n < (argc - 2)){
		len = strlen(argv[2+n]);
		char* bufferptr = (char*)malloc(sizeof(char) * (len + 1));
		strcpy(bufferptr, argv[2+n]);
		metaList[n] = malloc(sizeof(Meta));
		memset(metaList[n], 0, sizeof(Meta));
		metaList[n]->name = bufferptr;
		FILE* fp = fopen(bufferptr, "rb");
		fseek(fp, 0, SEEK_END);
		fileSize = ftell(fp);
		metaList[n]->size = fileSize;
		totalFileSize += fileSize;
		fclose(fp);
		n++;
	}
	n = 0;
	unsigned char originalDataSet[totalFileSize];
	totalFileSize = 0;
	while (n < (argc - 2)){
		unsigned char* origBuf = (unsigned char*)malloc(sizeof(char) * (metaList[n]->size));
		FILE* fp = fopen(metaList[n]->name, "rb");
		fread(origBuf, metaList[n]->size, 1, fp);
		memcpy((originalDataSet + totalFileSize), origBuf, metaList[n]->size);
		totalFileSize += metaList[n]->size;
		fclose(fp);
		n++;
	}
	unsigned char* compBuf = (unsigned char*)malloc(totalFileSize+(totalFileSize/10));
	resultSize = fastlz_compress(originalDataSet, totalFileSize, compBuf);

	FILE* resultFp = fopen(resultFile, "wb");
	fwrite(compBuf, resultSize, 1, resultFp);
	fclose(resultFp);

	FILE* metaFp = fopen("metadata.txt", "w");
	for(int i = 0; i < (argc - 2); i++){
		fprintf(metaFp, "%s\n%d\n", metaList[i]->name, metaList[i]->size);
		free(metaList[i]);
	}

	fclose(metaFp);
	free(compBuf);
	printf("%s\n", "Compression is done, metadata are in metadata.txt");
	#endif

	//decompression	
	#ifdef DECOMP
		char* targetFile = argv[1];
		FILE* metaFp = fopen("metadata.txt", "r");
		char check;
		int line = 0, fileSize = 0, resultSize = 0;
		while((check = fgetc(metaFp)) != EOF){
			if(check == '\n'){
				line++;
			}
		}
		printf("%s %d\n", "# of source files: ", line/2);
		Meta* metaList[(line/2)];
		fseek(metaFp, 0, SEEK_SET);
		for(int i = 0; i < (line/2); i++){
			metaList[i] = malloc(sizeof(Meta));
			memset(metaList[i], 0, sizeof(Meta));
			metaList[i]->name = malloc(sizeof(char) * 255);
			fscanf(metaFp, "%s\n", metaList[i]->name);
			fscanf(metaFp, "%d\n", &(metaList[i]->size));
		}
		fclose(metaFp);
		
		//mkdir
		DIR* dp;
		if((dp = opendir(argv[2])) == NULL){
			mkdir(argv[2], S_IRWXU);
		}
		closedir(dp);
		
		//decomp
		FILE* targetFp = fopen(targetFile, "rb");
		fseek(targetFp, 0, SEEK_END);
		fileSize = ftell(targetFp);
		fseek(targetFp, 0 , SEEK_SET);
		unsigned char compBuf[fileSize];
		fread(compBuf, fileSize, 1, targetFp);
		fclose(targetFp);
		unsigned char decompBuf[4194304];
		resultSize = fastlz_decompress(compBuf, fileSize, decompBuf, 4194304);
		int accumSize = 0;
		for(int i = 0; i < (line/2); i++){
			char* path = malloc(sizeof(char) * 255);
			strcpy(path, "./out/");
			strcat(path, metaList[i]->name);
			FILE* outFp = fopen(path, "wb");
			printf("%s %s\n", "reading: ", metaList[i]->name);
			fwrite(decompBuf, metaList[i]->size, 1, outFp);
			char* nextBuf = decompBuf + (metaList[i]->size);
			accumSize += metaList[i]->size;
			memmove(decompBuf, nextBuf, resultSize-accumSize);
			free(path);
		}
		
		for(int i = 0; i < (line/2); i++){
			free(metaList[i]->name);
		}
		for(int i = 0; i < (line/2); i++){
			free(metaList[i]);
		}
		printf("%s\n", "Decompression is done");
	#endif

	return 0;
}
