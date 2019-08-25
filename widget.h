#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include<QtWidgets>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();
    template<typename T>
    void showInfo(const QString _templeteString,const T & value);

private slots:
    void on__PushButtonStep1_clicked();

    void on__PushButtonInitial_clicked();


    void on_tabWidget_tabBarClicked(int index);

    void on__PushButtonStart_clicked();

    void on__PushButtonStop_clicked();

private:
    Ui::Widget *ui;
    QButtonGroup * _ButtonGroup;
    enum CC{INSERT,REMOVE};
    int _fuck=0;
    int _whichTabWidget=0;
    QTimer _timer;
    int ii=0;
};

#endif // WIDGET_H
