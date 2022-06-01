#pragma once
#include <QThread>
#include <list>
class RAudioThread : protected QThread
{
public:

	int sampleRate = 44100;
	int channels = 2;
	int sampleByte = 2;
	int cacheSize = 10;
	int nbSample = 1024;

	static RAudioThread *Get()
	{
		static RAudioThread ct;
		return &ct;
	}

	//���ؿռ����û�����
	char *GetPCM();
	void Start();
	void Stop();
	void run();

	virtual ~RAudioThread();

protected:
	bool isExit = false;
	std::list <char *> pcms;
	QMutex mutex;
	RAudioThread();
};

