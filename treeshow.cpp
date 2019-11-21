#include "treeshow.h"
#include<QPainter>
#include<QDebug>
#include<iostream>

TreeShow::TreeShow(QWidget *parent):QLabel (parent)
{
    initial();
    prepareBeforeDraw();

    timer.setSingleShot(true);
    connect(&timer,&QTimer::timeout,[&](){
        _IsChangeWidgetSize=false;
        this->update();
        emit setpix();
    });
}

TreeShow::~TreeShow()
{
    delete NIL;
    delete pix;
    emptyRBtree(root);
    emptyRbtreeForDraw(_rootForDraw);
    delete _sentinelForDraw;
    delete _NILForDraw;
}

void TreeShow::initial()
{
    RBtreeNodeInitial();
}

void TreeShow::takeOnePicture()
{
    QPainter pp(pix);
    pix->fill();
    pp.translate(0,_diameter/2);
    QFont font = pp.font();
    font.setPixelSize(_fontSize);
    pp.setFont(font);
    drawAllElement(pp,_rootForDraw);
    pp.end();
    auto x=_step-1;
    if(_arrayForOrder[x]._ope==Operator::Search)
        drawCurrentNodeItem(search(_arrayForOrder[x]));
    update();
}

void TreeShow::taskOfInsert(int i)
{
    _qvectorForData.append(i);
    insert2(i);
}

void TreeShow::taskOfRemove(int i)
{
    remove2(i);
}

void TreeShow::setTreeNodeDiameter(int diameter)
{
    _diameter=diameter;
    _radius=_diameter/2;
    _fontSize=_radius;
    _nodeLineHeight = 4 * _radius;
}

int TreeShow::getTreeNodeDiameter() const
{
    return _diameter;
}

int TreeShow::getAllTreeNodeCount() const
{
    return _nodeSize;
}

void TreeShow::drawPicture(int x, int operation)
{
    if(_step>=_arrayForOrder.size()){
        emit noMorePicture();
        return  ;
    }
    insertAndRemoveOfNodeItem();
    drawAllElement();
    update();
}

void TreeShow::drawPicture()
{
    if(_step>=_arrayForOrder.size()){
        emit noMorePicture();
        return  ;
    }
    dispatchActionAndDraw(_arrayForOrder[_step]);
    update();
    _step++;
}

void TreeShow::clearPictureForDraw()
{
    if(_isClearForDraw)
        return ;
    _sentinelForDraw->_prev=_sentinelForDraw->_next=_sentinelForDraw;
    emptyRBtree(root);
    root=NIL;
    emptyRbtreeForDraw(_rootForDraw);
    _rootForDraw=_NILForDraw;

    _hashForNodeItem.clear();
    _hashForNodeItem.insert(_NILForDraw->_value,_NILForDraw);

    _arrayForOrder.clear();
    _step=0;

    _qvectorForData.clear();
    _indexForQvector=0;

    _nodeSize=0;
    pix->fill();
    _isClearForDraw=true;
    update();
}

void TreeShow::paintEvent(QPaintEvent *event)
{
    //防抖处理
    if(_IsChangeWidgetSize)
        return;
    QPainter p(this);
    p.drawPixmap(0,0,*pix);
}

void TreeShow::prepareBeforeDraw()
{
    pix=new QPixmap(_pixWidth,_pixHeight);
    pix->fill();
}

void TreeShow::insertNodeItemIntoLinkedList(TreeShow::NodeItem *_nodeItem)
{
    NodeItem * _pCurrent=_sentinelForDraw->_prev;
    while (_pCurrent!=_sentinelForDraw&&_nodeItem->_value<_pCurrent->_value) {
        _pCurrent->x++;
        _pCurrent=_pCurrent->_prev;
    }
    _nodeItem->x=_pCurrent->x+1;
    _nodeItem->_next=_pCurrent->_next;
    _pCurrent->_next->_prev=_nodeItem;
    _nodeItem->_prev=_pCurrent;
    _pCurrent->_next=_nodeItem;
}

