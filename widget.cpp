#include "widget.h"
#include <iostream>
#include "ui_widget.h"

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget) {
    ui->setupUi(this);
    ui->_PushButtonStep1_4->setVisible(false);
    _ButtonGroup = new QButtonGroup(this);
    _ButtonGroup->addButton(ui->_InsertRadioButton, 1);
    _ButtonGroup->addButton(ui->_RemoveRadioButton, 2);

    connect(_ButtonGroup,static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),[=](int id) {
        _fuck = id;
        ui->_TextBrowserInfo->append(
                    QString("Current operation:%1")
                    .arg(_ButtonGroup->button(id)->text()));
    });
    connect(ui->_TreeShow, &TreeShow::setpix, [this]() {
        ui->_TextBrowserInfo->append(QString("Resize TreeShow size:%1 %2")
                                     .arg(ui->_TreeShow->width())
                                     .arg(ui->_TreeShow->height()));
    });

    _timerCommon.setInterval(300);
    connect(&_timerCommon, &QTimer::timeout,[=]() {
        ui->_TreeShow->drawPicture(0, _fuck);
    });
    _timerForDetail.setInterval(100);
    connect(&_timerForDetail, &QTimer::timeout, [=]() {
        ui->_TreeShow->drawPicture();
    });
    connect(ui->_TreeShow, &TreeShow::nextValueReady, [=](int a) {
        ui->lcdNumber->display(a);
    });
    connect(ui->_TreeShow, &TreeShow::insertFinish, [=](bool a) {
        if(a)
            ui->lcdNumber_2->display(--_numberOfValueLeft);
    });
    connect(ui->_TreeShow,&TreeShow::noMorePicture,[=](){
        _timerForDetail.stop();
        _timerCommon.stop();
        showInfo("模拟完成", "");
    });
    std::srand(time(0));
    ui->_SpinBoxForSpeed->setValue(_timerCommon.interval());
}

Widget::~Widget() { delete ui; }
template <typename T>
void Widget::showInfo(const QString _templeteString, const T &value) {
    ui->_TextBrowserInfo->append(_templeteString.arg(value));
}

void Widget::on__PushButtonInitial_clicked() {
    _timerForDetail.stop();
    _taskRead=false;
    _simulationStarted=false;
    ui->_TreeShow->clearPictureForDraw();
    ui->lcdNumber->display(0);
    ui->lcdNumber_2->display(0);
    ui->_PushButtonForTask->setDisabled(false);
    ui->_PushButtonStep1_4->setDisabled(false);
    ui->_CheckBoxForShowDetail->setDisabled(false);
}

void Widget::on_tabWidget_tabBarClicked(int index) {
    _whichTabWidget = index;
    ui->_TextBrowserInfo->append(
                QString("Current tabwidget: %1")
                .arg(index == 0 ? "自定义数据"
                                : index == 1 ? "随机指定数据" : "模拟"));
}

void Widget::on__PushButtonStart_clicked() {
    if (ui->_CheckBoxForShowDetail->isChecked()) {
        if (_timerForDetail.isActive()) return;
            _timerForDetail.start();
        ui->_CheckBoxForShowDetail->setDisabled(true);
        _simulationStarted=true;
    } else {
        if (_timerCommon.isActive())
            return ;
        _timerCommon.start();
        _simulationStarted=true;
    }
}

void Widget::on__PushButtonStop_clicked() {
    if (ui->_CheckBoxForShowDetail->isChecked()) {
        _timerForDetail.stop();
        ui->_CheckBoxForShowDetail->setDisabled(false);

    } else
        _timerCommon.stop();
}

void Widget::on__PushButtonForTask_clicked() {
    ui->_TreeShow->clearPictureForDraw();
    ui->_TreeShow->setIsClear(false);
    auto _witchToChose=ui->comboBox->currentIndex();
    int _count = ui->spinBox->value();
    int _rangeLeft = ui->_RangeLeft->text().toInt();
    int _rangeRight = ui->_RangeRight->text().toInt();
    int bb = _rangeRight - _rangeLeft;
    if (bb < 0) {
        showInfo("Wrong range:", "");
        return;
    }
    if(_witchToChose==0){   //随机
        for (auto a = 0; a < _count; a++) {
            ui->_TreeShow->taskOfInsert(_rangeLeft + rand() % bb);
        }
        showInfo("随机任务生成成功:共插入 %1 个节点",ui->_TreeShow->getAllTreeNodeCount());
    }else if(_witchToChose==1){     //从小到大
        for (auto a = _rangeLeft; a <= _rangeRight; a++) {
            ui->_TreeShow->taskOfInsert(a);
        }
        showInfo("顺序任务生成成功:共插入 %1 个节点",ui->_TreeShow->getAllTreeNodeCount());
    }else if(_witchToChose==2){     //自定义
        showInfo("暂未实现","");
        return ;
    }else {
        return ;
    }

    ui->_PushButtonForTask->setDisabled(true);
    ui->_PushButtonStep1_4->setDisabled(true);
    _numberOfValueLeft=ui->_TreeShow->getAllTreeNodeCount();
    _taskRead=true;
}

void Widget::on__PushButtonNext_clicked() { ui->_TreeShow->drawPicture(); }

void Widget::on_checkBox_stateChanged(int arg1)
{
    if(!ui->checkBox->isChecked()){
        ui->_PushButtonStep1->setVisible(false);
        ui->_PushButtonStep1_2->setVisible(false);
    } else {
        ui->_PushButtonStep1->setVisible(true);
        ui->_PushButtonStep1_2->setVisible(true);
    }

}

void Widget::on__PushButtonStep1_4_clicked()
{
//    ui->_TreeShow->setPixMapTo(0,0);

}

void Widget::on_spinBox_2_valueChanged(int arg1)
{
    _timerForDetail.stop();
    ui->_TreeShow->setTreeNodeDiameter(arg1);
    if(_simulationStarted)
        ui->_TreeShow->takePicture();
    showInfo("当前可大约显示 %1 个节点",(ui->_TreeShow->width()/arg1-1)*2);
}

void Widget::on_comboBox_activated(int index)
{
    if(index==1){
        ui->spinBox->setVisible(false);
        ui->label->setVisible(false);
    } else {
        ui->label->setVisible(true);
        ui->spinBox->setVisible(true);
    }
}

void Widget::on__SpinBoxForSpeed_valueChanged(int arg1)
{
    if(ui->_CheckBoxForShowDetail->isChecked()){
        _timerForDetail.setInterval(arg1);
//        _timerForDetail.start();
    }
    else {
        _timerCommon.setInterval(arg1);
//        _timerCommon.start();
    }
}


void Widget::on__CheckBoxForShowDetail_stateChanged(int arg1)
{
    if(arg1==Qt::Checked)
        ui->_SpinBoxForSpeed->setValue(_timerForDetail.interval());
    else
        ui->_SpinBoxForSpeed->setValue(_timerCommon.interval());
}
