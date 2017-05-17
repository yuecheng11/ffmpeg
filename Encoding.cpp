#include "Error.h"
#include "Encoding.h"
using namespace std;

//�ַ���ת����
/****************************/
/*int a;
string str = "1024";
istringstream sInt(str);
sInt >> a;
*/
/****************************/

int parse_input_parameters(int argc, char** argv, IOPARAM &param)
{
	if (argc < 0)
	{
		cout << "param error" << endl;
		return PARAM_ERROR;
	}
	param.inFileName = argv[1];
	param.outFileName = argv[2];

	param.nImageWidth = atoi(argv[3]);
	param.nImageHeight = atoi(argv[4]);
	param.bitRate = atoi(argv[5]);
	param.TotalFrame = atoi(argv[6]);

	param.nFrameRate = 25;

	return OK;
}

int open_file(IOPARAM &ioparam)
{
	fopen_s(&ioparam.pFin, ioparam.inFileName, "rb");
	if (!(ioparam.pFin))
	{
		fprintf(stderr, "could not open %s\n", ioparam.inFileName);
		return ERROR_OPEN_FILE;
	}

	fopen_s(&ioparam.pFout, ioparam.outFileName, "wb");
	if (!(ioparam.pFin))
	{
		fprintf(stderr, "could not open %s\n", ioparam.outFileName);
		return ERROR_OPEN_FILE;
	}

	return OK;
}

int Read_yuv_data(CodecCtx &ctx, IOPARAM &io_param, int color_plane)
{
	int frame_height = color_plane == 0 ? ctx.frame->height : ctx.frame->height / 2;
	int frame_width = color_plane == 0 ? ctx.frame->width : ctx.frame->width / 2;
	int frame_size = frame_width * frame_height;
	int frame_stride = ctx.frame->linesize[color_plane];                   //ͼ��洢�����

	if (frame_width == frame_stride)
	{
		//��ȺͿ����ȣ�������Ϣ�������
		cout << "yueccheng add test,frame height : " << frame_height << "frame width: " << frame_width
			<< "frame size: " << frame_size << "frame stride: " << frame_stride << endl;
		fread_s(ctx.frame->data[color_plane], frame_size, 1, frame_size, io_param.pFin);
	}
	else
	{
		//���С�ڿ�ȣ�������Ϣ����ռ�֮����ڼ��
		for (int row_idx = 0; row_idx < frame_height; row_idx++)
		{
			fread_s(ctx.frame->data[color_plane] + row_idx * frame_stride, frame_width, 1, frame_width, io_param.pFin);
		}
	}

	return frame_size;
}

