#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

typedef struct {
        int bufferSize;
        int lastIndex;
        float* buffer;
    	int headIndex;
	    int tailIndex;

} CircularBuffer;

void addElementToCircularBuffer(float elmt, CircularBuffer* cb){
    
    assert( NULL != cb );

	cb->buffer[cb->tailIndex] = elmt;


	cb->tailIndex++;
	if(cb->tailIndex == (cb->bufferSize)){
		cb->tailIndex = 0;
	}
	cb->headIndex = (cb->tailIndex+cb->bufferSize+1)%cb->bufferSize;
	return;
}

void initCircularBuffer(int size, CircularBuffer* cb){
    
    assert( size > 0 );

	cb->bufferSize = size;
	cb->buffer = (float*)calloc(cb->bufferSize, sizeof(float));
	cb->headIndex  = 0;
	cb->tailIndex = 0;	
	return;
}

void printCircularBuffer(CircularBuffer* cb){

    assert( NULL != cb );
	
	int i;
	for(i = 0; i<cb->bufferSize; i++)
		printf("elt at index:%d  = %f\n", i, cb->buffer[i]);
	return;
}

float getFirstElementInCircularBuffer(CircularBuffer* cb){
    assert( NULL != cb );
	return cb->buffer[cb->headIndex];
}

float getLastElementInCircularBuffer(CircularBuffer* cb){
    assert( NULL != cb );	
    //the tail has just wrapped around, last element is at the end of the buffer
	if(cb->tailIndex == 0) return cb->buffer[cb->bufferSize-1];
	else return cb->buffer[cb->tailIndex-1];
}

double getSumOfSquaresOfCircularBuffer(CircularBuffer* cb){
    assert( NULL != cb );

	int i;
	double sumOfSquares = 0;
	int bufferSize = cb->bufferSize;
	double squareValue = 0;	

	for(i=0; i<bufferSize; i++){
		squareValue =  pow(cb->buffer[i],2);
		sumOfSquares+= squareValue;
	
	}
	return sumOfSquares;
}

double getRMSValueOfCircularBuffer(CircularBuffer* cb){
    
    assert( NULL != cb );

	int bufferSize = cb->bufferSize;
	
	double sumOfSquares = getSumOfSquaresOfCircularBuffer(cb);
	
	return sqrt((((double)1/bufferSize))*sumOfSquares);
		
}

void freeCircularBuffer(CircularBuffer* cb){
	if( NULL != cb ){
		free(cb->buffer);
		cb->buffer = NULL;
		cb = NULL;
	}
	return;
}

