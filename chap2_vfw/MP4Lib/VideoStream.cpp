#include "stdafx.h"
#include "Mp4Stream.h"
#include <vector>


/* 5 seconds stream duration */  
#define STREAM_DURATION    5.0  
#define STREAM_FRAME_RATE 25 /* 25 images/s */  
#define STREAM_NB_FRAMES   ((int )(STREAM_DURATION * STREAM_FRAME_RATE ))  
#define STREAM_PIX_FMT PIX_FMT_YUV420P /* default pix_fmt */  

int m_FrameCount;

static int sws_flags = SWS_PRINT_INFO ;  
AVFrame* m_FrameRGB;
AVFrame* pFrameRGBtemp ;
AVFrame* pFrameRGBtemp1;
struct SwsContext* m_ImgConvertCtx;
AVStream *m_VideoStream;
enum PixelFormat m_PixFmt;



static AVFrame *alloc_picture(int pix_fmt, int width, int height)
{
	AVFrame *picture;
	uint8_t *picture_buf;
	int size;

	picture = avcodec_alloc_frame();
	if (!picture)
		return NULL;
	size = avpicture_get_size(pix_fmt, width, height);
	picture_buf = (uint8_t *) av_malloc(size);
	if (!picture_buf) {
		av_free(picture);
		return NULL;
	}
	avpicture_fill((AVPicture *)picture, picture_buf,
		pix_fmt, width, height);
	return picture;
}
int dd;
static void Write(AVStream* p_VideoStream,AVFormatContext* p_OutFmtCtx ,uint8_t* buffer, int buffersize)
{

	AVPacket m_VideoPacket;
	av_init_packet(&m_VideoPacket);
	if(p_VideoStream->codec-> coded_frame ->key_frame ){  
		m_VideoPacket. flags |= PKT_FLAG_KEY ;  
	}
	m_VideoPacket.pts=p_VideoStream->pts.val;
	m_VideoPacket.stream_index =p_VideoStream->index ;  
	m_VideoPacket.data= buffer ;
	//m_VideoPacket.dts=m_VideoPacket.pts=dd;//av_rescale_q(p_VideoStream->codec->frame_number-1,p_VideoStream->time_base,p_VideoStream->time_base);
	//dd+=1;
	//m_VideoPacket.pts= av_rescale_q(p_VideoStream->codec->coded_frame->pts, p_VideoStream->codec->time_base, p_VideoStream->time_base);
	//double d=av_rescale_q(p_VideoStream->codec->coded_frame->pts, p_VideoStream->codec->time_base, p_VideoStream->time_base);
	// not the video_outbuf_size, note!  
	m_VideoPacket. size= buffersize;


	/* write the compressed frame in the media file */  
	av_interleaved_write_frame(p_OutFmtCtx , &m_VideoPacket );
	//av_write_frame (p_OutFmtCtx , &m_VideoPacket );  

	av_free_packet(&m_VideoPacket);

}
void InitVideoFormat(EncodeInfo* p_Info)
{
	m_PixFmt = PIX_FMT_NONE;
	if(!memcmp(&p_Info->m_BitmapInfo ->bmiHeader.biCompression,"I420",4))
	{
		m_PixFmt = PIX_FMT_YUV420P;
	}

	else if(!memcmp(&p_Info->m_BitmapInfo ->bmiHeader.biCompression,"MJPG",4))
	{
		m_PixFmt = PIX_FMT_YUV420P;
	}
	else if(!memcmp(&p_Info->m_BitmapInfo ->bmiHeader.biCompression,"UYVY",4))
	{
		m_PixFmt = PIX_FMT_UYVY422;
	}
	else if(!memcmp(&p_Info->m_BitmapInfo ->bmiHeader.biCompression,"YUY2",4))
	{
		m_PixFmt= PIX_FMT_YUYV422;
	}
	else if(p_Info->m_BitmapInfo ->bmiHeader.biCompression == 0)
	{
		if (p_Info->m_BitmapInfo->bmiHeader.biBitCount == 24)
			m_PixFmt = PIX_FMT_BGR24;
		else if (p_Info->m_BitmapInfo->bmiHeader.biBitCount == 32)
			m_PixFmt = PIX_FMT_BGR32;
	}

}


