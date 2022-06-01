//#include "client.h"
//
//client::client(QWidget *parent)
//	: QWidget(parent)
//{
//	ui.setupUi(this);
//}
//
//client::~client()
//{
//}
#include "client.h"
#include <QFileDialog>
#include <QDebug>
#include <QFile>
#include<winsock2.h>
#include <stdio.h>
#include <iostream>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QMouseEvent>
#include "login.h"
#include <QDialog>
#pragma comment(lib, "ws2_32.lib")

#pragma execution_character_set("utf-8")

#define UIPAddress "192.168.221.128"
#define DIPAddress "192.168.221.133"

client::client(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setWindowFlag(Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);
	//connect(ui.connect, &QPushButton::clicked, this, &client::ConnectToHost);  
	//����Ҫʹ�ð�ť�� �򿪾������������
	ConnectToHost();
	isLogined = false;   //��¼״̬
	log_ui = NULL;
	//ConnectToHost();

	
}

client::~client()
{
}

//void client::mousePressEvent(QMouseEvent* e)
//{
//	//ֻ�е��������в���
//	if (e->button() == Qt::LeftButton)
//	{
//		//��¼������������һ�̵�λ��
//		pt.setX(e->x());
//		pt.setY(e->y());
//	}
//}
//
//void client::mouseMoveEvent(QMouseEvent* e)
//{
//	// move�ƶ���λ�����൱����Ļ������ϵ
//	//event->x() , event->y()���൱�ڴ��ڵ�����ϵ
//	this->move(e->globalX() - pt.x(), e->globalY() - pt.y());
//}

