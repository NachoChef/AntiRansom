#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <getopt.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>


#include <jansson.h>


#include "VtFile.h"
#include "VtResponse.h"

//assuming API key set

struct CallbackData {
	int counter;
};

void search_callback(const char *resource, void *data) {
	struct CallbackData *cb_data = (struct CallbackData *) data;

	cb_data->counter++;
	printf("------------- Result %d ----------------\n", cb_data->counter);
	printf("resource: %s \n", resource);
	printf("\n");
}

/*
 *	given a file path, will hash the file and transmit to VT. 
 *	returns true is there are any VT results, otherwise false.
 */
bool VTscan(char* path)
{
	int c;
	int ret = 0;
	struct VtFileDist *file_dist;
	int repeat = 3;
	int sleep_sec = 3;
	struct CallbackData cb_data = { .counter = 0 };
	char* fhash;
	
	//make sure file is openable
	FILE* file = fopen(path, "rb");
	if (!file)	//if not opened
	{
		printf("Error: File cannot be opened.\n");
		printf("Allowing execution by default.\n")
		return false;
	}

	unsigned char hash[SHA256_DIGEST_LENGTH];	//fixes problems with unmatching hashes
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	const int bufSize = 32768;
	char* buffer = malloc(bufSize);
	int bytesRead = 0;
	
	if (!buffer) return false;

	while ((bytesRead = fread(buffer, 1, bufSize, file)))
	{
		SHA256_Update(&sha256, buffer, bytesRead);
	}
	SHA256_Final(hash, &sha256);

	sha256_hash_string(hash, fhash);	//bad naming
	fclose(file);
	free(buffer);

	file_dist = VtFileDist_new();

	ret = VtFile_rescanHash(file_scan, fhash, 0, 0, 0, NULL, false);
	if (ret) {
		printf("Error: %d \n", ret);
	}
	else {
		response = VtFile_getResponse(file_scan);
		//we're really only interested in #references for this application
		str = VtResponse_get(response);
	}
	if(response > 3){
		printf("References found!\n");
		printf("Consider further investigation of the file at:"\n);
		printf("\t %s", *path);
		return true;
	}
	else return false;

}