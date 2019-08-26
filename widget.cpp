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
    _timerForDetail.setInterval(200);
    connect(&_timer,&QTimer::timeout,[=](){ui->_TreeShow->drawPicture(ii++,_fuck);});
    connect(&_timerForDetail,&QTimer::timeout,[=](){if(!ui->_TreeShow->drawPicture())
            _timer.stop();});
    connect(ui->_TreeShow,&TreeShow::nextValueReady,[=](){ui->lcdNumber->display(_qvectorForData[++_indexForQvector]);});
    std::srand(time(0));
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


        for(auto a=0;a<_count;a++)
            ui->_TreeShow->drawPicture(_rangeLeft+rand()%bb,_fuck);
    }else
        showInfo("unimplement","");
}

void Widget::on__PushButtonInitial_clicked()
{
    //    ui->_TreeShow->clearPicture();
    if(ui->_CheckBoxForShowDetail->isChecked())
        ui->_TreeShow->clearPictureForDraw();
    _timerForDetail.stop();
    _qvectorForData.empty();
    ui->lcdNumber->display(0);
    ui->_PushButtonForTask->setDisabled(false);
}


void Widget::on_tabWidget_tabBarClicked(int index)
{
    _whichTabWidget=index;
    ui->_TextBrowserInfo->append(QString("Current tabwidget: %1").arg(index==0?"自定义数据":index==1?"随机指定数据":"模拟"));
}

void Widget::on__PushButtonStart_clicked()
{
    if(ui->_CheckBoxForShowDetail->isChecked()){
        if(_timerForDetail.isActive())
            return;
        _timerForDetail.start();
        ui->lcdNumber->display(_qvectorForData[0]);
        ui->_CheckBoxForShowDetail->setDisabled(true);
    }
    else{
        if(_timer.isActive())
            return
                    _timer.start();
    }
}

void Widget::on__PushButtonStop_clicked()
{
    if(ui->_CheckBoxForShowDetail->isChecked()){
        _timerForDetail.stop();
        ui->_CheckBoxForShowDetail->setDisabled(false);
    }
    else
        _timer.stop();
}

void Widget::on__PushButtonForTask_clicked()
{
    ui->_TreeShow->clearPictureForDraw();
    for(auto a=0;a<30;a++){
        auto i=rand()%100;
        ui->_TreeShow->taskOfInsert(i);
        _qvectorForData.append(i);
    }
    ui->_TreeShow->setIsClear(false);
    ui->_PushButtonForTask->setDisabled(true);
}

void Widget::on__PushButtonNext_clicked()
{
    ui->_TreeShow->drawPicture();
}
