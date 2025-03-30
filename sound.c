#include <pspaudio.h>
#include <pspaudiolib.h>
#include <pspiofilemgr.h>
#include <stdlib.h>
#include <raylib.h>

#include "sound.h"

const int CHANNEL_SFX = 1;
const int CHANNEL_BGM = PSP_AUDIO_CHANNEL_MAX - 1;

char SFXTAB[4] = { 0, 0, 0, 0 };

bhop_Sound *bgm = 0;

unsigned char *bgm_cursor;

void bhop_initSound(void)
{
    pspAudioInit();
}

void bhop_Sound_play(bhop_Sound *snd)
{
    if (!snd) return;

    int sfxc = 0;

    for (; sfxc < 4; sfxc++) {
        if (!SFXTAB[sfxc])
            break;
    }

    TraceLog(LOG_INFO, "Playing %d samples @%d/%dch\n",
            snd->data_len / 4, snd->sample_rate, snd->num_channels);

    sceAudioChReserve(sfxc, PSP_AUDIO_SAMPLE_ALIGN(snd->data_len / (2 * 2)) - 64, PSP_AUDIO_FORMAT_STEREO);
    SFXTAB[sfxc] = 1;
    sceAudioOutput(sfxc, 0x6000, snd->bytes);
}

void _impl_bhop_loadNextBgmChunk(void)
{
    int pos = bgm_cursor - bgm->bytes;
    int rem = bgm->data_len - pos;

    if (rem < PSP_AUDIO_SAMPLE_MAX) {
        TraceLog(LOG_DEBUG, "Feeding %d BGM samples and looping...", rem/4);
        sceAudioSetChannelDataLen(CHANNEL_BGM, PSP_AUDIO_SAMPLE_ALIGN(rem / 4) - 64);
        sceAudioOutput(CHANNEL_BGM, 0x4000, bgm_cursor);
        bgm_cursor = bgm->bytes;
    } else {
        TraceLog(LOG_DEBUG, "Feeding %d BGM samples...", PSP_AUDIO_SAMPLE_MAX);
        sceAudioSetChannelDataLen(CHANNEL_BGM, PSP_AUDIO_SAMPLE_MAX);
        sceAudioOutput(CHANNEL_BGM, 0x4000, bgm_cursor);
        bgm_cursor += PSP_AUDIO_SAMPLE_MAX*4;
    }
    return;
}

void bhop_Sound_loadBgm(bhop_Sound *snd)
{
    if (!snd) {
        sceAudioChRelease(CHANNEL_BGM);
        return;
    }

    bgm = snd;
    bgm_cursor = snd->bytes;

    TraceLog(LOG_INFO, "Loaded BGM: %d samples @%d/%dch\n",
            snd->data_len / 4, snd->sample_rate, snd->num_channels);

    sceAudioChRelease(CHANNEL_BGM);
    sceAudioChReserve(CHANNEL_BGM, PSP_AUDIO_SAMPLE_MAX, PSP_AUDIO_FORMAT_STEREO);
    sceAudioOutput(CHANNEL_BGM, 0x4000, snd->bytes);
}

void bhop_refreshSound(void)
{
    for (int i = 0; i < 4; i++) {
        if (SFXTAB[i]) {
            if (! sceAudioGetChannelRestLen(i)) {
                sceAudioChRelease(i);
                SFXTAB[i] = 0;
            }
        }
    }

    if (! sceAudioGetChannelRestLen(CHANNEL_BGM)) {
        _impl_bhop_loadNextBgmChunk();
    }
}

bhop_Sound *bhop_Sound_loadFromFile(char *path)
{
    typedef union {
        bhop_Sound *snd;
        char *data;
    } _impl_bhop_RawSound;

    SceUID sndFile;
    SceIoStat sndStat;
    _impl_bhop_RawSound result;

    if (sceIoGetstat(path, &sndStat))
        return 0;
    sndFile = sceIoOpen(path, PSP_O_RDONLY, 0777);

    result.data = malloc(sndStat.st_size + 32);

    sceIoRead(sndFile, result.data, sndStat.st_size);

    sceIoClose(sndFile);

    return result.snd;
}

void bhop_Sound_unload(bhop_Sound *snd)
{
    if (snd)
        free(snd);
    sceAudioChRelease(CHANNEL_BGM);
}
