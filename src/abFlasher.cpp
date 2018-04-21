#include "abFlasher.h"

#include <stdint.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <regex>
#include <string>
#include <cmath>
#include <algorithm>

void printUsage();
int uploadKernelModule(std::string name, uint32_t id, uint32_t hSize, std::fstream& binFile, std::fstream& serialPort);

int main(int argc, char ** argv){
  std::regex isOption ("-.*");
  UploadMode uMode = UploadMode::KERNEL_MODULE;
  int argIndex = 1;

  if(argc < 5){
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

  std::string progName(argv[argIndex]);
  argIndex++;

  if(progName.length() > NAME_SIZE_LIMIT){
    std::cout << "program name to long! max 60 characters\n";
    printUsage();
    return 1;
  }

  uint32_t progId = 0;
  std::stringstream(argv[argIndex]) >> progId;
  argIndex++;

  uint32_t heapSize = 0;
  std::stringstream(argv[argIndex]) >> heapSize;
  argIndex++;

  std::fstream binaryFile(argv[argIndex], std::fstream::in);
  if(binaryFile.fail()){
    std::cout << "cant find binary file: " <<  argv[argIndex] << "\n";
    printUsage();
    return 1;
  }
  argIndex ++;

  std::fstream serialPort(argv[argIndex]);
  if(serialPort.fail()){
    std::cout << "cannot open port: " << argv[argIndex] << "\n";
    printUsage();
    return 1;
  }

  switch(uMode){
    case UploadMode::KERNEL_MODULE:
      return uploadKernelModule(progName,progId,heapSize,binaryFile,serialPort);
    case UploadMode::USER_PROGRAM:

      break;
    default:
      printUsage();
      return 1;
  }

  return 0;
}


void printUsage(){
  std::cout << "usage: abFlasher [-k|-u] <program name> <program id> <heap size bytes> <.bin file> <serial port>\n";
}

void generateModuleHeader(char * hBuffer, std::string name, uint32_t id, uint32_t hSize, std::fstream& binFile);
int uploadKernelModule(std::string name, uint32_t id, uint32_t hSize, std::fstream& binFile, std::fstream& serialPort){

  binFile.seekg(0, std::fstream::end);
  uint fileLen = binFile.tellg();
  binFile.seekg(0, std::fstream::beg);
  uint pages = ceil((float)fileLen / (float)ARMBOY_PAGE_SIZE) + 1; // + 1 because of header page

  std::string greating = "";
  serialPort >> greating;
  if(greating != "===ARMBoy==="){
    std::cout << "unknown serial device on serial port\n";
    return 1;
  }

  serialPort << "upload " << pages;

  std::string response = "";
  serialPort >> response;

  char uploadBuffer[UPLOAD_CHUNK_SIZE];
  generateModuleHeader(uploadBuffer, name, id, hSize, binFile);
  for(int i =0; i < ARMBOY_PAGE_SIZE/UPLOAD_CHUNK_SIZE; i ++){

  }

  // upload binary
  while(response == "next"){
    binFile.read(uploadBuffer,UPLOAD_CHUNK_SIZE);
    serialPort.write(uploadBuffer,UPLOAD_CHUNK_SIZE);
    serialPort << "\n";
    serialPort >> response;
  }

  return 0;
}

void generateModuleHeader(char * hBuffer, std::string name, uint32_t id, uint32_t hSize, std::fstream& binFile){
  memcpy(hBuffer, name.c_str(), std::min(60UL,name.length()));
}
