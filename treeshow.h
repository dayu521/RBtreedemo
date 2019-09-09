#ifndef TREESHOW_H
#define TREESHOW_H

#include <QLabel>
#include <QPixmap>
#include <QQueue>
#include <QTimer>
#include <initializer_list>

class TreeShow : public QLabel {
    Q_OBJECT
public:
    explicit TreeShow(QWidget *parent = nullptr);
    ~TreeShow() override;

public:
    void taskOfInsert(int i);
    void taskOfRemove(int i);
    void takeOnePicture();
    void setIsClear(bool isClear) { _isClearForDraw = isClear; }
    void setTreeNodeDiameter(int diameter);
    int  getTreeNodeDiameter()const;
    int  getAllTreeNodeCount()const;
    int  getCurrentStep()const{ return _step;}

signals:
    void setpix();
    void nextValueReady(int nextValue);
    void noMorePicture();
    void insertFinish(bool isSuccess);

public slots:
    void drawPicture(int x, int operation);
    void drawPicture();
    void clearPictureForDraw();

    // QWidget interface
protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
private:
    void initial();
    void prepareBeforeDraw();
    template <typename T>
    class Node;
    struct NodeItem;
    void insertNodeItemIntoLinkedList(TreeShow::NodeItem *_nodeItem);
    void setY();
    void deleteNodeItemFromLinkedList(NodeItem * _nodeItem);
    void fillPropertyInInsert(NodeItem *_nodeItem);

    // operation for NodeItem-node
    struct Action;
    struct SomeNodeItem;
    NodeItem * search(Action &action);
    void add(Action &action);
    void rotate(Action &action);
    SomeNodeItem  changeColor(Action &action);
    void showNextValue();
    void substitute(Action &action);
    void done(Action &action);

    //  drawing for NodeItem-node
    void drawAllElement(QPainter &_painter, NodeItem *_nodeItem)const;
    void drawAllElement()const;
    void drawCurrentNodeItem(NodeItem * _nodeItem)const;
    void paintColor(NodeItem *root,QPainter & pp,int dx=0)const;
    template<typename T>
    void recolorNodeItem(std::initializer_list<T> lists)const;

    void dispatchActionAndDraw(Action &action);
    void insertAndRemoveOfNodeItem();

private:
    QPixmap *pix;

    int _pixWidth = 2000;
    int _pixHeight = 1000;
    int _diameter = _pixWidth/50;   //节点直径
    int _radius = _diameter / 2;    //半径
    int _nodeLineHeight = 3 * _diameter / 2;     //垂直距离
    int _fontSize=_radius;      //字体大小

    QTimer timer;
    bool _IsChangeWidgetSize = false;       //防抖操作,减少重置大小事件造成的多次绘图

    QVector<int> _qvectorForData;       //插入节点值保留的副本
    int _indexForQvector=0;     //以上副本的索引

    Node<int> *root;        //红黑树根节点
    NodeItem *_rootForDraw;     //绘图红黑树的根节点
    Node<int> *NIL;     //原始红黑树的哨兵节点
    NodeItem *_NILForDraw;     //为绘图而生成的红黑树的哨兵节点
    NodeItem *_sentinelForDraw;     //有绘图节点形成的序链接链表的哨兵节点

    QQueue<NodeItem *> _queueForDraw;       //用来进行层序遍历,然后设置每个绘图节点的y轴坐标
    QVector<Action> _arrayForOrder;     //保存的一系列操作
    QHash<int, NodeItem *> _hashForNodeItem;    //持有绘图树节点
    int _step = 0;      //当前进行到哪个操作了
    bool _isClearForDraw = false;       //是否已经清除

    //树节点数量
    int _nodeSize=0;

    NodeItem * _searchNodeItem=nullptr;

    // rbtree
private:
    enum Color { Red, Black };

    template <typename T>
    class Node {
    public:
        // for rbtree
        T item;
        Node<T> *parent;
        Node<T> *left;
        Node<T> *right;
        Color color;
        // for draw
        Node<T> *_next = nullptr;
        Node<T> *_prev = nullptr;
        int x = 0, y = -1;               // current coordinate
        int xParent = -1, yParent = -1;  // parent link coordinate