int main(int argc, char** argv)
{
	cout << "BEGIN DECODER" << endl;
	IOPARAM io_param;
	if (parse_input_parameters(argc, argv, io_param) < 0)
	{
		cout << "parse parameters error!" << endl;
		return -1;
	}

	if (open_file(io_param) < 0)
	{
		cout << "open file error" << endl;
		return -1;
	}

	CodecCtx ctx = { NULL, NULL, NULL };
	int frameIdx = 0;
	int packetIdx = 0;
	int ret, got_output;

	avcodec_register_all();                                     //ע���������������Ƶ�������

	ctx.codec = avcodec_find_encoder(AV_CODEC_ID_H264);         //����CODEC_ID���ұ����������ʵ����ָ��
	if (!ctx.codec)
	{
		fprintf(stderr, "Codec not found\n");
		return -1;
	}

	ctx.c = avcodec_alloc_context3(ctx.codec);                  //����AVCodecContextʵ��
	if (!ctx.c)
	{
		fprintf(stderr, "could not allocate video codec contex\n");
		return -1;
	}
	/*���ñ������������*/
	ctx.c->bit_rate = io_param.bitRate;
	ctx.c->width = io_param.nImageWidth;
	ctx.c->height = io_param.nImageHeight;

	/*frames per second*/
	AVRational rational = { 1, 25 };
	ctx.c->time_base = rational;
	ctx.c->pix_fmt = AV_PIX_FMT_YUV420P;

	av_opt_set(ctx.c->priv_data, "preset", "slow", 0);

	if (avcodec_open2(ctx.c, ctx.codec, NULL) < 0)    //���ݱ����������Ĵ򿪱�����
	{
		fprintf(stderr, "could not open codec\n");
		exit(1);
	}

	ctx.frame = av_frame_alloc();
	if (!ctx.frame)
	{
		fprintf(stderr, "Could not allocate video frame\n");
		return false;
	}

	ctx.frame->format = ctx.c->pix_fmt;
	ctx.frame->width = ctx.c->width;
	ctx.frame->height = ctx.c->height;

	//����AVFrame�����������ش洢�ռ�
	ret = av_image_alloc(ctx.frame->data, ctx.frame->linesize, ctx.c->width, ctx.c->height, ctx.c->pix_fmt, 32);
	if (ret < 0)
	{
		fprintf(stderr, "Could not allocate raw picture buffer\n");
		return false;
	}

	for (; frameIdx < io_param.TotalFrame; frameIdx++)
	{
		av_init_packet(&(ctx.pkt));				//��ʼ��AVPacketʵ��
		ctx.pkt.data = NULL;					// packet data will be allocated by the encoder
		ctx.pkt.size = 0;

		fflush(stdout);

		Read_yuv_data(ctx, io_param, 0);		//Y����
		Read_yuv_data(ctx, io_param, 1);		//U����
		Read_yuv_data(ctx, io_param, 2);		//V����

		ctx.frame->pts = frameIdx;   //��ʾʱ���

		/* encode the image */
		//int avcodec_send_frame(AVCodecContext *avctx, const AVFrame *frame);
		//int avcodec_receive_packet(AVCodecContext *avctx, AVPacket *avpkt);
		/****
		avctx: AVCodecContext�ṹ��ָ���˱����һЩ������
		avpkt: AVPacket�����ָ�룬���ڱ������������
		frame��AVframe�ṹ�����ڴ���ԭʼ���������ݣ�
		got_packet_ptr��������������ڱ�ʶAVPacket���Ƿ��Ѿ�����������һ֡��
		����ֵ�������Ƿ�ɹ����ɹ�����0��ʧ���򷵻ظ��Ĵ�����
		****/
		ret = avcodec_encode_video2(ctx.c, &(ctx.pkt), ctx.frame, &got_output);	//��AVFrame�е�������Ϣ����ΪAVPacket�е�����
		if (ret < 0)
		{
			fprintf(stderr, "Error encoding frame\n");
			exit(1);
		}
		printf("Encode frame index: %d, frame pts: %d.\n", frameIdx, ctx.frame->pts);
		if (got_output)
		{
			//���һ��������������
			printf("Write packets %3d (size=%5d). Packet pts: %d\n", packetIdx++, ctx.pkt.size, ctx.pkt.pts);
			fwrite(ctx.pkt.data, 1, ctx.pkt.size, io_param.pFout);
			av_packet_unref(&(ctx.pkt));
		}
	}
	/* get the delayed frames */	
	for (got_output = 1; got_output; frameIdx++)
	{
		fflush(stdout);

		ret = avcodec_encode_video2(ctx.c, &(ctx.pkt), NULL, &got_output);		//�����������ʣ�������
		if (ret < 0)
		{
			fprintf(stderr, "Error encoding frame\n");
			exit(1);
		}

		if (got_output)
		{
			printf("Cached frames: Write packets %3d (size=%5d). Packet pts: %d\n", packetIdx++, ctx.pkt.size, ctx.pkt.pts);
			fwrite(ctx.pkt.data, 1, ctx.pkt.size, io_param.pFout);
			av_packet_unref(&(ctx.pkt));
		}
	} //for (got_output = 1; got_output; frameIdx++) 

	fclose(io_param.pFin);
	fclose(io_param.pFout);

	avcodec_close(ctx.c);
	av_free(ctx.c);
	av_freep(&(ctx.frame->data[0]));
	av_frame_free(&(ctx.frame));
	system("pause");
	return 0;
}
