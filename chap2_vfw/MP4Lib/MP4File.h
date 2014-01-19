
class MP4File
{
private:
	FILE *m_File;
	char *m_FileName;
	char *m_Path;
	char *m_FullPath; 
protected:

public:
	MP4File();
	~MP4File();

	char* GetFileName(){return m_FileName;}
	char* GetPath(){return m_Path;}

	int Open(char* p_Path, char* p_FileName);
	int WriteAudio(void* data,size_t size,size_t count);
	int WriteVideo(void* data,size_t size,size_t count);
	int Close();
};