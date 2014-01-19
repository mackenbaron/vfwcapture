#include "stdafx.h"
#include "MP4File.h"

int MP4File::Open(char* p_Path,char* p_FileName)
{
	m_FileName=p_FileName;
	int pathlen=strlen(p_Path);
	int filenamelen=strlen(p_FileName);
	m_FullPath=(char*)malloc(pathlen+filenamelen*2);
	
	if(pathlen>0){
	strcpy_s(m_FullPath,pathlen,p_Path);
	}

	if(filenamelen>0){
	strcpy_s(m_FullPath,filenamelen*2,p_FileName);
	}
	fopen_s(&m_File,m_FullPath,"wb");
	return 0;
}
int MP4File::WriteAudio(void* data,size_t size,size_t count)
{
	fwrite(data,size,count,m_File);
	return 0;
}
int MP4File::WriteVideo(void* data,size_t size,size_t count)
{
	fwrite(data,size,count,m_File);
	return 0;
}
int MP4File::Close()
{
	if(m_File){
	fclose(m_File);
	}
	m_File=NULL;
	return 0;
}
MP4File::MP4File()
{
	m_File=NULL;
}
MP4File::~MP4File()
{
	Close();
}
