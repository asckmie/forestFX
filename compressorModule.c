/*
 * =====================================================================================
 *
 *       Filename:  compresssorModule.c
 *
 *    Description:  Compressor Module
 *
 *        Version:  1.0
 *        Created:  12-08-21 08:18:04 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Andrew Kmiecik (asckmie), andrew.kmiecik@mail.utoronto.ca
 *   Organization:  
 *
 * =====================================================================================
 */
#include "compressorModule.h"
#include "pcm_interface.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

void printCompressorParams(const CompressorParameters* params){

    assert( NULL != params );

    printf("Compression Parameters:\n");
    printf("\tThreshold:%d\n", params->threshold);
    printf("\tWindowSize:%d\n", params->windowSize);
    printf("\tRatio:%f\n", params->ratio);
    printf("\tAttack:%f\n", params->attack);
    printf("\tRelease:%f\n", params->release);
    printf("\tSampleRate:%d\n", params->sampleRate);
    printf("\tNumAttackSamples:%d\n", params->numAttackSamples);
}

void initCompressorParams(CompressorParameters* params, const int threshold,
                            const int windowSize,  const float ratio, const float attack, 
                            const float release, const int sampleRate){

    assert( NULL != params );    
    assert( threshold > 0 );
    assert( windowSize > 0 );
    assert( ratio > 0 && ratio < 1);
    assert( attack > 0 );
    assert( release > 0 );
    assert( sampleRate > 0 );    

    params->threshold = threshold;
    params->windowSize = windowSize;
    params->ratio = ratio;
    params->attack = attack;
    params->release = release;
    params->sampleRate = sampleRate;
    params->numAttackSamples = sampleRate * attack;
    printCompressorParams(params);
}

void initCompressor(Compressor* compressor, CompressorParameters* params){

    assert( NULL != compressor );    
    assert( NULL != params );

    compressor->params = params; 
    int numAttackSamples = (compressor->params->sampleRate)*(compressor->params->attack);
    compressor->attackedSampleCount = 0;
    compressor->attackSlopesArray = (float*)malloc(numAttackSamples*sizeof(float));
   
    int i; 
	for(i=0; i<numAttackSamples; i++ ){
		compressor->attackSlopesArray[i] = compressor->params->ratio + i*(1 - compressor->params->ratio)/numAttackSamples;
    }
    initCircularBuffer(compressor->params->windowSize,&(compressor->cb));
    compressor->params->type = piecewise;
    compressor->startAttack = 1;
    compressor->doneAttack = 1;
    
}

void printArray( const float* array, const int size){

    assert( NULL != array );    

    int i;
    for(i=0; i<size; i++){
        printf("%f", array[i]);
        if((size-1) != i){
            printf(",\n");
        }
    }
    printf("\n");
}

void printCompressor(Compressor* compressor){

    assert( NULL != compressor );

    printf("\n\nCompressor:\n");
    printCompressorParams(compressor->params);
    printf("\tAttackedSampleCount:%d\n", compressor->attackedSampleCount);
    //printArray(compressor->attackSlopesArray, compressor->params->numAttackSamples);
    // printCircularBuffer(&(compressor->cb));
    printf("\tCircularBuffer: [%f : %f]\n", getFirstElementInCircularBuffer(&(compressor->cb)),
                                                         getLastElementInCircularBuffer(&(compressor->cb))); 
    printf("\tAttackSamplesArray, [%f:%f]\n", compressor->attackSlopesArray[0], 
                                            compressor->attackSlopesArray[compressor->params->numAttackSamples-1]);
    printf("\tType:");
    switch(compressor->params->type){
        case piecewise:
            printf(" piecewise\n");
            break;
        case atan:
            printf(" arctan");
            break;
        default:
            printf(" unknown");
    }
    printf("StartAttack:", (0 < compressor->startAttack) ? "true" : "false");
    
}

void freeCompressor(Compressor* compressor){

    assert( NULL != compressor);

    freeCircularBuffer(&(compressor->cb));
    free(compressor->attackSlopesArray);
    free(compressor);
    compressor = NULL;
}


void compress(Compressor* compressor, unsigned char* data, const int frames){

    assert( NULL != compressor );
    assert( NULL != data );
    assert( frames > 0 );

	unsigned char *ptrIn = data;
	unsigned char *ptrOut = data;

	int i, j;

	double rmsValue = 0;	
    
    int index = 0;

	for(i=0; i<frames; i++){
        for (j=0; j < CHANNELS; j++ ){
            index = CHANNELS*i+j;
             
            //only acting upon a single channel for now
		    if(j%2 == 0){
			    addElementToCircularBuffer(ptrIn[index], &(compressor->cb));
            }
            if(i >= compressor->cb.bufferSize){
				//sumOfSquares = getSumOfSquaresOfCircularBuffer(&(compressor->cb));
                //printf("the sum of squares is:%f\n", sumOfSquares);
                rmsValue = getRMSValueOfCircularBuffer(&(compressor->cb));
				
                if(rmsValue > compressor->params->threshold){
					if(0 < compressor->startAttack){
						compressor->startAttack = 0;
						compressor->attackedSampleCount = 0;
					}
					ptrIn[index] = compress_pw_linear(
                                    ptrIn[index], 
                                    compressor->params->threshold, 
                                    compressor->params->ratio, 
                                    compressor->attackedSampleCount,
                                    compressor->attackSlopesArray);
					
                    if(compressor->attackedSampleCount != compressor->params->numAttackSamples){
                        compressor->attackedSampleCount++;
                    } else {
                        compressor->startAttack = 1;
                        compressor->attackedSampleCount = 0;
                    }
				}
			}
		}
	}
}
