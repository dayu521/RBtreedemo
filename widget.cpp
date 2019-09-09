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
        if(id==2){
            showInfo("未实现","");
            return ;
        }
        ui->_TextBrowserInfo->append(
                    QString("Current operation:%1")
                    .arg(_ButtonGroup->button(id)->text()));
    });
    connect(ui->_TreeShow, &TreeShow::setpix, [this]() {
        ui->_TextBrowserInfo->append(QString("Resize TreeShow size:%1 %2")
                                     .arg(ui->_TreeShow->width())
                                     .arg(ui->_TreeShow->height()));
    });

    _timerCommon.setInterval(200);
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
        else {
            showInfo("重复!","");
        }
    });
    connect(ui->_TreeShow,&TreeShow::noMorePicture,[=](){
        _timerForDetail.stop();
        _timerCommon.stop();
        showInfo("模拟完成,已无数据", "");
        ui->_PushButtonForTask->setDisabled(false);
        ui->_CheckBoxForShowDetail->setDisabled(false);
    });
    std::srand(time(0));
    ui->_SpinBoxForSpeed->setValue(_timerCommon.interval());    
}

Widget::~Widget() { delete ui; }
template <typename T>
void Widget::showInfo(const QString _templeteString, const T &value) {
    ui->_TextBrowserInfo->append(_templeteString.arg(value));
}

void Widget::on__InitialTreeButton_clicked() {
    _timerForDetail.stop();
    _taskRead=false;
    _simulationStarted=false;
    ui->_TreeShow->clearPictureForDraw();
    ui->lcdNumber->display(0);
    ui->lcdNumber_2->display(0);
    ui->_PushButtonForTask->setDisabled(false);
    ui->_PushButtonStep1_4->setDisabled(false);
    ui->_CheckBoxForShowDetail->setDisabled(false);
    showInfo("初始化完成","");
}

void Widget::on_tabWidget_tabBarClicked(int index) {
    _whichTabWidget = index;
    ui->_TextBrowserInfo->append(
                QString("Current tabwidget: %1")
                .arg(index == 0 ? "体验"
                                : index == 1 ? "模拟" : "模拟"));
}

void Widget::on__PushButtonStart_clicked() {
    if (ui->_CheckBoxForShowDetail->isChecked()) {
        if (_timerForDetail.isActive())
            return;
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
//    ui->_TreeShow->clearPictureForDraw();
    ui->_TreeShow->setIsClear(false);
    auto currentNodeItemCount=ui->_TreeShow->getAllTreeNodeCount();
    auto _whichToChose=ui->comboBox->currentIndex();
    int _count = ui->spinBox->value();
    int _rangeLeft = ui->_RangeLeft->text().toInt();
    int _rangeRight = ui->_RangeRight->text().toInt();
    int bb = _rangeRight - _rangeLeft;
    if (bb < 0) {
        showInfo("Wrong range:", "");
        return;
    }
    if(_rangeRight>10000||_rangeLeft<-9998){
        showInfo("范围是-9997到9999","");
        return;
    }

    if(_whichToChose==0){   //随机
        for (auto a = 0; a < _count; a++) {
            ui->_TreeShow->taskOfInsert(_rangeLeft + rand() % bb);
        }
        showInfo("随机任务生成成功:插入 %1 个节点",ui->_TreeShow->getAllTreeNodeCount()-currentNodeItemCount);
    }else if(_whichToChose==1){     //从小到大
        for (auto a = _rangeLeft; a <= _rangeRight; a++) {
            ui->_TreeShow->taskOfInsert(a);
        }
        showInfo("顺序任务生成成功:插入 %1 个节点",ui->_TreeShow->getAllTreeNodeCount()-currentNodeItemCount);
    }else if(_whichToChose==2){     //自定义
        showInfo("暂未实现","");
        return ;
    }else {
        return ;
    }
    showInfo("当前可进行放映模拟过程","");
    ui->_PushButtonForTask->setDisabled(true);
    ui->_PushButtonStep1_4->setDisabled(true);
    _numberOfValueLeft=ui->_TreeShow->getAllTreeNodeCount()-currentNodeItemCount;
    _taskRead=true;
}

void Widget::on__deleteButton_clicked()
{
    auto x=ui->_LineEditForData->text().toInt();
    ui->_TreeShow->taskOfRemove(x);
    showInfo("删除: %1",x);
    if(ui->checkBox->isChecked())
        return ;
    if(ui->_CheckBoxForShowDetail->isChecked()){
        if (_timerForDetail.isActive())
            return;
        _timerForDetail.start();
        ui->_CheckBoxForShowDetail->setDisabled(true);
        _simulationStarted=true;
    }else {
        ui->_TreeShow->drawPicture(0,0);
    }
}

void Widget::on_checkBox_stateChanged(int arg1)
{
    if(!ui->checkBox->isChecked()){
        ui->_prevStepButton->setDisabled(true);
        ui->_nextStepButton->setDisabled(true);
    } else {
        ui->_prevStepButton->setDisabled(false);
        ui->_nextStepButton->setDisabled(false);
    }

}

void Widget::on_spinBox_2_valueChanged(int arg1)
{
    _timerForDetail.stop();
    ui->_TreeShow->setTreeNodeDiameter(arg1);
    if(_simulationStarted)
        ui->_TreeShow->takeOnePicture();
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

void Widget::showEvent(QShowEvent *event)
{
    showInfo("当前可大约显示: %1 个节点",(ui->_TreeShow->width()/40-1)*2);
    ui->checkBox->setCheckState(Qt::CheckState::Unchecked);
    QWidget::showEvent(event);
}

void Widget::on__insertButton_clicked()
{
    auto x=ui->_LineEditForData->text().toInt();
    ui->_TreeShow->taskOfInsert(x);
    showInfo("插入: %1",x);
    if(ui->checkBox->isChecked())
        return ;
    if(ui->_CheckBoxForShowDetail->isChecked()){
        if (_timerForDetail.isActive())
            return;
        _timerForDetail.start();
        ui->_CheckBoxForShowDetail->setDisabled(true);
        _simulationStarted=true;
    }else {
        ui->_TreeShow->drawPicture(0,0);
    }
}

void Widget::on__initialButton_clicked()
{
    on__InitialTreeButton_clicked();
}


void Widget::on__stopButton2_clicked()
{
    on__PushButtonStop_clicked();
}

void Widget::on__prevStepButton_clicked()
{
    showInfo("暂未实现","");
}


void Widget::on__nextStepButton_clicked()
{
    if(ui->_CheckBoxForShowDetail->isChecked())
        ui->_TreeShow->drawPicture();
    else
        ui->_TreeShow->drawPicture(0,0);
}

void Widget::on_comboBox_currentIndexChanged(int index)
{
    if(index==2)
        showInfo("暂未实现","");
}
