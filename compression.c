#include <stdio.h>
#include <stdlib.h>
#include "pcm_interface.h"
#include "compress.h"
#include "compressorModule.h"

void main (int argc, char *argv[]) {
	
	int gain;
	char *devName="hw:0,0";
	int compressOn;
	int threshold;
    float ratio;
	float attack;
    float release;
	int windowSize;

    CompressorParameters * params = (CompressorParameters *)malloc(sizeof(CompressorParameters));
    Compressor * compressor = (Compressor*)malloc(sizeof(Compressor));


	if (argc!=8) {
		printf("\nUsage: ./compressor <switch {1,0}> <threshold> <ratio> <attack_ms> <release_ms>");
		printf(" <window_size> [Device Name]\n\n");
		printf("switch \t\t 0 turns compressor off, 1 turns compressor on\n");
		printf("threshold \t threshold beyond which samples are compressed\n");
		printf("ratio \t\t the amount of compression, higher values lead to heavier\n");
		printf("\t\t compression (ie: ratio of 2 means values are compressed by 1/2\n");
		printf("attack_ms \t when the level of the signal exceeds the threshold,\n");
		printf("\t\t an exponential decay \"attacks\" the signal for attack_ms milliseconds\n");
		printf("release_ms \t after the attack, the signal is compressed for as long as the level is above the threshold;\n");
		printf("\t\t the compression ceases after release_ms milliseconds\n");
		printf("window_size \t the rms window size, a larger size is more accurate, but slower\n");
		printf("Device Name \t the name of the device to use\n");
		printf("\nExample:\n\t ./compressor 1 100 2 10 10 100 hw:0,0\n");
		exit(0);
	} else {
        
		compressOn = atoi(argv[1]);	
		threshold = atoi(argv[2]);
		ratio = atof(argv[3]);
		attack = atof(argv[4]);
		release = atof(argv[5]);
		windowSize = atoi(argv[6]);
		//devName = argv[7];

        initCompressorParams(params, threshold, windowSize, ratio, attack, release, RATE);
        initCompressor(compressor, params);

        printCompressor(compressor);
	}

	/* start PCM device */
	printf("Starting up pcm device:%s\n", devName);
	snd_pcm_t *pcmOutHandle=openPCM(devName,SND_PCM_STREAM_PLAYBACK);	
	snd_pcm_t *pcmInHandle=openPCM(devName,SND_PCM_STREAM_CAPTURE);	

	configurePCM(pcmOutHandle);
	configurePCM(pcmInHandle);

	unsigned char *data;
    data = (unsigned char *)malloc(PERIOD_SIZE);
    
    int frames=PERIOD_SIZE/BYTE_PER_FRAME;
	int inframes,outframes;
	
    printf("done starting pcm device\n");
    /*-----------------------------*/
	
	/* start compressor loop */
	while(1) {

		inframes=readPCM(pcmInHandle, data, frames);		

    	if (inframes != frames){
        		fprintf(stderr, "Short read from capture device: %d, expecting %d\n", inframes, frames);
        }

		if(compressOn == 1) {
            compress(compressor, data, frames);
        }

    		outframes=writePCM(pcmOutHandle, data, frames);
    		if (outframes != inframes)
        		fprintf(stderr, "Short write to playback device: %d, expecting %d\n", outframes, frames);

	}
	
	free(data);
    freeCompressor(compressor);
	/*close filter loop*/
	closePCM(pcmOutHandle);
	closePCM(pcmInHandle);

}