//���ӷ�����
void client::ConnectToHost()
{
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA data;
	if (WSAStartup(sockVersion, &data) != 0)
	{
		return;
	}

	sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sclient == INVALID_SOCKET)
	{
		printf("invalid socket!");
		return;
	}

	sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(6000);
	serAddr.sin_addr.S_un.S_addr = inet_addr(UIPAddress);
	if (::connect(sclient, (sockaddr*)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
	{  //����ʧ��
		printf("connect error !");
		closesocket(sclient);
		return;
	}
	std::cout << "connect ok";

	//ˢ��һ���ļ��б�
	//display_file();
}

//��¼��ť�ۺ���ʵ��
void client::on_loginBtn_clicked()
{
	if (isLogined) {
		//QMessageBox::information(this, "", "���Ѿ���¼�������ظ���¼");
		int res = QMessageBox::question(this, "", "���Ѿ����ڵ�¼״̬���Ƿ��˳���¼��", QMessageBox::StandardButton::Yes, QMessageBox::StandardButton::Cancel);
		if (res == QMessageBox::StandardButton::Yes)
		{
			isLogined = false;
			ui.loginBtn->setText("δ��¼");
			ui.loginBtn->setStyleSheet("color: rgb(211, 211, 211);");
			ui.listWidget->clear();
		}
		return;
	}
	if(log_ui == NULL)
		log_ui = new login;
	log_ui->ui.stackedWidget->setCurrentWidget(log_ui->ui.pageLogin);
	log_ui->show();
	log_ui->setSocket(sclient);
	connect(log_ui, &login::isLogin, this, [&](QString &str) {
		isLogined = true;
		ui.loginBtn->setText(str);
		ui.loginBtn->setStyleSheet("color: rgb(10, 130, 10);");
		log_ui->close();
		display_file();
	});
}

//�ϴ��ļ��İ�ť
void client::on_upLoad_clicked()
{
	QString fileName = QFileDialog::getOpenFileName(this, "get file", "/", "*.*");
	if (fileName.isEmpty()) {
		qDebug() << "file is not exist 57" << fileName;
		return;
	}
	QByteArray arr = fileName.toUtf8();   //�������ʡ�ԣ�����д��fileName.toUtf8().data();
	char* filename = arr.data();
	//up_file(fileName.toStdString().c_str());    error
	up_file(filename);

}

//�����ļ��İ�ť
void client::on_download_clicked()
{
	QString file;
	QListWidgetItem* item = ui.listWidget->currentItem();
	if (!item)
	{
		QMessageBox::warning(this, "Warning", "����ѡ��Ҫ���ص��ļ�");
		return;
	}
	file = item->text();
	QByteArray arr = file.toUtf8();
	char* filename = arr.data();

	std::cout << filename << std::endl;

	QString text = QString("%1%2%3").arg("�Ƿ������ļ�").arg(filename).arg("?");
	QMessageBox::StandardButton res;
	res = QMessageBox::question(this, "DownLoad", text, QMessageBox::Yes | QMessageBox::Cancel);
	if (res == QMessageBox::Yes)
		down_file(filename);
}

//չʾ�ļ��İ�ť
void client::on_ls_clicked()
{
	//char *cmd = "ls";
	display_file();
}

//ɾ���ļ��İ�ť
void client::on_deleteFile_clicked()
{
	QListWidgetItem* item = ui.listWidget->currentItem();
	QString filename = item->text();
	QString text = QString("%1%2%3").arg("ȷ��ɾ���ļ�").arg(filename).arg("?");
	QMessageBox::StandardButton res;
	res = QMessageBox::question(this, "ȷ��", text, QMessageBox::Ok | QMessageBox::Cancel);
	if (res == QMessageBox::Ok)
		delete_file(filename);
	else
		return;
}
//ɾ���ļ�
void client::delete_file(const QString& file)
{
	//char* buff = file.toLocal8Bit().data();
	//char* buff = file.toUtf8().data();
	QByteArray ba = file.toLatin1();   //must ����д������ģ�ֻ�ܷ�Ϊ�������
	char* buff = ba.data();
	char cmd[128] = "rm ";
	strcat(cmd, buff);
	send(sclient, cmd, strlen(cmd), 0);
	char recv_buff[1024] = { 0 };
	int n = recv(sclient, recv_buff, 1023, 0);
	if (n <= 0)   //�������ر�
	{
		printf("ser close\n");
	}
	printf("%s\n", recv_buff);
	QMessageBox::information(this, "ok", QString("�ɹ�ɾ���ļ�") + file);
	//ˢ��һ���ļ��б�
	display_file();

}
//չʾ�ļ�
void client::display_file()
{
	char buff[128] = "ls";
	send(sclient, buff, strlen(buff), 0); //���ܵ����ls��rm a.c b.c , cp a.c b.c
	char recv_buff[1024] = { 0 };
	int n = recv(sclient, recv_buff, 1023, 0);
	if (n <= 0)   //�������ر�
	{
		printf("ser close\n");
	}
	printf("%s\n", recv_buff);
	//ui.plainTextEdit->appendPlainText(QString(recv_buff));

	CreateListWidget(recv_buff);
}

//����ListWidget
void client::CreateListWidget(const char* str)
{
	ui.listWidget->clear();
	QString files(str);
	qDebug() << files;
	for (int i = 0;; i++)
	{
		QString tmp = files.section("\n", i, i);
		if (tmp.isEmpty())
			break;
		QListWidgetItem* item = new QListWidgetItem(tmp);
		ui.listWidget->addItem(item);
	}

}

//����
void client::down_file(char* filename)
{
	if (filename == NULL)
	{
		//printf("please input filename\n");
		QMessageBox::warning(this, "Warning", "����ѡ��Ҫ���ص��ļ�");
		return;
	}
	char str[128] = "get ";
	strcat(str, filename);
	send(sclient, str, strlen(str), 0);  //get a.c
	char buff_file[128] = { 0 };
	int num = recv(sclient, buff_file, 127, 0); //ok#size  , err#xxx

												//�������쳣�ر���
	if (num <= 0)
	{
		printf("ser close\n");
		closesocket(sclient);
		exit(0);
	}

	if (strncmp(buff_file, "ok#", 3) != 0)   //ǰ�����ַ�!=OK#,��ʾ�յ��������ظ���ʧ�ܱ���
	{
		printf("%s\n", buff_file + 4);
		return;
	}
	//�ɹ��ˣ��յ��˷��������ص��ļ��Ĵ�С
	int filesize = 0;
	sscanf(buff_file + 3, "%d", &filesize); // ok#filesize

	printf("���ص��ļ���СΪ��%d\n", filesize);

	QString path = QFileDialog::getExistingDirectory(this, "����", "/");

	path += "/";
	path += filename;

	//����ľ���·��
	QByteArray arr = path.toUtf8();
	char* absolutePath = arr.data();
	FILE *fp = fopen(absolutePath, "wb");
	if (!fp) //���ļ�ʧ��
	{
		send(sclient, "err", 3, 0);
		return;
	}

	send(sclient, "ok#", 3, 0); //�򿪳ɹ�

	int curr_size = 0; //��ǰ�Ĵ�С
	char data[1024] = { 0 };

	while (1)
	{
		int num = recv(sclient, data, 1024, 0); //�յ�����������
		if (num <= 0)
		{
			//�������ر�
			printf("ser close --  in sending\n");
			closesocket(sclient);
			fclose(fp);
			exit(0);
		}

		fwrite(data, 1, num, fp);
		curr_size += num;
		printf("down:%.2f%%\r", curr_size * 100.0 / filesize); //  
		ui.upProgressBar->setValue(curr_size * 100.0 / filesize);
		//fflush(stdout);
		if (curr_size >= filesize)
		{
			break;
		}
	}
	printf("\n");
	QMessageBox::information(this, "info", "�ļ��������");
	ui.upProgressBar->setValue(0);
	

	fclose(fp);

}
//�ϴ�
void client::up_file(const char* filename)
{
	//qDebug() << "up_file is start";
	if (filename == NULL)    //�ļ���Ϊ��
	{
		printf("You haven't entered the filename yet.\n");
		return;
	}
	//int file_fd = open(filename,O_RDONLY);  //���ļ�
	FILE* fp = fopen(filename, "rb+");
	if (!fp)
	{
		printf("The file named %s is not exist.75\n ", filename);
		return;
	}

	//qDebug() << "open file is ok";
	//ֻ��Ҫ���� ����Ҫ·��

	QString s(filename);
	int pos = s.lastIndexOf("/");
	//�ļ�����  ���·�� ��JAY.mp3
	QString file = s.right(s.length() - pos - 1);
	file = "up " + file;              //��ʽ��up+�ļ���
	QByteArray arr = file.toUtf8();
	char* str = arr.data();

	//char str[128] = "up haigeshi.mp4";    //����д��
	send(sclient, str, strlen(str), 0);     //up filename
	char buff_file[128] = { 0 };
	int num = recv(sclient, buff_file, 127, 0);
	if (num <= 0)
	{
		printf("ser close\n");
		closesocket(sclient);
		exit(0);
	}
	if (strncmp(buff_file, "ok#", 3) != 0)   //ǰ�����ַ�!=OK#,�յ��������ظ���ʧ�ܱ���
	{
		printf("%s\n", buff_file + 4);
		return;
	}
	//������������
	//�����ļ��Ĵ�С
	fseek(fp, 0, SEEK_END);
	int file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	send(sclient, (char*)&file_size, 4, 0);

	//�ȴ��������˴���
	char recv_buff[128] = { 0 };
	int n = recv(sclient, recv_buff, 127, 0);
	if (n <= 0)    //���������쳣
	{
		printf("ser close\n");
		closesocket(sclient);
		fclose(fp);
		exit(0);
	}
	if (strcmp(recv_buff, "FINISHED") == 0)
	{
		printf("Finished:up:100.00%\n");    //�봫
		QMessageBox::information(this, "info", "�ļ��봫���");
		return;
	}
	else        //��ʼ�ϴ�
	{
		int curr_upsize = 0;
		//char size_buff[128] = {0};   //���շ���˷����ı��ģ��ϴ��Ĵ�С���ߴ�����Ϣ��
		char send_buff[1024] = { 0 };
		QString str = "";
		std::cout << "start read" << std::endl;
		while (1)
		{
			int n = fread(send_buff, 1, 1024, fp);
			if (n == 0)
			{
				//�ļ���ȡ���
				//
				break;
			}

			send(sclient, send_buff, n, 0);  //�����ļ�

											 //int m = recv(c,size_buff,127,0);

			int m = recv(sclient, (char*)&curr_upsize, 4, 0);

			if (m <= 0)    //���������쳣
			{
				printf("ser close\n");
				closesocket(sclient);
				fclose(fp);
				exit(0);
			}
			// if(strncmp(size_buff,"ok#",3) != 0)
			// {
			//     printf("%s\n",size_buff+4);    //��ӡ������Ϣ��������writeʧ��
			// }
			// else
			// {
			//     sscanf(size_buff+3,"%d",&curr_upsize); // ok#curr_size
			//     printf("up:%.2f%%\r",curr_upsize * 100.0 / file_size);
			// }
			printf("up:%.2f%%\r", curr_upsize * 100.0 / file_size);
			ui.upProgressBar->setValue(curr_upsize * 100.0 / file_size);

		}
	}
	QMessageBox::information(this, "info", "�ļ��ϴ����");

	//ˢ��һ���ļ��б�
	display_file();
	ui.upProgressBar->setValue(0);

	printf("\n");
}
