#include "widget.h"
#include "ui_widget.h"
#include<iostream>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    _ButtonGroup=new QButtonGroup(this);
    _ButtonGroup->addButton(ui->_InsertRadioButton,1);
    _ButtonGroup->addButton(ui->_RemoveRadioButton,2);
    connect(_ButtonGroup,static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
            [=](int id){ _fuck=id;ui->_TextBrowserInfo->append(QString("Current operation:%1").arg(_ButtonGroup->button(id)->text())); });
    connect(ui->_TreeShow,&TreeShow::setpix,
            [this](){ui->_TextBrowserInfo->append(QString("Resize TreeShow size:%1 %2")
                                                  .arg(ui->_TreeShow->width())
                                                  .arg(ui->_TreeShow->height()));});
    _timer.setInterval(500);
    connect(&_timer,&QTimer::timeout,[=](){ui->_TreeShow->drawPicture(ii++,_fuck);});
}

Widget::~Widget()
{
    delete ui;
}
template<typename T>
void Widget::showInfo(const QString _templeteString,const T & value)
{
    ui->_TextBrowserInfo->append(_templeteString.arg(value));
}

void Widget::on__PushButtonStep1_clicked()
{
    if(_whichTabWidget==0){
        auto arrays=ui->_LineEditForData->text().split(" ");
        for(auto x:arrays){
            ui->_TreeShow->drawPicture(x.toInt(),_fuck);
        }
        showInfo("Insert: %1",ui->_LineEditForData->text());
        ui->_LineEditForData->setFocus();
        ui->_LineEditForData->clear();
    }else if(_whichTabWidget==1){
        ui->_TreeShow->clearPicture();
        int _count=ui->spinBox->value();
        int _rangeLeft=ui->_RangeLeft->text().toInt();
        int _rangeRight=ui->_RangeRight->text().toInt();
        int bb=_rangeRight-_rangeLeft;
        if(bb<0){
            showInfo("Wrong range:","");
            return;
        }
        std::srand(time(0));

        for(auto a=0;a<_count;a++)
            ui->_TreeShow->drawPicture(_rangeLeft+rand()%bb,_fuck);
    }else
        showInfo("unimplement","");
}

void Widget::on__PushButtonInitial_clicked()
{
    ui->_TreeShow->clearPicture();
}


void Widget::on_tabWidget_tabBarClicked(int index)
{
    _whichTabWidget=index;
    ui->_TextBrowserInfo->append(QString("Current tabwidget: %1").arg(index==0?"自定义数据":index==1?"随机指定数据":"模拟"));
}

void Widget::on__PushButtonStart_clicked()
{
    _timer.start();
}

void Widget::on__PushButtonStop_clicked()
{
    _timer.stop();
}
