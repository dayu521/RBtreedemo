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

    void on__PushButtonForTask_clicked();

    void on__PushButtonNext_clicked();

    void on_checkBox_stateChanged(int arg1);

    void on__PushButtonStep1_4_clicked();

    void on_spinBox_2_editingFinished();

    void on_spinBox_2_valueChanged(int arg1);

private:
    Ui::Widget *ui;
    QButtonGroup * _ButtonGroup;
    enum CC{INSERT,REMOVE};
    int _fuck=0;
    int _whichTabWidget=0;
    QTimer _timer;
    QTimer _timerForDetail;
    int ii=0;
    bool _taskRead=false;

};

#endif // WIDGET_H
