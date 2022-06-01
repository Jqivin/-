#include "RAudioThread.h"
#include <iostream>
#include <QAudioInput>
using namespace std;
static QAudioInput *input = NULL;
static QIODevice *io = NULL;


void RAudioThread::Start()
{
	Stop();
	mutex.lock();
	isExit = false;
	//��ʼ��
	QAudioFormat fmt;
	fmt.setSampleRate(sampleRate);
	fmt.setChannelCount(channels);
	fmt.setSampleSize(sampleByte * 8);
	fmt.setSampleType(QAudioFormat::UnSignedInt);
	fmt.setByteOrder(QAudioFormat::LittleEndian);
	fmt.setCodec("audio/pcm");
	input = new QAudioInput(fmt);
	io = input->start();
	mutex.unlock();
	start();
}

void RAudioThread::Stop()
{
	mutex.lock();
	isExit = true;
	while (!pcms.empty())
	{
		delete pcms.front();
		pcms.pop_front();
	}
	//��new�Ŀռ������
	if (input)
	{
		io->close();
		input->stop();
		delete input;
		input = NULL;
		io = NULL;
	}
	mutex.unlock();
	wait();
}


char *RAudioThread::GetPCM()
{
	mutex.lock();
	if (pcms.empty())
	{
		mutex.unlock();
		return NULL;
	}
	char *re = pcms.front();
	pcms.pop_front();
	mutex.unlock();
	cout << "A";
	return re;
}
void RAudioThread::run()
{
	int size = nbSample*channels*sampleByte;
	while (!isExit)
	{
		mutex.lock();
		if (pcms.size() > cacheSize)
		{
			mutex.unlock();
			msleep(5);
			continue;
		}

		char *data = new char[size];
		int readedSize = 0;   //�Ѿ��������ֽ���  
		//����һ֡��Ƶ���ŵ��ڴ浱��
		while (readedSize < size)
		{
			//����Ѿ�׼���õ��ֽ���
			int br = input->bytesReady();
			//��1024�ſ�ʼ��
			if (br < 1024)
			{
				msleep(1);
				continue;
			}
			int s = 1024;

			//���һ�Σ�ʣ�µĲ���1024  ʣ���ٶĶ���
			if (size - readedSize < s)
			{
				s = size - readedSize;
			}
			//��ʼ����+readedSize��ʾָ���ƶ�λ��
			int len = io->read(data + readedSize, s);
			readedSize += len;
		}
		//����֮�󣬽����ݷŵ�data��
		pcms.push_back(data);
		mutex.unlock();
	}
}

RAudioThread::RAudioThread()
{
}


RAudioThread::~RAudioThread()
{
}
