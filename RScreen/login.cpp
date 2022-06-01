#include "login.h"
#include <QIcon>
#include <iostream>
#include<QMessageBox>

#pragma execution_character_set("utf-8")


login::login(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	QIcon icon(":/RScreen/img/recoding.ico");
	setWindowIcon(icon);
	
	
}

//ע�ᰴť
void login::on_RegisterBtn_clicked()
{
	ui.stackedWidget->setCurrentWidget(ui.pageRegister);
}

//ע����淵�ص�¼��ť
void login::on_ReturnLogin_clicked()
{
	ui.stackedWidget->setCurrentWidget(ui.pageLogin);
}

//��¼��ť
void login::on_LoginBtn_clicked()
{
	QString user = ui.LlineEditNum->text();
	QString passwd = ui.LlineEditPasswd->text();

	QString cmd("login ");
	cmd = cmd + user + " " + passwd;
	QByteArray a_cmd = cmd.toUtf8();	
	char* c_cmd = a_cmd.data();

	//���û��������뷢�͸���������֤
	send(sclient, c_cmd, strlen(c_cmd), 0);
	char recv_buff[1024] = { 0 };
	int n = recv(sclient, recv_buff, 1023, 0);
	if (n <= 0)   //�������ر�
	{
		printf("ser close\n");
	}

	if (strncmp(recv_buff, "ok#", 3) == 0)
	{
		//std::cout << "login success" << std::endl;
		QMessageBox::information(this, "�ɹ�", "��¼�ɹ�");
		emit isLogin(user);
	}
	else
	{
		QMessageBox::critical(this, "ʧ��", "��������û������������");
	}


}

//ע�ᰴť
void login::on_RokBtn_clicked()
{
	QString user = ui.RlineEditNumber->text();
	QString passwd = ui.RlineEditPasswd->text();
	QString passwd2 = ui.RlineEditPasswd2->text();
	if (passwd != passwd2) {
		QMessageBox::warning(this, "error", "��������������벻һ��,����������");
		ui.RlineEditPasswd2->clear();
		return;
	}

	//ƴװ����
	QString cmd("register ");
	cmd = cmd + user + " " + passwd;
	QByteArray a_cmd = cmd.toUtf8();
	char* c_cmd = a_cmd.data();

	//���͸�������
	send(sclient, c_cmd, strlen(c_cmd), 0);
	char recv_buff[1024] = { 0 };
	int n = recv(sclient, recv_buff, 1023, 0);
	if (n <= 0)   //�������ر�
	{
		printf("ser close\n");
	}

	if (strncmp(recv_buff, "exist", 5) == 0)
	{
		QMessageBox::warning(this, "�˺��Ѵ���", "��������˺��Ѵ��ڣ���һ����");
		//std::cout << "user have existed" << std::endl;
	}
	else if (strncmp(recv_buff, "err##", 5) == 0)
	{
		QMessageBox::warning(this, "����", "��������æ�����Ժ����԰�");
		//std::cout << "err failed" << std::endl;
	}
	else if (strncmp(recv_buff, "ok###", 5) == 0)
	{
		QMessageBox::information(this, "�ɹ�", "���Ѿ�ע��ɹ���ȥ��¼��");
		//std::cout << "register success" << std::endl;
	}
	
}



login::~login()
{

}

//�ṩ��rscreen�Ľӿ�
void login::setSocket(SOCKET c)
{
	sclient = c;
}