void TreeShow::setY()
{
    NodeItem * temp=nullptr;
    _rootForDraw->color=Black;
    _sentinelForDraw->y=-1;
    _NILForDraw->y=-1;
    _queueForDraw.enqueue(_rootForDraw);
    int level=0;
    int column=_queueForDraw.size();
    while (_queueForDraw.size()>0) {
        temp=_queueForDraw.dequeue();
        column--;
        temp->y=level;
        temp->xParent=temp->_parent->x;
        temp->yParent=temp->_parent->y;
        if(temp->_left!=_NILForDraw)
            _queueForDraw.enqueue(temp->_left);
        if(temp->_right!=_NILForDraw)
            _queueForDraw.enqueue(temp->_right);
        if(column==0){
            level++;
            column=_queueForDraw.size();
        }
    }
}

void TreeShow::deleteNodeItemFromLinkedList(TreeShow::NodeItem *_nodeItem)
{
    if(_nodeItem==_NILForDraw)
        return ;
    auto brokenPoint=_nodeItem->_prev;
    brokenPoint->_next=_nodeItem->_next;
    _nodeItem->_next->_prev=brokenPoint;
    brokenPoint=brokenPoint->_next;
    while(brokenPoint!=_sentinelForDraw){
        brokenPoint->x--;
        brokenPoint=brokenPoint->_next;
    }
}

void TreeShow::fillPropertyInInsert(TreeShow::NodeItem *_nodeItem)
{
    //insert value into linkedlist-ordered and update x coordinate
    insertNodeItemIntoLinkedList(_nodeItem);
    //set right infomation left without x by level-order
    setY();
}

void TreeShow::drawAllElement(QPainter &_painter,  NodeItem *_nodeItem)const
{
    if(_nodeItem!=_NILForDraw){
        drawAllElement(_painter,_nodeItem->_left);
        drawAllElement(_painter,_nodeItem->_right);
        //draw line
        if(_nodeItem->xParent>0)
            _painter.drawLine(_nodeItem->x*_diameter/2,_nodeItem->y*_nodeLineHeight,_nodeItem->xParent*_diameter/2,_nodeItem->yParent*_nodeLineHeight);
        //draw cycle
        QPainterPath myPath;
        myPath.addEllipse(QPoint(_nodeItem->x*_diameter/2,_nodeItem->y*_nodeLineHeight),_radius,_radius);
        _painter.drawPath(myPath);
        _painter.fillPath(myPath,_nodeItem->color==Red?Qt::red:Qt::black);
        //draw text
        _painter.setPen(Qt::white);
        _painter.drawText(QRect(_nodeItem->x*_diameter/2-_radius,_nodeItem->y*_nodeLineHeight-_radius,_diameter,_diameter),Qt::AlignCenter,QString::number(_nodeItem->_value));
        _painter.setPen(Qt::black);
    }
}

void TreeShow::drawAllElement() const
{
    pix->fill();
    QPainter pp(pix);
    pp.translate(0,_diameter/2);
    QFont font = pp.font();
    font.setPixelSize(_fontSize);
    pp.setFont(font);
    drawAllElement(pp,_rootForDraw);
}

void TreeShow::drawCurrentNodeItem(NodeItem *_nodeItem) const
{
    pix->fill();
    QPainter pp(pix);
    pp.translate(0,_diameter/2);
    QFont font = pp.font();
    font.setPixelSize(_fontSize);
    pp.setFont(font);
    drawAllElement(pp,_rootForDraw);

    QPainterPath myPath;
    myPath.addEllipse(QPoint(_nodeItem->x*_diameter/2,_nodeItem->y*_nodeLineHeight),_radius+5,_radius+5);
    pp.drawPath(myPath);
    pp.fillPath(myPath,_nodeItem->color==Red?Qt::red:Qt::black);
    pp.setPen(Qt::white);
    pp.drawText(QRect(_nodeItem->x*_diameter/2-_radius,_nodeItem->y*_nodeLineHeight-_radius,_diameter,_diameter),Qt::AlignCenter,QString::number(_nodeItem->_value));
    pp.setPen(Qt::black);
}

