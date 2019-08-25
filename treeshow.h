#ifndef TREESHOW_H
#define TREESHOW_H

#include <QLabel>
#include<QPixmap>
#include<QTimer>
#include<QQueue>


class TreeShow : public QLabel
{
    Q_OBJECT
public:
    explicit TreeShow(QWidget *parent = nullptr);
    ~TreeShow() override;
    void initial();
    void start(){}

public:
    enum Operator{Search,AddOrRemove,Rotate,ChangeColor,All};

signals:
    void setpix();

public slots:
    void drawPicture(int x,int operation);
    void clearPicture();

    // QWidget interface
protected:
    virtual void paintEvent(QPaintEvent *event) override;

private:   
    template<typename T>
    class Node;
    void emptyView();
    void prepareBeforeDraw();
    void fillPropertyInInsert(Node<int> *_nodeItem);
    void drawElement(QPainter & _painter,const Node<int> *_nodeItem);

    //for view-node
    void search();
    void addOrRemove();
    void rotate();
    void changeColor();
    struct Order;
    void dispatchAction(Order & action);

private:

    QPixmap * pix;
    int _pixWidth=1366;
    int _pixHeight=768;
    int _CycleRadiusMax=20;
    int _diameter=30;
    int _radius=_diameter/2;
    int _nodeLineWidth=3*_diameter/2;

    QPixmap * pold;
    QTimer timer;
    bool _IsChangeWidgetSize=false;

    Node<int> *root;
    Node<int> *_rootForDraw;
    Node<int> *NIL;
    Node<int> *_sentinel;
    QQueue<Node<int> *>   _queueForNode;
    QVector<Order> _arrayForOrder;
    // QWidget interface
protected:
    virtual void resizeEvent(QResizeEvent *event) override;

    //rbtree
private:
    enum Color{Red,Black};

    template<typename T>
    class Node
    {
    public:
        //for rbtree
        T item;
        Node<T> * parent;
        Node<T> * left;
        Node<T> * right;
        Color color;
        //for draw
        Node<T> * _next=nullptr;
        Node<T> * _prev=nullptr;
        int x=0,y=-1;    //current coordinate
        int xParent=-1,yParent=-1;    //parent link coordinate

        Node(const T & _item, Node<T> * _parent=nullptr,Node<T> * _left=nullptr, Node<T> * _right=nullptr,Color _Color=Red)
            :item(_item),parent(_parent),left(_left),right(_right), color(_Color){}
    };
    void RBtreeNodeInitial();
    void insert(Node<int> *&root, Node<int> *parent, int x);
    void insert(Node<int> *&root,Node<int> *&rootForDraw, Node<int> *parent, int x);
    void remove(Node<int> *&root, Node<int> *parent, int x);
    void rotationWithLeftChild(Node<int> *&root);
    void rotationWithRightChild(Node<int> *&root);
    void insertionFixUpOfDoubleRed(Node<int> *root);
    void removeFixUpOfLostOfBlack(Node<int> *root);
    void emptyRBtree(Node<int> * & root);
    Node<int>*& getParentReference(Node<int> *child) {
        if (child->parent == NIL)
            return this->root;
        return child == child->parent->left ?
                child->parent->left : child->parent->right;
    }
    int findmin(const Node<int> *root) const;  

    struct Order{
        Operator command;
        Node<int> * data;
    };
};



#endif // TREESHOW_H
