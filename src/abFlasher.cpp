#include "abFlasher.h"

#include <stdint.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <regex>
#include <string>
#include <cmath>
#include <algorithm>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <cstdio>

void printUsage();
int uploadKernelModule(std::fstream& binFile, FILE * serialPort);

int main(int argc, char ** argv){
  std::regex isOption ("-.*");
  UploadMode uMode = UploadMode::KERNEL_MODULE;
  int argIndex = 1;

  if(argc < 3){
    std::cout << "insuficent arguments\n";
    printUsage();
    return 1;
  }

  // optional option
  if(std::regex_match(argv[argIndex], isOption)){
    if(std::string(argv[argIndex]) == "-k"){
      uMode = UploadMode::KERNEL_MODULE;
    }
    else if (std::string(argv[argIndex]) == "-u"){
      uMode = UploadMode::USER_PROGRAM;
    }
    else{
      printUsage();
      return 1;
    }
    argIndex++;
  }

  std::fstream binaryFile(argv[argIndex], std::fstream::in | std::fstream::binary);
  if(binaryFile.fail()){
    std::cout << "cant find binary file: " <<  argv[argIndex] << "\n";
    printUsage();
    return 1;
  }
  argIndex ++;

  // some lower level IO is required for serial port
  int serialFd = open(argv[argIndex],  O_RDWR | O_NOCTTY | O_SYNC);
  FILE * serialPort = fdopen(serialFd, "r+");
  if(errno != 0){
    std::cout << "Could not open serial port " << argv[argIndex] << "\n";
    printUsage();
    return 1;
  }

  int res = 0;
  switch(uMode){
    case UploadMode::KERNEL_MODULE:
      res = uploadKernelModule(binaryFile,serialPort);
      close(serialFd);
      break;
    case UploadMode::USER_PROGRAM:

      break;
    default:
      printUsage();
      res = 1;
  }

  return res;
}


void printUsage(){
  std::cout << "usage: abFlasher [-k|-u] <.bin file> <serial port>\n";
}

int uploadKernelModule(std::fstream& binFile, FILE * serialPort){

  binFile.seekg(0, std::fstream::end);
  uint fileLen = binFile.tellg();
  binFile.seekg(0, std::fstream::beg);
  uint pages = ceil((float)fileLen / (float)ARMBOY_PAGE_SIZE);

  char * inputLine;
  size_t lineLen = 0;
  getline(&inputLine,&lineLen,serialPort);
  if(std::string(inputLine) != "===ARMBoy===\n"){
    std::cout << "unknown serial device on serial port!\n";
    return 1;
  }
  free(inputLine);
  inputLine = NULL;
  lineLen = 0;

  //instruct the MCU to start upload process
  char messageBuff[256];
  sprintf(messageBuff,"upload %d\n", pages);
  fwrite(messageBuff, strlen(messageBuff), 1, serialPort);
  getline(&inputLine,&lineLen,serialPort);

  std::cout << "upload start\n";
  char uploadBuffer[UPLOAD_CHUNK_SIZE];
  // upload binary
  while(std::string(inputLine) == "next\n"){
    std::cout << ".";
    binFile.read(uploadBuffer,UPLOAD_CHUNK_SIZE);

    fwrite(uploadBuffer,UPLOAD_CHUNK_SIZE,1,serialPort);

    free(inputLine);
    inputLine = NULL;
    lineLen = 0;
    getline(&inputLine,&lineLen,serialPort);
    std::cout << inputLine;
  }

  if(inputLine != NULL){
    free(inputLine);
  }
  std::cout << "\nJob Done!\n";
  return 0;
}
