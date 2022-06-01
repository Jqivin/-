#pragma once
#pragma execution_character_set("utf-8")
#include <QtWidgets/QWidget>
#include "ui_rscreen.h"
#include <QTimer>

class RScreen : public QWidget
{
    Q_OBJECT

public:
    RScreen(QWidget *parent = Q_NULLPTR);
	QPoint pt;                         //��¼���λ�õĵ�
	int width = 1920;                         //¼����Ƶ�Ŀ�
	int height = 1080;							//¼�Ƶ���Ƶ�ĸ�
	int fps = 10;							//��Ƶ֡��
	QString fmt = ".mp4";						//��Ƶ��ʽ

protected:
	void mousePressEvent(QMouseEvent* e);
	void mouseMoveEvent(QMouseEvent* e);
private:
    Ui::RScreenClass ui;
	void myUiSet();
	void connectFun();
	QTimer timer;           //��ʱ��
	void setLabelPixmap();
public slots:
	void on_changeFileBtn();
	void on_recordButton();
	void on_timeout();
	void on_upLoadBtn();
};
