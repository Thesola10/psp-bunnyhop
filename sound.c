#include <pspaudio.h>
#include <pspaudiolib.h>
#include <pspiofilemgr.h>
#include <stdlib.h>
#include <raylib.h>

#include "sound.h"

const int CHANNEL_SFX = 1;
const int CHANNEL_BGM = 2;

bhop_Sound *bgm = 0;

void bhop_initSound(void)
{
    pspAudioInit();
}

void bhop_Sound_play(bhop_Sound *snd)
{
    if (!snd) return;

    TraceLog(LOG_INFO, "Playing %d samples @%d/%dch\n",
            snd->data_len / 4, snd->sample_rate, snd->num_channels);

    sceAudioChReserve(CHANNEL_SFX, snd->data_len / (2 * 2), PSP_AUDIO_FORMAT_STEREO);
    sceAudioOutput(CHANNEL_SFX, 0x6000, snd->bytes);
    //sceAudioChRelease(CHANNEL_SFX);
}

void bhop_Sound_loadBgm(bhop_Sound *snd)
{
    bgm = snd;
}

void bhop_refreshSound(void)
{
    if (! sceAudioGetChannelRestLen(CHANNEL_SFX)) {
        sceAudioChRelease(CHANNEL_SFX);
    }

    if (! sceAudioGetChannelRestLen(CHANNEL_BGM)) {
        sceAudioChRelease(CHANNEL_BGM);
        if (bgm) {
            sceAudioChReserve(CHANNEL_BGM, bgm->data_len / (bgm->sample_rate * 2 * 2), PSP_AUDIO_FORMAT_STEREO);
            sceAudioOutput(CHANNEL_BGM, 0x4000, bgm->bytes);
        }
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

    result.data = malloc(sndStat.st_size);

    sceIoRead(sndFile, result.data, sndStat.st_size);

    sceIoClose(sndFile);

    return result.snd;
}
