#ifndef _ENCODER_H_
#define _ENCODER_H_


#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

#ifdef _32WIN
extern "C"
{
#include "libavformat\avformat.h"
#include "libavutil\opt.h"
#include "libavcodec\avcodec.h"
#include "libavutil\channel_layout.h"
#include "libavutil\common.h"
#include "libavutil\imgutils.h"
#include "libavutil\mathematics.h"
#include "libavutil\samplefmt.h"
};
#else

#ifdef __cplusplus
extern "C"
{
#endif
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/imgutils.h>
#include <libavutil/mathematics.h>
#include <libavutil/samplefmt.h>
#ifdef __cplusplus
}
#endif
#endif

typedef struct
{
	FILE* pFin;
	FILE* pFout;

	char* inFileName;
	char* outFileName;

	int nImageWidth;
	int nImageHeight;
	int nFrameRate;
	int bitRate;
	int TotalFrame;

}IOPARAM;

/*************************************************
Struct:			CodecCtx
Description:	FFMpeg�������������
*************************************************/
typedef struct
{
	AVCodec			*codec;		//ָ��������ʵ��
	AVFrame			*frame; 	//�������֮��/����֮ǰ����������
	AVCodecContext	*c;			//������������ģ�������������һЩ��������
	AVPacket		pkt;		//�������ṹ������������������
} CodecCtx;

#endif