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
    delete _sentinel;
    delete NIL;
    delete pix;
    emptyRBtree(root);
    emptyRbtreeForDraw(_rootForDraw);
}

void TreeShow::initial()
{
    RBtreeNodeInitial();
}

void TreeShow::takePicture()
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

void TreeShow::setPixMapTo(int diameter, int height)
{
    auto pold=pix;
//    int factor=this->width()/this->height();
    pix=new QPixmap(this->width(),this->height());
    pix->fill();
    delete  pold;
    update();
}

void TreeShow::setTreeNodeDiameter(int diameter)
{
    _diameter=diameter;
    _radius=_diameter/2;
    _fontSize=_radius;
    _nodeLineWidth = 4 * _radius;
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
    insertOfNodeItem();
    drawAllElement();
    update();
}

void TreeShow::drawPicture()
{
    if(_step>=_arrayForOrder.size()){
        emit noMorePicture();
        return  ;
    }
    //    pix->fill();
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

    _xTranslationOffset=0;
    _yTranslationOffset=0;
    _nodeSize=0;
    pix->fill();
    _isClearForDraw=true;
    update();
}

void TreeShow::clearPicture()
{
    _sentinel->_prev=_sentinel->_next=_sentinel;
    emptyRBtree(root);
    root=NIL;
    pix->fill();
    update();
}

void TreeShow::paintEvent(QPaintEvent *event)
{
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

/* void TreeShow::fillPropertyInInsert(Node<int> *_nodeItem)
{
    //insert value into linkedlist-ordered and update x coordinate
    Node<int> * _pCurrent=_sentinel->_prev;
    while (_pCurrent!=_sentinel&&_nodeItem->item<_pCurrent->item) {
        _pCurrent->x++;
        _pCurrent=_pCurrent->_prev;
    }
    _nodeItem->x=_pCurrent->x+1;
    _nodeItem->_next=_pCurrent->_next;
    _pCurrent->_next->_prev=_nodeItem;
    _nodeItem->_prev=_pCurrent;
    _pCurrent->_next=_nodeItem;
    //set correct infomation without x by levelorder
    Node<int> * temp=nullptr;
    root->color=Black;
    _sentinel->y=-1;
    NIL->y=-1;
    _queueForNode.enqueue(root);
    int level=0;
    int column=_queueForNode.size();
    while (_queueForNode.size()>0) {
        temp=_queueForNode.dequeue();
        column--;
        temp->y=level;
        temp->xParent=temp->parent->x;
        temp->yParent=temp->parent->y;
        if(temp->left!=NIL)
            _queueForNode.enqueue(temp->left);
        if(temp->right!=NIL)
            _queueForNode.enqueue(temp->right);
        if(column==0){
            level++;
            column=_queueForNode.size();
        }
    }
}
*/
void TreeShow::setX(TreeShow::NodeItem *_nodeItem)
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

void TreeShow::fillPropertyInInsert(TreeShow::NodeItem *_nodeItem)
{
    //insert value into linkedlist-ordered and update x coordinate
    setX(_nodeItem);
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
            _painter.drawLine(_nodeItem->x*_diameter/2,_nodeItem->y*_nodeLineWidth,_nodeItem->xParent*_diameter/2,_nodeItem->yParent*_nodeLineWidth);
        //draw cycle
        QPainterPath myPath;
        myPath.addEllipse(QPoint(_nodeItem->x*_diameter/2,_nodeItem->y*_nodeLineWidth),_radius,_radius);
        _painter.drawPath(myPath);
        _painter.fillPath(myPath,_nodeItem->color==Red?Qt::red:Qt::black);
        //draw text
        _painter.setPen(Qt::white);
        _painter.drawText(QRect(_nodeItem->x*_diameter/2-_radius,_nodeItem->y*_nodeLineWidth-_radius,_diameter,_diameter),Qt::AlignCenter,QString::number(_nodeItem->_value));
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
    myPath.addEllipse(QPoint(_nodeItem->x*_diameter/2,_nodeItem->y*_nodeLineWidth),_radius+5,_radius+5);
    pp.drawPath(myPath);
    pp.fillPath(myPath,_nodeItem->color==Red?Qt::red:Qt::black);
    pp.setPen(Qt::white);
    pp.drawText(QRect(_nodeItem->x*_diameter/2-_radius,_nodeItem->y*_nodeLineWidth-_radius,_diameter,_diameter),Qt::AlignCenter,QString::number(_nodeItem->_value));
    pp.setPen(Qt::black);
}

TreeShow::NodeItem *TreeShow::search(TreeShow::Action &action) const
{
    auto _nodeItem=_hashForNodeItem.value(action.array[0]);
    return _nodeItem;
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
        rotationWithLeftChildForDraw(getParentReferenceForDraw(_currentNodeItem));
    else
        rotationWithRightChildForDraw(getParentReferenceForDraw(_currentNodeItem));
    setY();
}

TreeShow::SomeNodeItem TreeShow::changeColor(TreeShow::Action &action)
{
    auto node1=_hashForNodeItem.value(action.array[0]);
    node1->color=action.array[1]==0?Red:Black;
    auto node2=_hashForNodeItem.value(action.array[2]);
    node2->color=action.array[3]==0?Red:Black;
    NodeItem * node3=nullptr;
    if(!(action.array[5]>1)){
        node3=_hashForNodeItem.value(action.array[4]);
        node3->color=action.array[5]==0?Red:Black;
    }
    return {node1,node2,node3};//必须返回不大于3个元素的数组
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
    myPath.addEllipse(QPoint(root->x*_diameter/2,root->y*_nodeLineWidth),_radius+dx,_radius+dx);
    pp.drawPath(myPath);
    pp.fillPath(myPath,root->color==Red?Qt::red:Qt::black);
    pp.setPen(Qt::white);
    pp.drawText(QRect(root->x*_diameter/2-_radius,root->y*_nodeLineWidth-_radius,_diameter,_diameter),Qt::AlignCenter,QString::number(root->_value));
    pp.setPen(Qt::black);
}

void TreeShow::paintRootBlack()
{
    this->_rootForDraw->color=Black;
}

void TreeShow::showNextValue()
{
    if(_qvectorForData.size()>_indexForQvector)
        emit nextValueReady(_qvectorForData[_indexForQvector++]);
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
    case Operator::ChangeColor:{
            auto arrayof3=changeColor(action);
            recolorNodeItem({arrayof3.a[0],arrayof3.a[1],arrayof3.a[2]});
        }
        break;
    case Operator::PaintBlack:
        paintRootBlack();
        recolorNodeItem({this->_rootForDraw});
        break;
    case Operator::NextValue:
        showNextValue();
        break;
    case Operator::Done:
        done(action);
        break;
    }
}

