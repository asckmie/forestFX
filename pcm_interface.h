#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>

#define PERIOD_SIZE 8192
#define RATE 44100
#define PERIODS 2
#define CHANNELS 2
#define BYTE_PER_FRAME 4
#define BYTE_PER_CHANNEL 2
#define INPUT_CHANNEL 1 // start from 0

snd_pcm_t* openPCM(const char *pcm_name, snd_pcm_stream_t mode);
void configurePCM(snd_pcm_t *pcm_handle);
void closePCM(snd_pcm_t *pcm_handle);
int readPCM(snd_pcm_t *handle, unsigned char *data, int frames);
int writePCM(snd_pcm_t *handle, unsigned char *data, int frames);
