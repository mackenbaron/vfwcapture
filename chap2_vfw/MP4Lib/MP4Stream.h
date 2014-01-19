#include "stdafx.h"
#include <vector>
#include "MP4File.h"
#include <queue>
#include <pthread.h>

struct EncodeInfo
{
	char *m_Path;
	char *m_FileName;
	PBITMAPINFO m_BitmapInfo;
	LPWAVEFORMATEX m_WaveFormat;
	int m_VideoBitRate;
	int m_AudioBitRate;
	
	int m_FrameFps;
	int m_SoundSecond;
	
};
struct MediaData
{
	char * Data;
	bool IsVideo;
	int len;
};

class MP4Stream
{
private:
	
	EncodeInfo* m_Info;
	bool m_EnableVideo;
	bool m_EnableAudio;
	bool m_IsClose;

	pthread_t m_EncodeThread;
	bool m_IsEncoding;
	std::queue<MediaData> m_Datas;
	AVOutputFormat *m_OutFmt;
	AVFormatContext *m_OutFmtCtx;
	

	static void* ThreadEncode(void* args);

protected:

	void OpenAudio();
	void CloseAudio();
	int AddAudioStream(AVFormatContext *oc, int codec_id);

	void OpenVideo();
	void CloseVideo();
	int  AddVideoStream(AVFormatContext *oc, int codec_id);

public :
	MP4Stream();
	~MP4Stream();


	void AddVideo(char *data,int len);
	void AddAudio(char *data,int len);

	int InitCode(EncodeInfo* p_Info);
	void EncodeAudio(short * p_InSamples,int p_InSize);
	void EncodeVideo(uint8_t* p_Buffer, int p_Bufsize);
	int Close();

};
