#include "stdafx.h"
#include "Mp4Stream.h"
#include <vector>
int m_Soundms;

AVStream *m_AudioStream;
int ds;

void Write(AVStream* p_AudioStream,AVFormatContext* p_OutFmtCtx ,uint8_t* buffer, int buffersize)
{
	AVPacket m_AudioPacket; 
	av_init_packet(&m_AudioPacket);

	m_AudioPacket.size=buffersize;
	m_AudioPacket.stream_index= m_AudioStream-> index;  
	m_AudioPacket.data= (uint8_t *)buffer; 
	m_AudioPacket.pts=p_AudioStream->pts.val;
	//m_AudioPacket.dts=m_AudioPacket.pts=ds; //ds++ * (p_AudioStream->codec->frame_size * 1000 / p_AudioStream->codec->sample_rate);
	//ds+=468;//940;
	/* write the compressed frame in the media file */  
	av_interleaved_write_frame(p_OutFmtCtx, &m_AudioPacket);

	//av_write_frame( m_OutFmtCtx, &m_AudioPacket);
	av_free_packet(&m_AudioPacket);

}
void MP4Stream::EncodeAudio(short *p_InSamples,int p_InSize)
{

	int outsize;
	int count= p_InSize/m_AudioStream->codec->frame_size/2;
	std::vector<uint8_t> buffer(AUDIOBUFFER*count);
	int len=0;

	for (int x = 0; x < count; x++)
	{

		outsize = avcodec_encode_audio(m_AudioStream->codec, buffer.data(), AUDIOBUFFER, (const short *) (p_InSamples + (m_AudioStream->codec->frame_size * x)));
		if(outsize>0){
			//len+=outsize;
			Write(m_AudioStream,m_OutFmtCtx, buffer.data(),outsize);
		}
	}
	//Write(m_AudioStream,m_OutFmtCtx, buffer.data(),len);
	std::vector<uint8_t>().swap(buffer);

}


void MP4Stream::OpenAudio()
{
	AVCodecContext *c;
	AVCodec *codec;
	int audio_input_frame_size=0;
	c = m_AudioStream->codec;
	m_Soundms=m_Info->m_SoundSecond/m_AudioStream->codec->time_base.den;//200ms 25fps/s= 8ms/fps
	/* find the audio encoder */
	codec = avcodec_find_encoder(c->codec_id);

	if (!codec) {
		fprintf(stderr, "codec not found\n");
		exit(1);
	}

	/* open it */
	if (avcodec_open(c, codec) < 0) {
		fprintf(stderr, "could not open codec\n");
		exit(1);
	}

	///* init signal generator */
	//t = 0;
	//tincr = (float) (2 * M_PI * 110.0 / c->sample_rate);
	///* increment frequency by 110 Hz per second */
	//tincr2 = (float) (2 * M_PI * 110.0 / c->sample_rate / c->sample_rate);


	/* ugly hack for PCM codecs (will be removed ASAP with new PCM
	support to compute the input frame size in samples */
	if (c->frame_size <= 1) {
		audio_input_frame_size = AUDIOBUFFER / c->channels;
		switch(m_AudioStream->codec->codec_id) {
		case CODEC_ID_PCM_S16LE:
		case CODEC_ID_PCM_S16BE:
		case CODEC_ID_PCM_U16LE:
		case CODEC_ID_PCM_U16BE:
			audio_input_frame_size >>= 1;
			break;
		default:
			break;
		}
	} else {
		audio_input_frame_size = c->frame_size;
	}

}
void MP4Stream::CloseAudio()
{

}

int MP4Stream::AddAudioStream(AVFormatContext *oc, int codec_id)
{
	AVCodecContext *c;


	if (m_OutFmt->audio_codec == CODEC_ID_NONE) {
		return 0;
	}

	m_AudioStream = av_new_stream(oc,1);
	if (!m_AudioStream) {
		fprintf(stderr, "Could not alloc stream\n");
		exit(1);
	}

	c = m_AudioStream->codec;
	c->codec_id = (CodecID)codec_id;
	//c->codec_id = CODEC_ID_MP3;

	c->codec_type = CODEC_TYPE_AUDIO;
	c->bit_rate=128*1000;
	//c->rc_max_rate=c->bit_rate;
	//c->rc_min_rate=c->bit_rate;

	c->time_base.den =m_Info->m_FrameFps;
	c->time_base.num = 1;
	c->gop_size = STREAM_GOP;
	c->sample_fmt=SAMPLE_FMT_S16;//SAMPLE_FMT_FLT;
	c->sample_rate =m_Info->m_WaveFormat->nSamplesPerSec;  //< samples per sec
	c->channels    = m_Info->m_WaveFormat->nChannels;   /* put sample parameters */
	

	//设置为每要个帧打时间戳时的参数，32表示时间戳是32位的，1和1000表示计时   
	//单位为1/1000秒。   
	av_set_pts_info(m_AudioStream,64, 1, 25);  
	
	return 0;
}

void MP4Stream::AddAudio(char *data,int len)
{
	char* dat=(char *)malloc(len);
	memcpy_s(dat,len,data,len);
	MediaData mediadata;
	mediadata.Data=dat;
	mediadata.IsVideo=FALSE;
	mediadata.len=len;
	m_Datas.push(mediadata);
}