#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <stdlib.h>

#define BUFFER_SIZE 64

int main(int argc, char** argv)
{
    char output_file_name[128];    //You may assume that the length of the output file name will not exceed 128 characters.
    if(argc != 3) { // First param is command name, so we should have 3 total
        printf("Expected usage: filesec -[e|d] [filename]\n");
                printf("Received %d params\n", argc);
                return -1;
    }

    // Set output_file_name to whatever the user provided
    strcpy(output_file_name, argv[2]); 
    int result = 0;
    if(argv[1] == "-e") {
        printf("Encoding %s\n", output_file_name);
        result = processFileByPaths(output_file_name, false);
    } else if (argv[1] == "-d") {
        printf("Decoding %s\n", output_file_name);
        result = processFileByPaths(output_file_name, true);
    } else {
        printf("Unknown flag: %s\n", argv[1]);
        return -1;
    }
    return 0;
}

// Given an input path and a boolean (true to decode, false to encode):
// 1. Figure out the proper output path (foo.abc -> foo_dec.abc or foobar.txt -> foobar_enc.txt)
// 2. Open the input file, return -1 if an error occurs
// 3. Open the output file (or create if it doesn't exist), return -1 if an error occurs
// 4. Call processFile with the file descriptors
int processFileByPaths(const char path[], bool decode) { // The path itself includes the file extension
    char *extension = strrchr(path, '.');
    if(extension == NULL) {
        printf("Invalid file path: %s\n", path);
        return -1;
    }

    char *strippedPath = malloc(strlen(path) - strlen(extension));
    strncpy(strippedPath, path, strlen(path) - strlen(extension));
    strippedPath[strlen(path) - strlen(extension)] = '\0';

    // We now have the path without the extension

    char *outputPath = malloc(strlen(strippedPath) + 5);
    strcpy(outputPath, strippedPath);
    strcat(outputPath, decode ? "_dec" : "_enc");
    strcat(outputPath, extension);

    int inputFile = open(path, O_RDONLY);
    if(inputFile == -1) {
        printf("Error opening input file: %s\n", path);
        return -1;
    }

    int outputFile = open(outputPath, O_WRONLY | O_CREAT, 0666);
    if(outputFile == -1) {
        printf("Error opening output file: %s\n", outputPath);
        return -1;
    }

    return processFile(inputFile, outputFile);
}

// Give two file descriptors, one for input and one for output:
// 1. Read up to BUFFER_SIZE bytes from the input file
// 2. For each char, add 1 to it if encoding, subtract 1 if decoding
// 3. Write the result to the output file 

int processFile(int fileIndex, int outputIndex, bool decode) {
    char buffer[BUFFER_SIZE];
    int bytesRead = read(fileIndex, buffer, BUFFER_SIZE);
    while(bytesRead > 0) {
        for(int i = 0; i < bytesRead; i++) {
            buffer[i] += decode ? -1 : 1;
        }
        write(outputIndex, buffer, bytesRead);
        bytesRead = read(fileIndex, buffer, BUFFER_SIZE);
    }
    return 0;
}