        Node(const T &_item, Node<T> *_parent = nullptr, Node<T> *_left = nullptr,
             Node<T> *_right = nullptr, Color _Color = Red)
            : item(_item),
              parent(_parent),
              left(_left),
              right(_right),
              color(_Color) {}
    };
    void RBtreeNodeInitial();
    void insert(Node<int> *&root, Node<int> *parent, int x);
    void insert2(int x);

    void remove(Node<int> *&root, Node<int> *parent, int x);
    void remove2(int x);

    void rotationWithLeftChild(Node<int> *&root);
    void rotationWithLeftChildForNodeItem(NodeItem *&root);

    void rotationWithRightChild(Node<int> *&root);
    void rotationWithRightChildForNodeItem(NodeItem *&root);

    void replace(Node<int> *y, Node<int> *x);
    void replaceForNodeItem(NodeItem *y, NodeItem *x);

    void insertionFixUpOfDoubleRed(Node<int> *root);
    void removeFixUpOfLostOfBlack(Node<int> *root);

    void emptyRBtree(Node<int> *&root);
    void emptyRbtreeForDraw(NodeItem *&root);

    Node<int> *&getParentReference(Node<int> *child) {
        if (child->parent == NIL) return this->root;
        return child == child->parent->left ? child->parent->left
                                            : child->parent->right;
    }
    NodeItem *&getParentReferenceForNodeItem(NodeItem *child) {
        if (child->_parent == _NILForDraw) return this->_rootForDraw;
        return child == child->_parent->_left ? child->_parent->_left
                                              : child->_parent->_right;
    }
    int findmin(const Node<int> *root) const;
    Node<int> * findMinValueNode(const Node<int> * root);

    enum Operator {
        Search,
        Add,
        Rotate,
        ChangeColor,
        NextValue,
        Substitute,
        Done
    };

    struct Action {
        Operator _ope;
    /* 下面是规则的说明
     * 1.增加节点( Add)
     *      索引0:新节点值
     *      索引1:父节点值
     * 2.旋转(Rotate)
     *      索引0:父节点值
     *      索引1:值为0与左孩子旋转,值为1与右孩子旋转
     * 3.变色(ChangeColor)
     *      0是红色,值1是黑色.索引5的值大于1表示不存在当前节点
     *      索引0:当前节点值,索引1:当前节点值颜色
     *      索引2:当前节点值,索引3:当前节点值颜色
     *      索引4:当前节点值,索引5:当前节点值颜色
     *      三个键值对组成
     * 4.查找过程(Search)
     *      索引0:当前节点值
     * 5.下一个值(NextValue)
     *      索引0值固定为0
     * 6.插入或删除完成(Done)
     *      索引0值:当成功时为1,失败时0
     * 7.替换某个节点(Substitute),并且删除那个节点
     *      索引0:被替换的节点,y
     *      索引1:将要用来替换的新节点,x
     *      索引2:被替换节点的原始节点,开始赋值的y
     *  注意:这种方式的作用不好说清,是不是换一种麻烦的代码但可读性较高的算法呢?
     */
        int array[6];
    };

    struct NodeItem {
        int _value;

        NodeItem *_left;
        NodeItem *_right;
        NodeItem *_parent;

        NodeItem *_next = nullptr;
        NodeItem *_prev = nullptr;

        int x = 0, y = -1;               // current coordinate
        int xParent = -1, yParent = -1;  // parent link coordinate
        Color color = Red;

        NodeItem(int value, NodeItem *left = nullptr, NodeItem *right = nullptr,
                 NodeItem *parent = nullptr)
            : _value(value), _left(left), _right(right), _parent(parent) {}
    };
    struct SomeNodeItem{
        NodeItem * a[3];
//        operator std::initializer_list<NodeItem *>()
//        {
//            return {a[0],a[1],a[2]};
//        }
    };
};

#endif  // TREESHOW_H
