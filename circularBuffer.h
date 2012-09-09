#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H 

typedef struct {
        int bufferSize;
        int lastIndex;
        float* buffer;
	int headIndex;
	int tailIndex;

} CircularBuffer;


void addElementToCircularBuffer(float elmt, CircularBuffer* cb);
void initCircularBuffer(int size, CircularBuffer* cb);
void printCircularBuffer(CircularBuffer* cb);
float getFirstElementInCircularBuffer(CircularBuffer* cb);
float getLastElementInCircularBuffer(CircularBuffer* cb);
double getSumOfSquaresOfCircularBuffer(CircularBuffer* cb);
double getRMSValueOfCircularBuffer(CircularBuffer* cb);
void freeCircularBuffer(CircularBuffer* cb);

#endif /* CIRCULARBUFFER_H */

