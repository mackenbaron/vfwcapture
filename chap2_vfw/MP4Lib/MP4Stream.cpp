#include "stdafx.h"
#include "MP4Stream.h"




int MP4Stream::InitCode(EncodeInfo* p_Info)
{
	m_Info=p_Info;
	//m_EncodeThread=pthread_t();

	
	av_register_all();

	m_OutFmt=guess_format(NULL, p_Info->m_FileName, NULL);
	//m_OutFmt->audio_codec=CODEC_ID_MP3;
	m_OutFmt-> video_codec = (CodecID ) CODEC_ID_H264 ; 
	if(m_OutFmt->video_codec!=CODEC_ID_NONE)
	{
		m_EnableVideo=TRUE;
	}
	if(m_OutFmt->audio_codec!=CODEC_ID_NONE)
	{
		m_EnableAudio=TRUE;
	}

	if(!m_OutFmtCtx)
	{
		m_OutFmtCtx = av_alloc_format_context();
	}

	if (!m_OutFmtCtx) {
		fprintf(stderr, "Memory error\n");
		exit(1);
	}

	m_OutFmtCtx->oformat=m_OutFmt;


	if(m_EnableVideo){
		AddVideoStream(m_OutFmtCtx,m_OutFmt->video_codec);
	}



	if(m_EnableAudio){
		AddAudioStream(m_OutFmtCtx, m_OutFmt->audio_codec);
	}

	/* set the output parameters (must be done even if no
	parameters). */
	if (av_set_parameters(m_OutFmtCtx, NULL) < 0) {
		fprintf(stderr, "Invalid output format parameters\n");
		exit(1);
	}

	//	dump_format(m_OutFmtCtx,0,m_Info->m_FileName,1);


	// now that all the parameters are set, we can open the audio and
	// video codecs and allocate the necessary encode buffers 
	if (m_EnableVideo){
		OpenVideo();
	}
	// now that all the parameters are set, we can open the audio and
	// video codecs and allocate the necessary encode buffers 
	if (m_EnableAudio){
		OpenAudio();
	}

	/* open the output file */  
	if (!(m_OutFmt-> flags & AVFMT_NOFILE )) {  
		if ( url_fopen(&m_OutFmtCtx -> pb, m_Info->m_FileName, URL_WRONLY) < 0) {  
			fprintf( stderr, "Could not open '%s'\n" ,  m_Info->m_FileName );  
			exit(1);  
		}  
	}  

	/* write the stream header, if any */  
	if( av_write_header (m_OutFmtCtx ) < 0)  
	{  
		fprintf( stderr, "Could not write header for output file\n" );  
		return -1;  
	}  

	int a=pthread_create(&m_EncodeThread,NULL,MP4Stream::ThreadEncode,this);
	
}
int MP4Stream::Close()
{
	if(m_IsClose)
	{
		return 0;
	}
	m_IsClose=TRUE;
	while(m_IsEncoding)
	{
		Sleep(50);
	}

	pthread_cancel(m_EncodeThread);


	if(m_OutFmtCtx){
		av_write_trailer (m_OutFmtCtx );
		/* free the streams */  
		for(int i = 0; i < m_OutFmtCtx-> nb_streams; i++) {  
			av_freep(& m_OutFmtCtx-> streams[ i]-> codec);  
			m_OutFmtCtx-> streams[ i]-> codec=NULL;
			av_freep(& m_OutFmtCtx-> streams[ i]);  
			 m_OutFmtCtx-> streams[ i]=NULL;
		}
		
		if (!(m_OutFmt-> flags & AVFMT_NOFILE )) {  
			/* close the output file */  
			url_fclose( m_OutFmtCtx-> pb);  
		}  
		av_free( m_OutFmtCtx);
		m_OutFmtCtx=NULL;
	}

	CloseVideo();

	CloseAudio();





	return 0;
}

MP4Stream::MP4Stream()
{
	m_Datas=std::queue<MediaData>();
}
MP4Stream::~MP4Stream()
{
	Close();
}
void* MP4Stream::ThreadEncode(void* args)
{
	MP4Stream* stream=(MP4Stream*)args;	
	stream->m_IsEncoding=TRUE;
	while(1)
	{
		if(stream->m_Datas.empty())
		{
			if(stream->m_IsClose)
			{
				break;
			}
			Sleep(500);
			continue;
		}

		MediaData data= stream->m_Datas.front();
		
		if(data.IsVideo){
			stream->EncodeVideo((uint8_t *)data.Data,data.len);
		}
		else
		{
			stream->EncodeAudio((short *)data.Data,data.len);
		}
		stream->m_Datas.pop();
		free(data.Data);
		
	}
	stream->m_IsEncoding=FALSE;
	return 0;
}