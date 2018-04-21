#ifndef AB_FLASHER_H_
#define AB_FLASHER_H_

enum UploadMode{
  KERNEL_MODULE,
  USER_PROGRAM
};

const int NAME_SIZE_LIMIT = 60;
const int ARMBOY_PAGE_SIZE = 256;
const int UPLOAD_CHUNK_SIZE = 64;

#endif /*AB_FLASHER_H_*/