TreeShow::NodeItem *TreeShow::search(TreeShow::Action &action)
{
    _searchNodeItem=_hashForNodeItem.value(action.array[0]);
    return _searchNodeItem;
}

void TreeShow::add(TreeShow::Action &action)
{
    auto _parentNode=_hashForNodeItem.value(action.array[1]);
    auto _newNode=new NodeItem(action.array[0],_NILForDraw,_NILForDraw,_parentNode);
    _hashForNodeItem.insert(_newNode->_value,_newNode);
    if(_parentNode==_NILForDraw)
        _rootForDraw=_newNode;
    else if(_parentNode->_value>_newNode->_value)
        _parentNode->_left=_newNode;
    else
        _parentNode->_right=_newNode;

    fillPropertyInInsert(_newNode);
}

void TreeShow::rotate(TreeShow::Action &action)
{
    auto _currentNodeItem=_hashForNodeItem.value(action.array[0]);
    if(action.array[1]==0)
        rotationWithLeftChildForNodeItem(getParentReferenceForNodeItem(_currentNodeItem));
    else
        rotationWithRightChildForNodeItem(getParentReferenceForNodeItem(_currentNodeItem));
    setY();
}

TreeShow::SomeNodeItem TreeShow::changeColor(TreeShow::Action &action)
{
    int i=0;
    //qt5.13紧急修复：linux下gcc编译，是被初始化为nullptr的，win8.1下需要手动初始化为nullptr
    SomeNodeItem forReturn{{nullptr,nullptr,nullptr}};
    while(i<3){
        auto colorValue=action.array[2*i+1];
        if(colorValue>1)
            break;
        forReturn.a[i]=_hashForNodeItem.value(action.array[2*i]);
        forReturn.a[i]->color=colorValue==0?Red:Black;
        i++;
    }
    return forReturn;
}

template<typename T>
void TreeShow::recolorNodeItem(std::initializer_list<T> lists) const
{
    QPainter pp(pix);
    pp.translate(0,_diameter/2);
    QFont font = pp.font();
    font.setPixelSize(_fontSize);
    pp.setFont(font);
    for(auto x:lists)
        paintColor(x,pp);
}

void TreeShow:: paintColor(NodeItem * root, QPainter & pp, int dx) const
{
    if(root==nullptr)
        return;
    QPainterPath myPath;
    if(_searchNodeItem==root)
        dx=5;
    myPath.addEllipse(QPoint(root->x*_diameter/2,root->y*_nodeLineHeight),_radius+dx,_radius+dx);
    pp.drawPath(myPath);
    pp.fillPath(myPath,root->color==Red?Qt::red:Qt::black);
    pp.setPen(Qt::white);
    pp.drawText(QRect(root->x*_diameter/2-_radius,root->y*_nodeLineHeight-_radius,_diameter,_diameter),Qt::AlignCenter,QString::number(root->_value));
    pp.setPen(Qt::black);
}

void TreeShow::showNextValue()
{
    if(_qvectorForData.size()>_indexForQvector)
        emit nextValueReady(_qvectorForData[_indexForQvector++]);
}

void TreeShow::substitute(TreeShow::Action &action)
{
    auto y=_hashForNodeItem.value(action.array[0]);
    deleteNodeItemFromLinkedList(y);
    _hashForNodeItem.remove(y->_value);
    auto x=_hashForNodeItem.value(action.array[1]);
    if(action.array[0]!=action.array[2]){
        auto yOriginal=_hashForNodeItem.value(action.array[2]);
        _hashForNodeItem.remove(yOriginal->_value);
        yOriginal->_value=y->_value;
        _hashForNodeItem.insert(y->_value,yOriginal);
    }
    if(y==_rootForDraw)
        _rootForDraw=x;
    else if(y==y->_parent->_left)
        y->_parent->_left=x;
    else
        y->_parent->_right=x;
    x->_parent=y->_parent;
    setY();
    delete y;
}

void TreeShow::done(TreeShow::Action &action)
{
    emit insertFinish(action.array[0]==0?false:true);
}

