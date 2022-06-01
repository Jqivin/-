#include "rscreen.h"
#include <QMouseEvent>
#include <QFileDialog>
#include <iostream>
#include <QTimer>
#include <QTime>
#include <QScreen>
#include <QtWidgets/QApplication>
#include <QPixmap>
#include <QDesktopWidget>
#include "RScreenRecord.h"
#include <QDateTime>
#include "client.h"

//��¼�Ƿ�ʼ��¼��
static bool isRecord = false;
//��¼¼�Ƶ�ʱ��
static QTime time;
//��ʼ¼�ư�ť����ʽ��
#define RECORDQSS "\
QPushButton:!hover\
{background-image:url(:/RScreen/img/startRecoding.png);}\
QPushButton:hover\
{background-image:url(:/RScreen/img/pressStartRecoding.png);}\
QPushButton:pressed\
{background-image:url(:/RScreen/img/pressStartRecoding.png);\
background-color:rgba(255,255,255,0);}"

RScreen::RScreen(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
	myUiSet();
	connectFun();
	setWindowFlag(Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);
	//��ʱ����ʼ��
	//timer = new QTimer(this);
	timer.start(1000);
	setLabelPixmap();
}
void RScreen::setLabelPixmap()
{
	static QScreen *src = NULL;
	if (!src)
	{
		src = QGuiApplication::primaryScreen();
	}
	//��ȡȫ��
	QPixmap pix = src->grabWindow(QApplication::desktop()->winId());
	int w = ui.firstImg->width();
	QPixmap pix2 = pix.scaledToWidth(w);
	ui.firstImg->setPixmap(pix2);
}
void RScreen::mousePressEvent(QMouseEvent* e)
{
	//ֻ�е��������в���
	if (e->button() == Qt::LeftButton)
	{
		//��¼������������һ�̵�λ��
		pt.setX(e->x());
		pt.setY(e->y());
	}
}

void RScreen::mouseMoveEvent(QMouseEvent* e)
{
	// move�ƶ���λ�����൱����Ļ������ϵ
	//event->x() , event->y()���൱�ڴ��ڵ�����ϵ
	this->move(e->globalX() - pt.x(), e->globalY() - pt.y());
}
void RScreen::myUiSet()
{
	//ѡ��¼��ģʽ
	/*QStringList comboxAreaText;
	comboxAreaText << "ȫ��¼��" << "ѡ��¼��";
	foreach(auto x, comboxAreaText)
	{
		ui.comboxArea->addItem(x.toUtf8().data());
	}*/

	//ѡ��¼��֡��
	QStringList comboxFpsText;
	comboxFpsText << "10" << "20" << "24" << "25" << "30" << "60" << "120";
	ui.comboxFps->addItems(comboxFpsText);

	//ѡ��������
	QStringList comboxQingxidu;
	comboxQingxidu << "1080P����" << "720P����" << "480P����";
	ui.comboxqingxidu->addItems(comboxQingxidu);
 
	//ѡ����Ƶ��ʽ
	QStringList comboxFmtText;
	comboxFmtText << ".mp4" << ".avi" << ".wmv" << ".flv";
	ui.comboxFmt->addItems(comboxFmtText);
}

void RScreen::connectFun()
{
	//��¼��ť
	//connect(ui.loginBtn, &QPushButton::clicked, this, &RScreen::on_loginBtn);
	//�ư�ť
	connect(ui.upLoadBtn, &QPushButton::clicked, this, &RScreen::on_upLoadBtn);
	//�����ļ�λ�õİ�ť
	connect(ui.changeFileBtn, &QPushButton::clicked, this, &RScreen::on_changeFileBtn);
	//��ʼ¼�ư�ť
	connect(ui.recordButton, &QPushButton::clicked, this, &RScreen::on_recordButton);
	//��ʱ���ۺ�������
	connect(&timer, &QTimer::timeout, this, &RScreen::on_timeout);
	//֡�ʱ仯
	void(QComboBox::*fun)(const QString&) = &QComboBox::currentIndexChanged;
	connect(ui.comboxFps, fun, this, [=] {
		fps = ui.comboxFps->currentText().toInt();
		std::cout << fps;
	});

	//�����ȱ仯
	void(QComboBox::*funQ)(const QString&) = &QComboBox::currentIndexChanged;
	connect(ui.comboxqingxidu, funQ, this, [=] {
		QString str = ui.comboxqingxidu->currentText();
		if (str == "1080P����")
		{
			width = 1920;
			height = 1080;
		}
		else if (str == "720P����")
		{
			width = 1280;
			height = 720;
		}
		else if (str == "480P����")
		{
			width = 720;
			height = 480;
		}
		std::cout << width << "*" << height;
	});

	//��Ƶ��ʽ�仯
	void(QComboBox::*funFmt)(const QString&) = &QComboBox::currentIndexChanged;
	connect(ui.comboxFmt, funFmt, this, [=] {
		fmt = ui.comboxFmt->currentText();
		std::cout << fmt.toStdString();
	});

}

//�����ļ�λ�õİ�ť�Ĳۺ�����ʵ��
void RScreen::on_changeFileBtn()
{
	//���һ��·��
	QString strPath = QFileDialog::getExistingDirectory();
	if (!strPath.isEmpty())
	{
		//����ui����
		ui.urlEdit->setText(strPath);
		//����·��
		// ...
	}
}

//��ʼ¼�ư�ť�Ĳۺ���
void RScreen::on_recordButton()
{
	isRecord = !isRecord;
	//�����ʼ��¼�ƣ��ٵ����ť���ǹر�¼��
	if (!isRecord)
	{
		ui.recordButton->setStyleSheet(RECORDQSS);
		RScreenRecord::Get()->Stop();
	}
	else
	{
		time.restart();
		QString str = "background-image:url(:/RScreen/img/stopRecoding.png);background-color:rgba(255,255,255,0);";
		ui.recordButton->setStyleSheet(str);
		
		
		QDateTime t = QDateTime::currentDateTime();
		//��ʱ�䴴���ļ���
		QString filename = t.toString("yyyyMMdd_hhmmss");
		filename = "rscreen_" + filename;
		//��Ƶ��ʽ
		filename += fmt;
		filename = ui.urlEdit->text() + "\\" + filename;

		RScreenRecord::Get()->fps = fps;
		RScreenRecord::Get()->outWidth = width;
		RScreenRecord::Get()->outHeight = height;
		if (RScreenRecord::Get()->Start(filename.toLocal8Bit()))
		{
			return;
		}
		
		//timer->start(1000);    //��ʱ��ÿ��ˢ��һ��
		//this->showMinimized();   //��ʼ¼��֮����С��
		
		isRecord = false;
	}
}

void RScreen::on_timeout()
{
	if (isRecord)
	{
		//���ŵ�ʱ�� ����ֵ��һ������
		int elapsedTime = time.elapsed() / 1000;
		int Minutes = elapsedTime / 60;
		int Second = elapsedTime % 60;
		int Hour = elapsedTime / 3600;
		ui.timelabel->setText(QString::asprintf("%02d:%02d:%02d",Hour,Minutes, Second));
	}
}

//�ϴ���ť��ʵ��
void RScreen::on_upLoadBtn()
{
	client* cli_ui = new client;
	cli_ui->show();
}

////��¼��ť�ۺ���ʵ��
//void RScreen::on_loginBtn()
//{
//	login* log_ui = new login;
//	log_ui->show();
//}
