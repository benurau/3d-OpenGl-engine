#pragma once

#include "miniaudio.h"

#ifdef _WIN32
#pragma comment(lib, "ole32.lib")
#endif

enum AudioGroup {
    AUDIO_GROUP_MUSIC,
    AUDIO_GROUP_SFX,
    AUDIO_GROUP_VOICE,
    AUDIO_GROUP_COUNT
};

struct AudioEngine {
    ma_engine engine;
    ma_sound_group groups[AUDIO_GROUP_COUNT];
    bool initialized = false;

    bool init()
    {
        if (initialized) return true;

        ma_result result = ma_engine_init(NULL, &engine);
        if (result != MA_SUCCESS) return false;

        for (int i = 0; i < AUDIO_GROUP_COUNT; i++)
        {
            result = ma_sound_group_init(&engine, NULL, NULL, &groups[i]);
            if (result != MA_SUCCESS)
            {
                ma_engine_uninit(&engine);
                return false;
            }
        }

        ma_sound_group_set_volume(&groups[AUDIO_GROUP_MUSIC], 0.4f);

        initialized = true;
        return true;
    }

    void shutdown()
    {
        if (!initialized) return;

        for (int i = 0; i < AUDIO_GROUP_COUNT; i++)
            ma_sound_group_uninit(&groups[i]);

        ma_engine_uninit(&engine);
        initialized = false;
    }

    void setMasterVolume(float volume)
    {
        if (!initialized) return;
        ma_engine_set_volume(&engine, volume);
    }

    void setGroupVolume(AudioGroup group, float volume)
    {
        if (!initialized) return;
        if (group < 0 || group >= AUDIO_GROUP_COUNT) return;
        ma_sound_group_set_volume(&groups[group], volume);
    }

    void setListenerPosition(float x, float y, float z)
    {
        if (!initialized) return;
        ma_engine_listener_set_position(&engine, 0, x, y, z);
    }

    void setListenerDirection(float fx, float fy, float fz)
    {
        if (!initialized) return;
        ma_engine_listener_set_direction(&engine, 0, fx, fy, fz);
    }

    void setListenerWorldUp(float ux, float uy, float uz)
    {
        if (!initialized) return;
        ma_engine_listener_set_world_up(&engine, 0, ux, uy, uz);
    }

    ma_sound* playSound(const char* filePath, AudioGroup group = AUDIO_GROUP_SFX)
    {
        if (!initialized) return NULL;
        if (group < 0 || group >= AUDIO_GROUP_COUNT) return NULL;

        ma_sound* sound = new ma_sound;
        ma_result result = ma_sound_init_from_file(&engine, filePath, 0, &groups[group], NULL, sound);
        if (result != MA_SUCCESS)
        {
            delete sound;
            return NULL;
        }

        ma_sound_start(sound);
        return sound;
    }

    void playFireAndForget(const char* filePath, AudioGroup group = AUDIO_GROUP_SFX)
    {
        if (!initialized) return;
        ma_engine_play_sound(&engine, filePath, NULL);
    }

    ma_sound* playMusic(const char* filePath)
    {
        if (!initialized) return NULL;

        ma_sound* music = new ma_sound;
        ma_result result = ma_sound_init_from_file(
            &engine, filePath,
            MA_SOUND_FLAG_STREAM | MA_SOUND_FLAG_DECODE,
            &groups[AUDIO_GROUP_MUSIC], NULL, music);
        if (result != MA_SUCCESS)
        {
            delete music;
            return NULL;
        }

        ma_sound_set_looping(music, MA_FALSE);
        ma_sound_start(music);
        return music;
    }

    void stopSound(ma_sound* sound)
    {
        if (!sound) return;
        ma_sound_stop(sound);
        ma_sound_uninit(sound);
        delete sound;
    }

    void stopAllSounds()
    {
        if (!initialized) return;
        for (int i = 0; i < AUDIO_GROUP_COUNT; i++)
            ma_sound_group_stop(&groups[i]);
    }
};