void TreeShow::emptyRbtreeForDraw(NodeItem *&root)
{
    if(root!=_NILForDraw)
    {
        emptyRbtreeForDraw(root->_left);
        emptyRbtreeForDraw(root->_right);
        delete root;
        root=nullptr;
    }
}

void TreeShow::dispatchActionAndDraw(Action &action)
{
    switch (action._ope) {
    case Operator::Search:        
        drawCurrentNodeItem(search(action));
        break;
    case Operator::Add:
        add(action);
        drawAllElement();
        break;
    case Operator::Rotate:
        rotate(action);
        drawAllElement();
        break;
    case Operator::Substitute:
        substitute(action);
        drawAllElement();
        break;
    case Operator::ChangeColor:{
            auto arrayof3=changeColor(action);
            recolorNodeItem({arrayof3.a[0],arrayof3.a[1],arrayof3.a[2]});
        }
        break;
    case Operator::NextValue:
        showNextValue();
        break;
    case Operator::Done:
        done(action);
        drawAllElement();
        break;
    }
}

void TreeShow::insertAndRemoveOfNodeItem()
{
    auto _isDone=false;
    while (!_isDone) {
        auto action=_arrayForOrder[_step];
        switch (action._ope) {
            case Operator::Search:
//                search(action);
                break;
            case Operator::Add:
                add(action);
                break;
            case Operator::Rotate:
                rotate(action);
                break;
            case Operator::Substitute:
                substitute(action);
                break;
            case Operator::ChangeColor:
                changeColor(action);
                break;
            case Operator::NextValue:
                showNextValue();
                break;
            case Operator::Done:
                _isDone=true;
                done(action);
                break;
        }
        _step++;
    }
}

void TreeShow::resizeEvent(QResizeEvent *event)
{
    timer.start(200);
    _IsChangeWidgetSize=true;
    QLabel::resizeEvent(event);
}

// here is the definition of rbtree
void TreeShow::RBtreeNodeInitial()
{
    root=NIL=new Node<int>(-9998);
    NIL->color=Black;
    NIL->parent=NIL->left=NIL->right=NIL;

    _sentinelForDraw=new NodeItem(-9999);
    _sentinelForDraw->_prev=_sentinelForDraw->_next=_sentinelForDraw;

    _rootForDraw=_NILForDraw=new NodeItem(-9998);
    _NILForDraw->color=Black;
    _NILForDraw->_parent=_NILForDraw->_left=_NILForDraw->_right=_NILForDraw;

    _hashForNodeItem.insert(_NILForDraw->_value,_NILForDraw);

}

//之前单独实现红黑树时使用递归实现的插入,觉得不太好,重新用循环另外写了一个.它在这里无用
void TreeShow::insert(Node<int> *&root, Node<int> *parent, int x) {
    if (root == NIL) {
        root = new Node<int>(x, parent, NIL, NIL);
        auto tem=root;  //root is pointer Reference,we need a pointer value
        if (parent->color == Red)
            insertionFixUpOfDoubleRed(tem);
    } else if (x < root->item)
        insert(root->left, root, x);
    else if (x > root->item)
        insert(root->right, root, x);
    else
        ;	//重复
}

//有关插入操作指令序列的核心
void TreeShow::insert2(int x)
{
    _arrayForOrder.append({Operator::NextValue,{0}});
    auto temp=root;
    //红黑树在调整时,其哨兵节点NIL的父节点在旋转过程中会改变,因此,在树进行一轮清空后,
    //此时root=NIL,即root.parent不再是NIL.所以这里不可以写成root->parent
    auto tempParent=NIL;
    while (temp!=NIL) {
        tempParent=temp;
        _arrayForOrder.append({Operator::Search,{temp->item}});  //Search
        if(temp->item>x){
            temp=temp->left;
        }
        else if(temp->item<x){
            temp=temp->right;
        }
        else
            break;
    }
    if(temp!=NIL){
        _arrayForOrder.append({Operator::Done,{0}});
        return ;
    }
    auto newNode=new Node<int>(x,tempParent,NIL,NIL);
    _nodeSize++;
    if(tempParent==NIL){
        root=newNode;
        _arrayForOrder.append({Operator::Add,{x,tempParent->item}});
    }
    else {
        if(x<tempParent->item){
            tempParent->left=newNode;
        }
        else{
            tempParent->right=newNode;
        }
        _arrayForOrder.append({Operator::Add,{x,tempParent->item}});
    }
    insertionFixUpOfDoubleRed(newNode);
    _arrayForOrder.append({Operator::Done,{1}});
}

