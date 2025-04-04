

typedef struct {
    // RIFF Header
    char riff_header[4];    // Contains "RIFF"
    int wav_size;           // Size of the wav portion of the file, which follows the first 8 bytes.
    char wave_header[4];    // Contains "WAVE"

    // Format Header
    char fmt_header[4];     // Contains "fmt " (includes trailing space)
    int fmt_chunk_size;     // Should be 16 for PCM
    short audio_format;     // Should be 1 for PCM. 3 for IEEE Float
    short num_channels; 
    int sample_rate;
    int byte_rate;          // Number of bytes per second. sample_rate * num_channels * Bytes Per Sample
    short sample_alignment; // num_channels * Bytes Per Sample
    short bit_depth; // Number of bits per sample

    // Data
    char data_header[4];    // Contains "data"
    int data_len;           // Number of bytes in data. Number of samples * num_channels * sample byte size

    unsigned char bytes[];        // Actual data
} wav_file_t;

typedef wav_file_t bhop_Sound;

void bhop_initSound(void);

void bhop_Sound_play(bhop_Sound *snd);

void bhop_Sound_loadBgm(bhop_Sound *snd, char loop);

void bhop_refreshSound(void);

bhop_Sound *bhop_Sound_loadFromFile(char *path);
void bhop_Sound_unload(bhop_Sound *sound);
