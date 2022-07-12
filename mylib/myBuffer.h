#ifndef _MYBUFFER
#define _MYBUFFER

typedef unsigned char uint8_t;

typedef struct
{
	uint8_t buf[256];
	uint8_t _ind0;
	uint8_t _ind1;
} MyBuffer;

#ifdef __cplusplus
extern "C"
{
#endif
	void myBufferInit(MyBuffer *myBuf);
	void myBufferClear(MyBuffer *myBuf);
	char myBufferIsEmpty(MyBuffer *myBuf);
	char myBufferIsFull(MyBuffer *myBuf);
	char myBufferEnqueue(uint8_t data, MyBuffer *myBuf);
	char myBufferDequeue(uint8_t *data, MyBuffer *myBuf);
#ifdef __cplusplus
}
#endif

#endif
