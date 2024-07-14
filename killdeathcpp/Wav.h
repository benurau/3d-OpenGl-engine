#pragma once
#include <xaudio2.h>

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif


struct wavHeader
{
    char chunkID[4];
    unsigned long chunkSize;
    char format[4];
    char subchunk1ID[4];
    unsigned long subchunk1Size;
    unsigned short audioFormat;
    unsigned short numChannels;
    unsigned long sampleRate;
    unsigned long byteRate;
    unsigned short blockAlign;
    unsigned short bitsPerSample;
};

typedef struct GAMESOUND {
    wavHeader header;
    XAUDIO2_BUFFER buffer;
}GAMESOUND;


struct chunk_t
{
    char ID[4];
    unsigned long size;
};

DWORD LoadWavFromFile(const char* fileName, GAMESOUND* gameSound);