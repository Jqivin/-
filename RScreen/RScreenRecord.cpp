#include "RScreenRecord.h"

#include "RCaptureThread.h"
#include "RAudioThread.h"
#include "RVideoWriter.h"
#include <iostream>
using namespace std;
bool RScreenRecord::Start(const char *filename)
{
	if (!filename)return false;
	Stop();
	mutex.lock();
	isExit = false;

	//��ʼ����Ļ¼��
	//fps�Ǳ����һ����Ա���� ���Դ����洫�ݹ���
	//���滹��һ��fps
	RCaptureThread::Get()->fps = fps; 
	RCaptureThread::Get()->Start();

	//��ʼ����Ƶ¼��
	//��Ƶ�Ĳ�����ʱ��ʹ��Ĭ�ϲ��� Ҳ���Դ����洫�ݽ���
	RAudioThread::Get()->Start();

	//��ʼ��������(RVideoWriter)
	//��Ҫ��һЩ����������
	RVideoWriter::Get()->inWidth = RCaptureThread::Get()->width;   //����Ŀ�� Ҳ����ȫ��
	RVideoWriter::Get()->inHeight = RCaptureThread::Get()->height;  //����ĸ߶�
	RVideoWriter::Get()->outWidth = outWidth;                 //rgbת����yuv֮�����Ŀ��
	RVideoWriter::Get()->outHeight = outHeight;
	RVideoWriter::Get()->outFPS = fps;
	//��ʼ��
	RVideoWriter::Get()->Init(filename);
	//�����Ƶ��
	RVideoWriter::Get()->AddVideoStream();
	//�����Ƶ��
	RVideoWriter::Get()->AddAudioStream();
	if (!RVideoWriter::Get()->WriteHead())
	{
		//���ʧ�ܣ����߳�stop
		mutex.unlock();
		Stop();
		return false;
	}
	//����ɹ������߳�
	mutex.unlock();
	start();
	return true;
}
void RScreenRecord::Stop()
{
	mutex.lock();
	isExit = true;
	mutex.unlock();
	wait();
	mutex.lock();
	//д��β����Ϣ���ر�
	RVideoWriter::Get()->WriteEnd();
	RVideoWriter::Get()->Close();
	//��Ļ����Ƶ�̹߳ر�
	RCaptureThread::Get()->Stop();
	RAudioThread::Get()->Stop();
	mutex.unlock();
}
void RScreenRecord::run()
{


	while (!isExit)
	{
		mutex.lock();

		//д����Ƶ
		//���ȣ���RCaptureThread��ԭʼ����
		if (RVideoWriter::Get()->IsVideoBefor())
		{
			char *rgb = RCaptureThread::Get()->GetRGB();
			if (rgb)
			{
				//��������֮�� ��ʼ����
				AVPacket *p = RVideoWriter::Get()->EncodeVideo((unsigned char*)rgb);
				delete rgb;
				//д����Ƶ֡
				RVideoWriter::Get()->WriteFrame(p);
				//cout << "@";
			}
		}
		else
		{
			//д����Ƶ
			//��ȡ��Ƶ
			char *pcm = RAudioThread::Get()->GetPCM();
			if (pcm)
			{
				AVPacket *p = RVideoWriter::Get()->EncodeAudio((unsigned char*)pcm);
				delete pcm;
				RVideoWriter::Get()->WriteFrame(p);
				//cout << "#";
			}
		}
		/*
		char *rgb = RCaptureThread::Get()->GetRGB();
		if (rgb)
		{
			//��������֮�� ��ʼ����
			AVPacket *p = RVideoWriter::Get()->EncodeVideo((unsigned char*)rgb);
			delete rgb;
			//д����Ƶ֡
			RVideoWriter::Get()->WriteFrame(p);
			cout << "@";
		}

		//д����Ƶ
		//��ȡ��Ƶ
		char *pcm = RAudioThread::Get()->GetPCM();
		if (pcm)
		{
			AVPacket *p = RVideoWriter::Get()->EncodeAudio((unsigned char*)pcm);
			delete pcm;
			RVideoWriter::Get()->WriteFrame(p);
			cout << "#";
		}
		*/
		msleep(10);
		mutex.unlock();
	}
}

RScreenRecord::RScreenRecord()
{
}


RScreenRecord::~RScreenRecord()
{
}
