#pragma once

#include <iostream>
#include <cstdio>
#include <cmath>
#include "Wav.h"




DWORD LoadWavFromFile(const char* fileName, GAMESOUND* gameSound){

    DWORD Error = ERROR_SUCCESS;

    BOOL dataChunkFound = FALSE;

    FILE* wavFile;

    BYTE* buffer = {0};

    int sample_size = 0;
    int samples_count = 0;
    short int* value = 0;

    if ((wavFile = fopen(fileName, "rb")) == 0) {
        Error = GetLastError();
        goto Exit;
    };

    fread(&gameSound->header, sizeof(gameSound->header), 1, wavFile);


    printf("WAV File Header read:\n");
    printf("File Type: %s\n", gameSound->header.chunkID);
    printf("File Size: %ld\n", gameSound->header.chunkSize);
    printf("WAV Marker: %s\n", gameSound->header.format);
    printf("Format Name: %s\n", gameSound->header.subchunk1ID);
    printf("Format Length: %ld\n", gameSound->header.subchunk1Size);
    printf("Format Type: %hd\n", gameSound->header.audioFormat);
    printf("Number of Channels: %hd\n", gameSound->header.numChannels);
    printf("Sample Rate: %ld\n", gameSound->header.sampleRate);
    printf("Sample Rate * Bits/Sample * Channels / 8: %ld\n", gameSound->header.byteRate);
    printf("Bits per Sample * Channels / 8.1: %hd\n", gameSound->header.blockAlign);
    printf("Bits per Sample: %hd\n", gameSound->header.bitsPerSample);

    

    chunk_t chunk;
    printf("id\t" "size\n");

    while (true)
    {
        fread(&chunk, sizeof(chunk), 1, wavFile);
        printf("%c%c%c%c\t" "%li\n", chunk.ID[0], chunk.ID[1], chunk.ID[2], chunk.ID[3], chunk.size);
        if (*(unsigned int*)&chunk.ID == 0x61746164)
            break;

        fseek(wavFile, chunk.size, SEEK_CUR);
    }


    sample_size = gameSound->header.bitsPerSample / 8;
    samples_count = chunk.size * 8 / gameSound->header.bitsPerSample;
    printf("Samples count = %i\n", samples_count);

    value = new short int[samples_count];
    memset(value, 0, sizeof(short int) * samples_count);

    buffer = new BYTE[gameSound->header.chunkSize];
    

    for (int i = 0; i < gameSound->header.chunkSize; i++)
    {
        fread(&buffer[i], sample_size, 1, wavFile);
    }

    gameSound->buffer.pAudioData = buffer;
    

    for (int i = 0; i < samples_count; i++)
    {
        printf("%i \n", gameSound->buffer.pAudioData[i]);
    }
    

Exit:
    return(Error);
}
