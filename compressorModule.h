/*
 * =====================================================================================
 *
 *       Filename:  compressorModule.h
 *
 *    Description:  Compressor struct
 *
 *        Version:  1.0
 *        Created:  12-08-21 07:49:50 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Andrew Kmiecik (asckmie), andrew.kmiecik@mail.utoronto.ca
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef COMPRESSORMODULE_H
#define COMPRESSORMODULE_H 

#include "compress.h"
#include "circularBuffer.h"

typedef enum  {
    piecewise,
    atan
} CompressionType;

typedef struct {
    int threshold;
    int windowSize;
    float ratio;
    float attack;
    float release;
    int sampleRate;
    int numAttackSamples;
    CompressionType type;
} CompressorParameters;


typedef struct {
    CircularBuffer cb;
    int attackedSampleCount;
    float* attackSlopesArray;    
    CompressorParameters* params;
    int startAttack;
    int doneAttack;
} Compressor;


void initCompressorParams(CompressorParameters* params, const int threshold, const int windowSize, const float ratio, 
                          const float attack, const float release, const int sampleRate);

void printCompressorParameters(const CompressorParameters* params);
void printCompressor(Compressor* compressor);
void initCompressor(Compressor* compressor,  CompressorParameters* params);
void compress(Compressor* compressor, unsigned char* data, const int frames);
void freeCompressor(Compressor* compressor);

#endif /* COMPRESSORMODULE_H */
