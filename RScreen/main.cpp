#include "rscreen.h"
#include <QtWidgets/QApplication>
#include "RCaptureThread.h"
#include "RAudioThread.h"
#include "RScreenRecord.h"
#include <iostream>
#include <QDateTime>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RScreen w;
    w.show();

	/*������Ļ¼����	
	RCaptureThread::Get()->Start();
	//QThread::msleep(3000);
	for (;;)
	{
		char* c = RCaptureThread::Get()->GetRGB();
		if (c)
		{
			std::cout << "+";
		}
	}
	RCaptureThread::Get()->Stop();
	*/
	/*������Ƶ¼����
	
	*/
	/*RAudioThread::Get()->Start();
	for (;;)
	{
		char *pcm = RAudioThread::Get()->GetPCM();
		if (pcm)
		{
			std::cout << "*";
		}
	}*/

	/*����RScreenRecord
	*/
	/*QDateTime t = QDateTime::currentDateTime();
	��ʱ�䴴���ļ���
	QString filename = t.toString("yyyyMMdd_hhmmss"); 
	filename = "rscreen_" + filename;
	filename += ".mp4";
	RScreenRecord::Get()->Start(filename.toLocal8Bit());
	getchar();
	RScreenRecord::Get()->Stop();*/

    return a.exec();
}