//这个函数和insert函数一样的,没用
void TreeShow::remove(Node<int> *&root, Node<int> *parent, int x) {
    if (root == NIL)
        return;
    else if (x < root->item)
        remove(root->left, root, x);
    else if (x > root->item)
        remove(root->right, root, x);
    else {
        if (root->left != NIL && root->right != NIL) {  //双节点
            int x_new = findmin(root->right);
            root->item = x_new;
            remove(root->right, root, x_new);
        } else {
            Node<int> *replaced = root;
            Node<int> *replacing = nullptr;
            Color origin_color = replaced->color;
            Node<int> *pold = replaced;
            replacing =
                    replaced->left != NIL ? replaced->left : replaced->right;
            replacing->parent = parent;
            root = replacing;
            delete pold;
            if (origin_color == Black)
                removeFixUpOfLostOfBlack(root);
        }
    }
}

void TreeShow::replace(Node<int> * y,Node<int> * x)
{
    if(y==root)
        root=x;
    else if(y==y->parent->left)
        y->parent->left=x;
    else
        y->parent->right=x;
    x->parent=y->parent;
}

//有关删除指令序列的核心
void TreeShow::remove2(int v)
{
    auto temp=root;
    while (temp!=NIL) {
        _arrayForOrder.append({Operator::Search,{temp->item}});  //Search
        if(temp->item>v){
            temp=temp->left;
        }
        else if(temp->item<v){
            temp=temp->right;
        }
        else
            break;
    }
    if(temp==NIL){
        _arrayForOrder.append({Operator::Done,{0}});
        return ;
    }
    auto y=temp;
    auto temp1=v;
    auto yOriginalColor=y->color;
    Node<int> * x=nullptr;
    if(temp->left==NIL){
        x=temp->right;
        replace(y,x);

    }else if(temp->right==NIL){
        x=temp->left;
        replace(y,y->left);
    }else{
        auto minNode=findMinValueNode(temp->right);
        //这里使用一种简单的方式,保留源节点,仅仅替换值的方式间接删除找到的节点
        y->item=minNode->item;
        y=minNode;
        x=y->right;
        yOriginalColor=y->color;
        if(y->parent==temp)
            x->parent=y;//算法导论中这几行没看明白,暂时把形式保存在这里好了.
        //喔,调试源码偶然发现了,这一步当x为哨兵节点是很重要,因为哨兵节点也参与之后调整过程
        //向上回溯时,需要赋值root=root.parent.哨兵节点父节点每次都会变化,所以这里必须强制更新
        //这里虽然采用删除方式不同,但还是保留了算法导论中的结构
        else {
            ;
        }
        replace(y,y->right);
    }
    _arrayForOrder.append({Operator::Substitute,{y->item,x->item,temp1}});
    delete y;
    y=nullptr;
    if(yOriginalColor==Black)
        removeFixUpOfLostOfBlack(x);
    _arrayForOrder.append({Operator::Done,{1}});
}

void TreeShow::rotationWithLeftChild(Node<int> *&root) {
    Node<int> *left_child = root->left;

    root->left = left_child->right;
    left_child->right->parent = root;

    left_child->right = root;
    left_child->parent = root->parent;

    root->parent = left_child;
    root = left_child;
}

void TreeShow::rotationWithLeftChildForNodeItem(TreeShow::NodeItem *&root)
{
    NodeItem *left_child = root->_left;

    root->_left = left_child->_right;
    left_child->_right->_parent = root;

    left_child->_right = root;
    left_child->_parent = root->_parent;

    root->_parent = left_child;
    root = left_child;
}

