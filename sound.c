#include <pspaudio.h>
#include <pspaudiolib.h>
#include <pspiofilemgr.h>
#include <stdlib.h>
#include <raylib.h>

#include "sound.h"

const int CHANNEL_SFX = 1;
const int CHANNEL_BGM = 0;

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

    sceAudioChReserve(sfxc + CHANNEL_SFX, PSP_AUDIO_SAMPLE_ALIGN(snd->data_len / (2 * 2)) - 64, PSP_AUDIO_FORMAT_STEREO);
    SFXTAB[sfxc] = 1;
    sceAudioOutput(sfxc + CHANNEL_SFX, 0x6000, snd->bytes);
}

void _impl_bhop_loadNextBgmChunk(void)
{
    long pos = bgm_cursor - bgm->bytes;
    long rem = bgm->data_len - pos;

    if (rem >= (PSP_AUDIO_SAMPLE_MAX*4)) {
        TraceLog(LOG_INFO, "Feeding %d BGM samples...", PSP_AUDIO_SAMPLE_MAX);
        sceAudioSetChannelDataLen(CHANNEL_BGM, PSP_AUDIO_SAMPLE_MAX);
        sceAudioOutput(CHANNEL_BGM, 0x4000, bgm_cursor);
        bgm_cursor += PSP_AUDIO_SAMPLE_MAX*4;
    } else {
        TraceLog(LOG_INFO, "Feeding %d BGM samples and looping...", rem/4);
        sceAudioSetChannelDataLen(CHANNEL_BGM, PSP_AUDIO_SAMPLE_ALIGN(rem / 4) - 64);
        sceAudioOutput(CHANNEL_BGM, 0x4000, bgm_cursor);
        bgm_cursor = bgm->bytes;
    }
 
    return;
}

void bhop_Sound_loadBgm(bhop_Sound *snd)
{
    int ret = 0;

    if (!snd) {
        sceAudioChRelease(CHANNEL_BGM);
        return;
    }

    bgm = snd;
    bgm_cursor = snd->bytes + PSP_AUDIO_SAMPLE_MAX*4;

    TraceLog(LOG_INFO, "Loaded BGM: %d samples @%d/%dch\n",
            snd->data_len / 4, snd->sample_rate, snd->num_channels);

    ret = sceAudioChRelease(CHANNEL_BGM);
    TraceLog(LOG_INFO, "BGM: sceAudioChRelease: %d\n", ret);
    ret = sceAudioChReserve(CHANNEL_BGM, PSP_AUDIO_SAMPLE_MAX, PSP_AUDIO_FORMAT_STEREO);
    TraceLog(LOG_INFO, "BGM: sceAudioChReserve: %d\n", ret);
    ret = sceAudioOutput(CHANNEL_BGM, 0x4000, snd->bytes);
    TraceLog(LOG_INFO, "BGM: sceAudioOutput: %d\n", ret);
}

void bhop_refreshSound(void)
{
    for (int i = 0; i < 4; i++) {
        if (SFXTAB[i]) {
            if (! sceAudioGetChannelRestLen(i + CHANNEL_SFX)) {
                sceAudioChRelease(i + CHANNEL_SFX);
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
