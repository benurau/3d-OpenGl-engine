#pragma once
#include "SDL2/SDL.h"


typedef struct WAV {
    const char* name;
    SDL_AudioSpec wavSpec;
    Uint32 wavLength;
    Uint8* wavBuffer;
};

class soundEngine {
public:

    void initialize();
    WAV loadWavFile(const char* filepath, const char* wavName);
    SDL_AudioDeviceID openAudioDevice(WAV wav);
    void playAudio(WAV wav, SDL_AudioDeviceID deviceId);
    void audioCleanup(WAV wav, SDL_AudioDeviceID deviceId);
};
