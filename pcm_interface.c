#include "pcm_interface.h"

snd_pcm_t* openPCM(const char *pcm_name, snd_pcm_stream_t mode){
	/* Handle for the PCM device */ 
    	snd_pcm_t *pcm_handle;          

    /* Open PCM. The last parameter of this function is the mode. */
    /* If this is set to 0, the standard mode is used. Possible   */
    /* other values are SND_PCM_NONBLOCK and SND_PCM_ASYNC.       */ 
    /* If SND_PCM_NONBLOCK is used, read / write access to the    */
    /* PCM device will return immediately. If SND_PCM_ASYNC is    */
    /* specified, SIGIO will be emitted whenever a period has     */
    /* been completely processed by the soundcard.                */
    	if (snd_pcm_open(&pcm_handle, pcm_name, mode, 0) < 0) {
      		fprintf(stderr, "Error opening PCM device %s\n", pcm_name);
      		exit(-1);
    	}

	return pcm_handle;

}

void configurePCM(snd_pcm_t *pcm_handle) {
    /* This structure contains information about    */
    /* the hardware and can be used to specify the  */      
    /* configuration to be used for the PCM stream. */ 
    	snd_pcm_hw_params_t *hwparams;
  

    /* Allocate the snd_pcm_hw_params_t structure on the stack. */
    	snd_pcm_hw_params_alloca(&hwparams);

	/* Init hwparams with full configuration space */
    	if (snd_pcm_hw_params_any(pcm_handle, hwparams) < 0) {
      		fprintf(stderr, "Can not configure this PCM device.\n");
      		exit(-1);
    	}

    	unsigned int exact_rate;   /* Sample rate returned by */
                      /* snd_pcm_hw_params_set_rate_near */ 
	snd_pcm_uframes_t exact_buffersize;
    	snd_pcm_uframes_t periodsize = 8192; /* Periodsize (bytes) */

    /* Set access type. This can be either    */
    /* SND_PCM_ACCESS_RW_INTERLEAVED or       */
    /* SND_PCM_ACCESS_RW_NONINTERLEAVED.      */
    /* There are also access types for MMAPed */
    /* access, but this is beyond the scope   */
    /* of this introduction.                  */
    	if (snd_pcm_hw_params_set_access(pcm_handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED) < 0) {
      		fprintf(stderr, "Error setting access.\n");
      		exit(-1);
    	}
  
    /* Set sample format */
    	if (snd_pcm_hw_params_set_format(pcm_handle, hwparams, SND_PCM_FORMAT_S16_LE) < 0) {
      		fprintf(stderr, "Error setting format.\n");
      		exit(-1);
    	}

    /* Set sample rate. If the exact rate is not supported */
    /* by the hardware, use nearest possible rate.         */ 
    	exact_rate = RATE;
    	if (snd_pcm_hw_params_set_rate_near(pcm_handle, hwparams, &exact_rate, 0) < 0) {
      		fprintf(stderr, "Error setting rate.\n");
      		exit(-1);
    	}
    	if (exact_rate!=RATE) {
      		fprintf(stderr, "The rate %d Hz is not supported by your hardware.\n ==> Using %d Hz instead.\n", RATE, exact_rate);
    	}

    /* Set number of channels */
    	if (snd_pcm_hw_params_set_channels(pcm_handle, hwparams, CHANNELS) < 0) {
      		fprintf(stderr, "Error setting channels.\n");
      		exit(-1);
    	}

    /* Set number of periods. Periods used to be called fragments. */ 
    	if (snd_pcm_hw_params_set_periods(pcm_handle, hwparams, PERIODS, 0) < 0) {
      		fprintf(stderr, "Error setting periods.\n");
      		exit(-1);
    	}

    /* Set buffer size (in frames). The resulting latency is given by */
    /* latency = periodsize * periods / (rate * bytes_per_frame)     */
	exact_buffersize=(PERIOD_SIZE * PERIODS)>>2;
    	if (snd_pcm_hw_params_set_buffer_size_near(pcm_handle, hwparams, &exact_buffersize) < 0) {
      		fprintf(stderr, "Error setting buffersize.\n");
      		exit(-1);
    	}
	if (exact_buffersize!=(PERIOD_SIZE * PERIODS)>>2) {
      		fprintf(stderr, "The buffer size %dB is not supported by your hardware.\n ==> Using %dB instead.\n", (PERIOD_SIZE * PERIODS)>>2, exact_buffersize);
    	}

    /* Apply HW parameter settings to */
    /* PCM device and prepare device  */
    	if (snd_pcm_hw_params(pcm_handle, hwparams) < 0) {
      		fprintf(stderr, "Error setting HW params.\n");
      		exit(-1);
    	}
}

void closePCM(snd_pcm_t *pcm_handle) {
	/* Stop PCM device and drop pending frames */
    	snd_pcm_drop(pcm_handle);
}

int readPCM(snd_pcm_t *handle, unsigned char *data, int frames) {
	int inframes;
	while ((inframes = snd_pcm_readi(handle, data, frames)) < 0) {
        	fprintf(stderr, "Input buffer overrun\n");
        	snd_pcm_prepare(handle);
    	}
	return inframes;
}
int writePCM(snd_pcm_t *handle, unsigned char *data, int frames) {
	int outframes=0;
	while ((outframes = snd_pcm_writei(handle, data, frames)) < 0) {
        	//fprintf(stderr, "Output buffer underrun\n");
        	snd_pcm_prepare(handle);
    	}
	return outframes;
}