void TreeShow::insertOfNodeItem()
{
    auto _isDone=false;
    while (!_isDone) {
        auto action=_arrayForOrder[_step];
        switch (action._ope) {
            case Operator::Search:
                search(action);
                break;
            case Operator::Add:
                add(action);
                break;
            case Operator::Rotate:
                rotate(action);
                break;
            case Operator::ChangeColor:
                changeColor(action);
                break;
            case Operator::PaintBlack:
                paintRootBlack();
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
    _sentinel=new Node<int>(-9999);
    _sentinel->_prev=_sentinel->_next=_sentinel;

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

void TreeShow::rotationWithLeftChild(Node<int> *&root) {
    Node<int> *left_child = root->left;

    root->left = left_child->right;
    left_child->right->parent = root;

    left_child->right = root;
    left_child->parent = root->parent;

    root->parent = left_child;
    root = left_child;
}

void TreeShow::rotationWithLeftChildForDraw(TreeShow::NodeItem *&root)
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

void TreeShow::rotationWithRightChildForDraw(TreeShow::NodeItem *&root)
{
    auto right_child = root->_right;

    root->_right = right_child->_left;
    right_child->_left->_parent = root;

    right_child->_left = root;
    right_child->_parent = root->_parent;

    root->_parent = right_child;
    root = right_child;
}

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
    _arrayForOrder.append({PaintBlack,{0}});
}


void TreeShow::removeFixUpOfLostOfBlack(Node<int> *root) {
    while (root->color == Black && root != this->root) {
        Node<int> *root_brother = nullptr;
        if (root == root->parent->left) {
            root_brother = root->parent->right;
            if (root_brother->color == Red) {	//case 1
                Node<int> *&gp = getParentReference(root->parent);
                rotationWithRightChild(gp);
                gp->color = Black;
                gp->left->color = Red;
                root_brother = root->parent->right;
            }
            if (root_brother->left->color == Black
                    && root_brother->right->color == Black) {	//case 2
                root->color = Red;
                root_brother->color = Red;
                root = root->parent;
            } else {
                Node<int> *&root_parent = root->parent;
                if (root_brother->left->color == Red) {	//case 3
                    rotationWithLeftChild(getParentReference(root_brother));
                    root_parent->right->color = Black;
                    root_parent->right->right->color = Red;
                }
                rotationWithRightChild(root_parent);	//case 4
                root_parent->color = root_parent->left->color;
                root_parent->left->color = Black;
                root_parent->right->color = Black;
                root = this->root;
            }
        } else {
            root_brother = root->parent->left;
            if (root_brother->color == Red) {	//case 1
                Node<int> *&gp = getParentReference(root->parent);
                rotationWithLeftChild(gp);
                gp->color = Black;
                gp->right->color = Red;
                root_brother = root->parent->left;
            }
            if (root_brother->left->color == Black
                    && root_brother->right->color == Black) {	//case 2
                root->color = Red;
                root_brother->color = Red;
                root = root->parent;
            } else {
                Node<int> *&root_parent = root->parent;
                if (root_brother->right->color == Red) {	//case 3
                    rotationWithRightChild(getParentReference(root_brother));
                    root_parent->left->color = Black;
                    root_parent->left->left->color = Red;
                }
                rotationWithLeftChild(root_parent);	//case 4
                root_parent->color = root_parent->right->color;
                root_parent->left->color = Black;
                root_parent->right->color = Black;
                root = this->root;
            }
        }
    }
    root->color = Black;
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




