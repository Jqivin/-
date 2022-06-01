#pragma once

#include <QWidget>
#include "ui_client.h"
#include<winsock2.h>
#include "login.h"
class client : public QWidget
{
	Q_OBJECT

public:
	client(QWidget *parent = Q_NULLPTR);
	~client();
	login* log_ui;     //��¼����

	private slots:
	void on_upLoad_clicked();
	void on_download_clicked();
	void on_ls_clicked();
	void on_deleteFile_clicked();
	void on_loginBtn_clicked();

	//QPoint pt;
	

private:
	Ui::client ui;
	SOCKET sclient;
	//void up_file(const QString & filename);
	void ConnectToHost();
	void up_file(const char* filename);
	void down_file(char* filename);
	void display_file();
	void delete_file(const QString& file);
	void CreateListWidget(const char* str);

	//��¼�Ƿ��¼��״̬
	bool isLogined;

protected:
	//void mousePressEvent(QMouseEvent* e);
	//void mouseMoveEvent(QMouseEvent* e);
};


