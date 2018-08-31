#pragma once
#include "string.h"
#define N 256   // 2^8

void swap(unsigned char *a, unsigned char *b);
int KSA(unsigned char *key, unsigned int keyLen, unsigned char *S);
int PRGA(unsigned char *S, unsigned char *buffer, unsigned int len);
int RC4(unsigned char *key, unsigned int keyLen, unsigned char* buff, unsigned int buffLen);