void TreeShow::rotationWithRightChild(Node<int> *&root) {
    Node<int> *right_child = root->right;

    root->right = right_child->left;
    right_child->left->parent = root;

    right_child->left = root;
    right_child->parent = root->parent;

    root->parent = right_child;
    root = right_child;
}

void TreeShow::rotationWithRightChildForNodeItem(TreeShow::NodeItem *&root)
{
    auto right_child = root->_right;

    root->_right = right_child->_left;
    right_child->_left->_parent = root;

    right_child->_left = root;
    right_child->_parent = root->_parent;

    root->_parent = right_child;
    root = right_child;
}

//把每步插入修复过程转化成一个操作序列的核心步骤
void TreeShow::insertionFixUpOfDoubleRed(Node<int> *root) {
    while (root->parent->color == Red) {
        if (root->parent == root->parent->parent->left) {
            if (root->parent->parent->right->color == Red) {  //case 1
                root->parent->color = Black;
                root->parent->parent->right->color = Black;
                root->parent->parent->color = Red;
                _arrayForOrder.append({Operator::ChangeColor,{root->parent->item,1,
                                                              root->parent->parent->right->item,1,
                                                              root->parent->parent->item,0}});  //record
                root = root->parent->parent;
                _arrayForOrder.append({Operator::Search,{root->item}}); //record
            } else {
                Node<int> *&gp = getParentReference(root->parent->parent);
                if (root == root->parent->right) {   //case 2
                    root = root->parent;
                    _arrayForOrder.append({Operator::Rotate,{getParentReference(root)->item,1}});
                    rotationWithRightChild(getParentReference(root));
                }
                _arrayForOrder.append({Operator::Rotate,{gp->item,0}});
                rotationWithLeftChild(gp);    //case 3
                gp->color = Black;
                gp->right->color = Red;
                _arrayForOrder.append({Operator::ChangeColor,{gp->item,1,gp->right->item,0,-1,2}});
                break;
            }
        } else {
            if (root->parent->parent->left->color == Red) {
                root->parent->color = Black;
                root->parent->parent->left->color = Black;
                root->parent->parent->color = Red;
                _arrayForOrder.append({Operator::ChangeColor,{root->parent->item,1,
                                                              root->parent->parent->left->item,1,
                                                              root->parent->parent->item,0}});
                root = root->parent->parent;
                _arrayForOrder.append({Search,{root->item}});
            } else {
                Node<int> *&gp = getParentReference(root->parent->parent);
                if (root == root->parent->left) {
                    root = root->parent;
                    _arrayForOrder.append({Rotate,{getParentReference(root)->item,0}});
                    rotationWithLeftChild(getParentReference(root));
                }
                _arrayForOrder.append({Rotate,{gp->item,1}});
                rotationWithRightChild(gp);
                gp->color = Black;
                gp->left->color = Red;
                _arrayForOrder.append({ChangeColor,{gp->item,1,gp->left->item,0,-1,2}});
                break;
            }
        }
    }
    this->root->color = Black;
    _arrayForOrder.append({Operator::ChangeColor,{this->root->item,1,-1,2,-1,2}});
}

