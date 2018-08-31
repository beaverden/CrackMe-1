#include "rc4.h"

//https://gist.github.com/rverton/a44fc8ca67ab9ec32089

void swap(unsigned char *a, unsigned char *b) 
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

int KSA(unsigned char *key, unsigned int keyLen, unsigned char *S) 
{

    int j = 0;

    for (int i = 0; i < N; i++)
        S[i] = i;

    for (int i = 0; i < N; i++) {
        j = (j + S[i] + key[i % keyLen]) % N;

        swap(&S[i], &S[j]);
    }

    return 0;
}

int PRGA(unsigned char *S, unsigned char *buffer, unsigned int len) 
{

    int i = 0;
    int j = 0;

    for (size_t n = 0; n < len; n++) {
        i = (i + 1) % N;
        j = (j + S[i]) % N;

        swap(&S[i], &S[j]);
        int rnd = S[(S[i] + S[j]) % N];

        buffer[n] = rnd ^ buffer[n];

    }

    return 0;
}

int RC4(unsigned char *key, unsigned int keyLen, unsigned char* buff, unsigned int buffLen) {

    unsigned char S[N];
    KSA(key, keyLen, S);

    PRGA(S, buff, buffLen);

    return 0;
}