void MP4Stream::EncodeVideo(uint8_t* p_Buffer, int p_Bufsize)
{

	//TRACE("0\r\n");
	if(!m_VideoStream)
	{
		return;
	}
	// Allocate an AVFrame structure
	if (m_FrameRGB)
	{
		av_free(m_FrameRGB);
		m_FrameRGB = 0;
	}
	m_FrameRGB = avcodec_alloc_frame();
	if(m_FrameRGB==NULL){
		//	return -1;
	}
	char *pin=NULL, *pout=NULL, *pend=NULL, *peol=NULL, ptemp;

	//    static struct SwsContext *img_convert_ctx;

	int sws_flags = SWS_PRINT_INFO;//SWS_DIRECT_BGR

	int width=m_VideoStream->codec->width;
	int height=m_VideoStream->codec->height;
	/* encode the image */
	avpicture_fill((AVPicture *)m_FrameRGB, p_Buffer, m_PixFmt, width, height);

	if (m_PixFmt != PIX_FMT_YUV420P)
	{
		if (m_ImgConvertCtx == NULL)
		{
			m_ImgConvertCtx = sws_getContext(width, height, m_PixFmt,
				width, height, PIX_FMT_YUV420P,
				SWS_PRINT_INFO, NULL, NULL, NULL);
			if (m_ImgConvertCtx == NULL) 
			{
				fprintf(stderr, "Cannot initialize the conversion context\n");
				exit(1);
			}
			pFrameRGBtemp = alloc_picture(m_PixFmt,width,height);
			pFrameRGBtemp1 = alloc_picture(PIX_FMT_YUV420P, width, height);
			if (!pFrameRGBtemp || !pFrameRGBtemp1) 
			{
				fprintf(stderr, "Could not allocate temporary picture\n");
				exit(1);
			}
		}

		memcpy(pFrameRGBtemp->data[0],m_FrameRGB->data[0], avpicture_get_size(m_PixFmt, width, height));
		if(m_PixFmt== PIX_FMT_BGR24)
		{
			pin = (char *) ((pFrameRGBtemp->data[0] + p_Bufsize) - pFrameRGBtemp->linesize[0]);
			pend = (char *) (pFrameRGBtemp->data[0] + (p_Bufsize/2));
			pout = (char *) pFrameRGBtemp->data[0];
			peol = pout + pFrameRGBtemp->linesize[0];

			while (pout < pend) 
			{
				ptemp = *pout;
				*pout = *pin;
				*pin = ptemp;
				pin++;
				pout++;
				if (pout == peol) 
				{
					pin -= ( 2 * pFrameRGBtemp->linesize[0]);
					peol = pout +pFrameRGBtemp->linesize[0];
				}
			}
		}

		avpicture_fill((AVPicture *)pFrameRGBtemp, pFrameRGBtemp->data[0], m_PixFmt,
			width, height);

		sws_scale(m_ImgConvertCtx, pFrameRGBtemp->data, pFrameRGBtemp->linesize,
			0, height, pFrameRGBtemp1->data, pFrameRGBtemp1->linesize);

		m_FrameRGB = pFrameRGBtemp1;


	}

	//if(m_IsClose)
	//{
	//	m_VideoCoding=FALSE;
	//	return;
	//}
	uint8_t *buffer=(uint8_t * )malloc(sizeof(uint8_t)*p_Bufsize);
	int len = avcodec_encode_video(m_VideoStream->codec, buffer, p_Bufsize, m_FrameRGB);

	//write
	if ( len > 0) {   
		Write(m_VideoStream,m_OutFmtCtx,buffer,len);
	}

	free(buffer);
	m_FrameRGB=NULL;
}

