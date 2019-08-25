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
}

void TreeShow::initial()
{

    RBtreeNodeInitial();
}

void TreeShow::drawPicture(int x, int operation)
{
    if(operation==1)
        return ;
    insert(root,NIL,x);
    QPainter pp(pix);
    pp.translate(0,_diameter/2);
    pix->fill();
    for(Node<int> * ppp=_sentinel->_next;ppp!=_sentinel;ppp=ppp->_next){
        drawElement(pp,ppp);
    }
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
    p.drawPixmap(0,0,pix->scaled(width(),height(),Qt::KeepAspectRatio));

}

void TreeShow::prepareBeforeDraw()
{
    pix=new QPixmap(_pixWidth,_pixHeight);
    pix->fill();
}

void TreeShow::fillPropertyInInsert(Node<int> *_nodeItem)
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
    //set right infomation without x by levelorder
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

void TreeShow::drawElement(QPainter &_painter, const TreeShow::Node<int> *_nodeItem)
{
    //draw line
    if(_nodeItem->xParent>0)
        _painter.drawLine(_nodeItem->x*_diameter/2,_nodeItem->y*_nodeLineWidth,_nodeItem->xParent*_diameter/2,_nodeItem->yParent*_nodeLineWidth);
    //draw cycle
    QPainterPath myPath;
    myPath.addEllipse(QPoint(_nodeItem->x*_diameter/2,_nodeItem->y*_nodeLineWidth),_radius,_radius);
    _painter.drawPath(myPath);
    _painter.fillPath(myPath,_nodeItem->color==0?Qt::red:Qt::black);
    // draw text
    _painter.setPen(Qt::white);
    _painter.drawText(QRect(_nodeItem->x*_diameter/2-_radius,_nodeItem->y*_nodeLineWidth-_radius,_diameter,_diameter),Qt::AlignCenter,QString::number(_nodeItem->item));
    _painter.setPen(Qt::black);
}

void TreeShow::dispatchAction(TreeShow::Order &action)
{
    switch (action.command) {
    case Operator::Search:
        search();
        break;
    case Operator::AddOrRemove:
        addOrRemove();
        break;
    case Operator::Rotate:
        rotate();
        break;
    case Operator::ChangeColor:
        changeColor();
        break;
    case Operator::All:
        ;
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

    _rootForDraw=new Node<int>(-9997);
    _rootForDraw->left=_rootForDraw->right=_rootForDraw->parent=nullptr;
}

void TreeShow::insert(Node<int> *&root, Node<int> *parent, int x) {
    if (root == NIL) {
        root = new Node<int>(x, parent, NIL, NIL);
        auto tem=root;  //root is pointer Reference,we need a pointer value
        if (parent->color == Red)
            insertionFixUpOfDoubleRed(root);
        fillPropertyInInsert(tem);
    } else if (x < root->item)
        insert(root->left, root, x);
    else if (x > root->item)
        insert(root->right, root, x);
    else
        ;	//重复
}

void TreeShow::insert(Node<int> *&root, Node<int> *&rootForDraw, Node<int> *parent, int x)
{

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

void TreeShow::rotationWithRightChild(Node<int> *&root) {
    Node<int> *right_child = root->right;

    root->right = right_child->left;
    right_child->left->parent = root;

    right_child->left = root;
    right_child->parent = root->parent;

    root->parent = right_child;
    root = right_child;
}

void TreeShow::insertionFixUpOfDoubleRed(Node<int> *root) {
    while (root->parent->color == Red) {
        if (root->parent == root->parent->parent->left) {
            if (root->parent->parent->right->color == Red) {  //case 1
                root->parent->color = Black;
                root->parent->parent->right->color = Black;
                root->parent->parent->color = Red;
                root = root->parent->parent;
            } else {
                Node<int> *&gp = getParentReference(root->parent->parent);
                if (root == root->parent->right) {   //case 2
                    root = root->parent;
                    rotationWithRightChild(getParentReference(root));
                }
                rotationWithLeftChild(gp);    //case 3
                gp->color = Black;
                gp->right->color = Red;
                break;
            }
        } else {
            if (root->parent->parent->left->color == Red) {
                root->parent->color = Black;
                root->parent->parent->left->color = Black;
                root->parent->parent->color = Red;
                root = root->parent->parent;
            } else {
                Node<int> *&gp = getParentReference(root->parent->parent);
                if (root == root->parent->left) {
                    root = root->parent;
                    rotationWithLeftChild(getParentReference(root));
                }
                rotationWithRightChild(gp);
                gp->color = Black;
                gp->left->color = Red;
                break;
            }
        }
    }
    this->root->color = Black;
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

