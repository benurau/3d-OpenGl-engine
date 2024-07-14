#include "soundengine.h"
#include <iostream>
#undef main

// Function to handle errors
void logSDLError(std::ostream& os, const std::string& msg) {
    os << msg << " error: " << SDL_GetError() << std::endl;
}



void soundEngine::initialize() {
    if (SDL_Init(SDL_INIT_AUDIO) != 0) {
        logSDLError(std::cout, "SDL_Init");
    }
}



WAV soundEngine::loadWavFile(const char* filepath, const char* wavName) {
    WAV temp = { 0 };
    temp.name = wavName;

    if (SDL_LoadWAV(filepath, &temp.wavSpec, &temp.wavBuffer, &temp.wavLength) == nullptr) {
        logSDLError(std::cout, "SDL_LoadWAV");
        SDL_Quit();
    }
    return temp;
}


SDL_AudioDeviceID  soundEngine::openAudioDevice(WAV wav) {
    SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(nullptr, 0, &wav.wavSpec, nullptr, 0);
    if (deviceId == 0) {
        logSDLError(std::cout, "SDL_OpenAudioDevice");
        SDL_FreeWAV(wav.wavBuffer);
        SDL_Quit();
    }
    return deviceId;
}

void soundEngine::playAudio(WAV wav, SDL_AudioDeviceID deviceId){
    int success = SDL_QueueAudio(deviceId, wav.wavBuffer, wav.wavLength);
    if (success < 0) {
        logSDLError(std::cout, "SDL_QueueAudio");
        SDL_Quit();
    }

    SDL_PauseAudioDevice(deviceId, 0);
    SDL_Delay((wav.wavLength / wav.wavSpec.freq) * 1000);
}

void soundEngine::audioCleanup(WAV wav, SDL_AudioDeviceID deviceId) {
    SDL_CloseAudioDevice(deviceId);
    SDL_FreeWAV(wav.wavBuffer);
    SDL_Quit();
}
   