void  MP4Stream::OpenVideo()
{
	AVCodec *codec;
	AVCodecContext *c;
	m_FrameCount=1;
	c =m_VideoStream->codec;

	//m_FrameRGB = alloc_picture(c->pix_fmt, c->width, c->height);  
	/* find the video encoder */
	codec = avcodec_find_encoder(c->codec_id);
	if (!codec) {
		fprintf(stderr, "codec not found\n");
		exit(1);
	}
	
	/* open the codec */
	if (avcodec_open(c, codec) < 0) {
		fprintf(stderr, "could not open codec\n");
		exit(1);
	}
	InitVideoFormat(m_Info);

}
void  MP4Stream::CloseVideo()
{
	if(m_ImgConvertCtx){
		av_free(m_ImgConvertCtx);
	}
	if(m_FrameRGB){
		av_free(m_FrameRGB);
	}

	if(pFrameRGBtemp){
		av_free(pFrameRGBtemp);
	}
	if(pFrameRGBtemp1&&pFrameRGBtemp1!=m_FrameRGB){
		av_free(pFrameRGBtemp1);
	}
}

int MP4Stream::AddVideoStream(AVFormatContext *oc, int codec_id)
{

	if (m_OutFmt->video_codec == CODEC_ID_NONE) {
		return -1;
	}
	AVCodecContext *c;

	m_VideoStream = av_new_stream(oc, 0);
	if (!m_VideoStream) {
		fprintf(stderr, "Could not alloc stream\n");
		exit(1);
	}

	c = m_VideoStream->codec;
	c->codec_id = (CodecID)codec_id;
	c->codec_type = CODEC_TYPE_VIDEO;


	/* resolution must be a multiple of two */
	c->width = m_Info->m_BitmapInfo->bmiHeader.biWidth;
	c->height = m_Info->m_BitmapInfo->bmiHeader.biHeight;
	//¹Ì¶¨±àÂëÂÊ
	int kbps=1000;
	c->bit_rate = 1000*kbps;
	c->bit_rate_tolerance =c->bit_rate;
	c->rc_min_rate=c->bit_rate;
	c->rc_max_rate=c->bit_rate;
	c->rc_buffer_size=c->bit_rate;
	c->rc_initial_buffer_occupancy=c->rc_buffer_size*3/4;
	c->rc_buffer_aggressivity=(float)1.0;
	c->rc_initial_cplx=0.5;

	/* time base: this is the fundamental unit of time (in seconds) in terms
	of which frame timestamps are represented. for fixed-fps content,
	timebase should be 1/framerate and timestamp increments should be
	identically 1. */
	c->time_base.den =m_Info->m_FrameFps;
	c->time_base.num = 1;

	c->pix_fmt = PIX_FMT_YUV420P;

	if (c->codec_id == CODEC_ID_MJPEG) 
	{
		c->pix_fmt = PIX_FMT_YUVJ420P;

	}

	if (c->codec_id == CODEC_ID_H264)
	{
		c->dct_algo = 0;
		c->me_pre_cmp=2;
		//c->cqp = 26;
		c->me_method =7;
		c->qmin = 3;
		c->qmax = 31;
		c->max_qdiff = 3;
		//c->qcompress = 0.5;
		c->qblur = 0.5;
		c->nsse_weight = 8;
		c->i_quant_factor = (float)0.8;
		c->b_quant_factor = 1.25;
		c->b_quant_offset = 1.25;

		c->qcompress = 0; 
		c->max_b_frames=0;
		c->gop_size = STREAM_GOP;
		c-> flags |= CODEC_FLAG_GLOBAL_HEADER ;

	}
	av_set_pts_info(m_VideoStream, 64, 1, 90000);  

	return 0;
}

void MP4Stream::AddVideo(char *data,int len)
{
	char* dat=(char *)malloc(len);
	memcpy_s(dat,len,data,len);
	MediaData mediadata;
	mediadata.Data=dat;
	mediadata.IsVideo=TRUE;
	mediadata.len=len;
	m_Datas.push(mediadata);
}