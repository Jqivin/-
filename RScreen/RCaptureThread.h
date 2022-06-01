#pragma once
#include <QThread>
class RCaptureThread:public QThread
{
public:
	//out
	int width = 1280;    //��
	int height = 720;   //��
	

	//in
	int fps = 10;  //֡��
	int cacheSize = 3;

	//�̰߳�ȫ,���صĿռ����û��ͷ�
	char *GetRGB();
	void Start();
	void Stop();
	void run();

	static RCaptureThread* Get()
	{
		static RCaptureThread ct;
		return &ct;
	}
	virtual ~RCaptureThread();

protected:
	RCaptureThread();
	QMutex mutex;
	bool isExit = false;     //��¼�̵߳�״̬
	std::list<char *> rgbs;   //��¼ץ�������� char*����
};