//同样是删除修复的核心步骤
void TreeShow::removeFixUpOfLostOfBlack(Node<int> *root) {
    while (root->color == Black && root != this->root) {
        Node<int> *root_brother = nullptr;
        if (root == root->parent->left) {
            root_brother = root->parent->right;
            if (root_brother->color == Red) {	//case 1
                Node<int> *&gp = getParentReference(root->parent);
                _arrayForOrder.append({Operator::Rotate,{gp->item,1}});
                rotationWithRightChild(gp);
                gp->color = Black;
                gp->left->color = Red;
                root_brother = root->parent->right;
                _arrayForOrder.append({Operator::ChangeColor,{gp->item,1,gp->left->item,0,-1,2}});
            }
            if (root_brother->left->color == Black
                    && root_brother->right->color == Black) {	//case 2
                root_brother->color = Red;
                _arrayForOrder.append({Operator::ChangeColor,{root_brother->item,0,-1,2,-1,2}});
                root = root->parent;
                _arrayForOrder.append({Operator::Search,{root->item}});
            } else {
                Node<int> * &root_parent = getParentReference(root->parent);
                if (root_brother->right->color == Black) {	//case 3
                    _arrayForOrder.append({Operator::Rotate,{root_brother->item,0}});
                    rotationWithLeftChild(root_parent->right);
                    root_parent->right->color = Black;
                    root_parent->right->right->color = Red;
                    root_brother=root_parent->right;
                    _arrayForOrder.append({Operator::ChangeColor,{root_parent->right->item,1,root_parent->right->right->item,0,-1,2}});
                }
                _arrayForOrder.append({Operator::Rotate,{root_parent->item,1}});
                rotationWithRightChild(root_parent);	//case 4
                root_parent->color = root_parent->left->color;
                root_parent->left->color = Black;
                root_parent->right->color = Black;
                //利用枚举默认值也是和我们的规定值相等,否则下面要判断
                _arrayForOrder.append({Operator::ChangeColor,{root_parent->item,root_parent->color,root_parent->left->item,1,root_parent->right->item,1}});
                root = this->root;
                _arrayForOrder.append({Operator::Search,{root->item}});
            }
        } else {
            root_brother = root->parent->left;
            if (root_brother->color == Red) {	//case 1
                Node<int> *&gp = getParentReference(root->parent);
                _arrayForOrder.append({Operator::Rotate,{gp->item,0}});
                rotationWithLeftChild(gp);
                gp->color = Black;
                gp->right->color = Red;
                root_brother = root->parent->left;
                _arrayForOrder.append({Operator::ChangeColor,{gp->item,1,gp->right->item,0,-1,2}});
            }
            if (root_brother->left->color == Black
                    && root_brother->right->color == Black) {	//case 2
                root_brother->color = Red;
                _arrayForOrder.append({Operator::ChangeColor,{root_brother->item,0,-1,2,-1,2}});
                root = root->parent;
                _arrayForOrder.append({Operator::Search,{root->item}});
            } else {
                Node<int> * &root_parent = getParentReference(root->parent);
                if (root_brother->left->color == Black) {	//case 3
                    _arrayForOrder.append({Operator::Rotate,{getParentReference(root_brother)->item,1}});
                    rotationWithRightChild(root_parent->left);
                    root_parent->left->color = Black;
                    root_parent->left->left->color = Red;
                    _arrayForOrder.append({Operator::ChangeColor,{root_parent->left->item,1,root_parent->left->left->item,0,-1,2}});
                }
                _arrayForOrder.append({Operator::Rotate,{root_parent->item,0}});
                rotationWithLeftChild(root_parent);	//case 4
                root_parent->color = root_parent->right->color;
                root_parent->left->color = Black;
                root_parent->right->color = Black;
                _arrayForOrder.append({Operator::ChangeColor,{root_parent->item,root_parent->color,root_parent->left->item,1,root_parent->right->item,1}});
                root = this->root;
                _arrayForOrder.append({Operator::Search,{root->item}});
            }
        }
    }
    root->color = Black;
    _arrayForOrder.append({Operator::ChangeColor,{root->item,1,-1,2,-1,2}});
}

void TreeShow::emptyRBtree(Node<int> *&root)
{
    if(root!=NIL)
    {
        emptyRBtree(root->left);
        emptyRBtree(root->right);
        delete root;
        root=nullptr;
    }
}

int TreeShow::findmin(const Node<int> *root) const {
    const Node<int> *temp = root;
    while (temp->left != NIL) {
        temp = temp->left;
    }
    return temp->item;
}

TreeShow::Node<int> *TreeShow::findMinValueNode(const Node<int> *root)
{
    auto temp=root;
    while (temp->left!=NIL) {
        _arrayForOrder.append({Operator::Search,{temp->item}});
        temp=temp->left;
    }
    return const_cast<Node<int> *>(temp);
}
