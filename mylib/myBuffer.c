#include "myBuffer.h"
#define _IS_EMPTY(myBuf) (myBuf->_ind0 == myBuf->_ind1)

#define _CLEAR_BUF(myBuf) \
	do                    \
	{                     \
		myBuf->_ind0 = 0; \
		myBuf->_ind1 = 0; \
	} while (0)

void myBufferInit(MyBuffer *myBuf)
{
	_CLEAR_BUF(myBuf);
}
void myBufferClear(MyBuffer *myBuf)
{
	_CLEAR_BUF(myBuf);
}
inline char myBufferIsEmpty(MyBuffer *myBuf)
{
	return _IS_EMPTY(myBuf);
}

inline char myBufferIsFull(MyBuffer *myBuf)
{
	return myBuf->_ind1 + 1 == myBuf->_ind0;
}

/*入列，成功返回1，否则返回0*/
char myBufferEnqueue(uint8_t data, MyBuffer *myBuf)
{
	uint8_t next = myBuf->_ind1 + 1;
	if (next == myBuf->_ind0)
		return 0;

	myBuf->buf[myBuf->_ind1++] = data;
	return 1;
}
/*出列，成功返回1，data返回数据；否则返回0*/
char myBufferDequeue(uint8_t *data, MyBuffer *myBuf)
{
	if (_IS_EMPTY(myBuf))
		return 0;
	*data = myBuf->buf[myBuf->_ind0++];
	return 1;
}
