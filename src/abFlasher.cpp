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

/****************************************
  quick N dirty flash tool for the ARMBoy tm.
  can flash firmware (kernel modules) and user software.
*****************************************/

void printUsage();
int uploadKernelModule(std::fstream& binFile, FILE * serialPort);
int uploadUserProgram(std::fstream& binFile, FILE * serialPort, uint32_t heapSize, uint32_t stackSize);

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

  uint heapSize = 0;
  uint stackSize = 0;
  if(uMode == UploadMode::USER_PROGRAM){
    sscanf(argv[argIndex], "%u", &heapSize);
    argIndex ++;
    sscanf(argv[argIndex], "%u", &stackSize);
    argIndex ++;
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
      res = uploadUserProgram(binaryFile,serialPort, heapSize, stackSize);
      close(serialFd);
      break;
    default:
      printUsage();
      res = 1;
  }

  return res;
}

const uint32_t CSUM_BUFFER_SIZE = 256;
uint32_t calcChecksum(std::fstream& binFile){
  uint32_t checksum = 0;
  char buffer[CSUM_BUFFER_SIZE];
  memset(buffer,0,CSUM_BUFFER_SIZE);
  binFile.read(buffer,CSUM_BUFFER_SIZE);
  while(binFile.gcount() != 0){
    for(int i =0; i < CSUM_BUFFER_SIZE/4; i++){
      checksum = checksum + *((uint32_t*)buffer + i);
    }

    memset(buffer,0,CSUM_BUFFER_SIZE);
    binFile.read(buffer,CSUM_BUFFER_SIZE);
  }

  return checksum;
}

bool checkDeviceOnPort(FILE * serialPort){
  char * inputLine;
  size_t lineLen = 0;
  getline(&inputLine,&lineLen,serialPort);
  if(std::string(inputLine) != "===ARMBoy===\n"){
    return false;
  }
  free(inputLine);
  return true;
}

void printUsage(){
  std::cout << "usage: abFlasher -k <.bin file> <serial port>\n"
            << "usage: abFlasher -u <heapSize> <stackSize> <.bin file> <serialPort>";
}

int uploadKernelModule(std::fstream& binFile, FILE * serialPort){

  binFile.seekg(0, std::fstream::end);
  uint fileLen = binFile.tellg();
  binFile.seekg(0, std::fstream::beg);
  uint pages = ceil((float)fileLen / (float)ARMBOY_PAGE_SIZE);

  uint32_t checksum = calcChecksum(binFile);
  binFile.clear();
  binFile.seekg(0, std::fstream::beg);

  if(!checkDeviceOnPort(serialPort)){
    std::cout << "unknown device on serial port! \n";
    return 1;
  }

  char * inputLine;
  size_t lineLen = 0;

  //instruct the MCU to start upload process
  char messageBuff[256];
  sprintf(messageBuff,"upload %d %u\n", pages, checksum);
  fwrite(messageBuff, strlen(messageBuff), 1, serialPort);
  getline(&inputLine,&lineLen,serialPort);

  std::cout << "upload start w/ checksum: " << std::hex << checksum << "\nuploading";
  char uploadBuffer[UPLOAD_CHUNK_SIZE];
  // upload binary
  while(std::string(inputLine) == "next\n"){
    std::cout << ".";

    memset(uploadBuffer, 0, UPLOAD_CHUNK_SIZE);
    binFile.read(uploadBuffer,UPLOAD_CHUNK_SIZE);

    fwrite(uploadBuffer,UPLOAD_CHUNK_SIZE,1,serialPort);

    free(inputLine);
    inputLine = NULL;
    lineLen = 0;
    getline(&inputLine,&lineLen,serialPort);
  }
  std::cout << inputLine;

  if(inputLine != NULL){
    free(inputLine);
  }
  return 0;
}

int uploadUserProgram(std::fstream& binFile, FILE * serialPort, uint32_t heapSize, uint32_t stackSize){
  binFile.seekg(0, std::fstream::end);
  uint fileLen = binFile.tellg();
  binFile.seekg(0, std::fstream::beg);
  uint chunks = ceil((float)fileLen / (float)UPLOAD_CHUNK_SIZE);

  uint32_t checksum = calcChecksum(binFile);
  binFile.clear();
  binFile.seekg(0, std::fstream::beg);

  if(!checkDeviceOnPort(serialPort)){
    std::cout << "unknown device on serial port! \n";
    return 1;
  }

  //instruct the MCU to start upload process
  char messageBuff[256];
  sprintf(messageBuff,"u_upload %d %d %d %u\n", chunks*64, heapSize, stackSize, checksum);
  fwrite(messageBuff, strlen(messageBuff), 1, serialPort);

  char * inputLine;
  size_t lineLen = 0;
  getline(&inputLine,&lineLen,serialPort);

  std::cout << "upload start w/ checksum: " << std::hex << checksum << "\nuploading";
  char buffer[UPLOAD_CHUNK_SIZE];

  while(std::string(inputLine) == "next\n"){
    std::cout << ".";

    memset(buffer, 0, UPLOAD_CHUNK_SIZE);
    binFile.read(buffer,UPLOAD_CHUNK_SIZE);

    fwrite(buffer,UPLOAD_CHUNK_SIZE,1,serialPort);

    free(inputLine);
    inputLine = NULL;
    lineLen = 0;
    getline(&inputLine,&lineLen,serialPort);
  }
  std::cout << inputLine;

  if(inputLine != NULL){
    free(inputLine);
  }
  return 0;
}
