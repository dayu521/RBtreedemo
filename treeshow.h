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
    void initial();
    void start() {}

public:
    enum Operator {
        Search,
        Add,
        Remove,
        Rotate,
        ChangeColor,
        All,
        PaintBlack,
        NextValue,
        Done
    };
    void taskOfInsert(int i);
    void takePicture();
    void setIsClear(bool isClear) { _isClearForDraw = isClear; }
    void setPixMapTo(int diameter,int height);
    void setTreeNodeDiameter(int diameter);
    int  getTreeNodeDiameter()const;
    int  getAllTreeNodeCount()const;

    void prepareQPainter();



signals:
    void setpix();
    void nextValueReady(int nextValue);
    void noMorePicture();
    void insertFinish(bool isSuccess);

public slots:
    void drawPicture(int x, int operation);
    void clearPicture();
    void drawPicture();
    void clearPictureForDraw();

    // QWidget interface
protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
private:
    template <typename T>
    class Node;
    struct NodeItem;
    void emptyView();
    void prepareBeforeDraw();
//    void fillPropertyInInsert(Node<int> *_nodeItem);
    void setX(TreeShow::NodeItem *_nodeItem);
    void setY();
    void fillPropertyInInsert(NodeItem *_nodeItem);


    // operation for NodeItem-node
    struct Action;
    NodeItem * search(Action &action)const;
    void add(Action &action);
    void rotate(Action &action);
    struct SomeNodeItem;
    SomeNodeItem  changeColor(Action &action);
    void paintRootBlack();
    void showNextValue();
    void done(Action &action);
    //  drawing for NodeItem-node
    void drawAllElement(QPainter &_painter, NodeItem *_nodeItem)const;
    void drawAllElement()const;
    void drawCurrentNodeItem(NodeItem * _nodeItem)const;
    void paintColor(NodeItem *root,QPainter & pp,int dx=0)const;
    template<typename T>
    void recolorNodeItem(std::initializer_list<T> lists)const;

    void emptyRbtreeForDraw(NodeItem *&root);

    void dispatchActionAndDraw(Action &action);
    void insertOfNodeItem();

private:
    QPixmap *pix;
    int _pixWidth = 2000;
    int _pixHeight = 1000;
    int _CycleRadiusMax = 20;
    int _diameter = _pixWidth/50;
    int _radius = _diameter / 2;
    int _nodeLineWidth = 3 * _diameter / 2;
    int _fontSize=_radius;

    QPixmap *pold;
    QTimer timer;
    bool _IsChangeWidgetSize = false;

    QVector<int> _qvectorForData;
    int _indexForQvector=0;
    Node<int> *root;
    NodeItem *_rootForDraw;
    Node<int> *NIL;
    NodeItem *_NILForDraw;
    Node<int> *_sentinel;
    NodeItem *_sentinelForDraw;
    QQueue<Node<int> *> _queueForNode;
    QQueue<NodeItem *> _queueForDraw;
    QVector<Action> _arrayForOrder;
    QHash<int, NodeItem *> _hashForNodeItem;
    int _step = 0;
    bool _isClearForDraw = false;

    int _nodeSize=0;
    int _xTranslationOffset=0;
    int _yTranslationOffset=0;

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
    void rotationWithLeftChild(Node<int> *&root);
    void rotationWithLeftChildForDraw(NodeItem *&root);
    void rotationWithRightChild(Node<int> *&root);
    void rotationWithRightChildForDraw(NodeItem *&root);
    void insertionFixUpOfDoubleRed(Node<int> *root);
    void removeFixUpOfLostOfBlack(Node<int> *root);
    void emptyRBtree(Node<int> *&root);
    Node<int> *&getParentReference(Node<int> *child) {
        if (child->parent == NIL) return this->root;
        return child == child->parent->left ? child->parent->left
                                            : child->parent->right;
    }
    NodeItem *&getParentReferenceForDraw(NodeItem *child) {
        if (child->_parent == _NILForDraw) return this->_rootForDraw;
        return child == child->_parent->_left ? child->_parent->_left
                                              : child->_parent->_right;
    }
    int findmin(const Node<int> *root) const;

    struct Action {
        Operator _ope;


    /* 1.增加节点( Add)
     *      索引0:新节点值
     *      索引1:父节点值
     * 2.旋转(Rotate)
     *      索引0:父节点值
     *      索引1:值为0与左孩子旋转,值为1与右孩子旋转
     * 3.变色(ChangeColor)
     *      0是红色,值1是黑色.大于1表示不存在当前节点
     *      索引0:当前节点值,索引1:当前节点值颜色
     *      索引2:当前节点值,索引3:当前节点值颜色
     *      索引4:当前节点值,索引5:当前节点值颜色
     *      三个键值对组成
     * 4.删除过程中具有两个子节点的被删除节点最小节点被替换成右子树
     *      0:要删除的节点
     *      1:找到的最小节点
     * 5.查找过程(Search)
     *      索引0:当前节点值
     * 6.下一个值(NextValue)
     *      索引0值固定为0
     * 7.插入或删除完成(Done)
     *      索引0值:当成功时为1,失败时0
     * 9.涂根节点为黑(PaintBlack)
     *      索引0值固定为0
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
