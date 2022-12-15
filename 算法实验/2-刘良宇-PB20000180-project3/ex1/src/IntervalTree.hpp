#ifndef RBTREE
#define RBTREE

#include <algorithm>
#include <functional>
#include <memory>
#include <utility>

/**
 * @brief 区间树
 *
 */
class IntervalTree {
    // 结点结构体
    struct Node {
        // 子结点使用 shared_ptr，父结点使用 weak_ptr
        // 目的是为了保证自动的构造析构
        std::shared_ptr<Node> left;
        std::shared_ptr<Node> right;
        std::weak_ptr<Node> parent;

        // 颜色定义
        enum COLOR { BLACK = 0, RED };
        COLOR color;
        inline bool isRed() const { return color == RED; }
        inline bool isBlack() const { return color == BLACK; }
        inline void setRed() { color = RED; }
        inline void setBlack() { color = BLACK; }
        inline COLOR getColor() const { return color; }
        inline void setColor(COLOR c) { color = c; }

        // 存储区间
        int int_low;
        int int_high;
        // 红黑树的附加信息
        int max;

        Node() = default;
        Node(int int_low, int int_high)
            : int_low(int_low), int_high(int_high), max(int_high) {}
    };

  public:
    using shared = std::shared_ptr<Node>;
    using weak = std::weak_ptr<Node>;

  private:
    // 哨兵结点
    shared nil = std::make_shared<Node>();
    // 根结点指针
    shared root = nil;

    void leftRotate(const shared &x) {
        auto y = x->right;
        x->right = y->left;
        if (y->left != nil) {
            y->left->parent = x;
        }
        y->parent = x->parent;
        if (x->parent.lock() == nil) {
            root = y;
        } else if (x == x->parent.lock()->left) {
            x->parent.lock()->left = y;
        } else {
            x->parent.lock()->right = y;
        }
        y->left = x;
        x->parent = y;
        // 维护附加信息
        y->max = x->max;
        x->max = std::max(std::max(x->int_high, x->left->max), x->right->max);
    }

    void rightRotate(const shared &y) {
        auto x = y->left;
        y->left = x->right;
        if (x->right != nil) {
            x->right->parent = y;
        }
        x->parent = y->parent;
        if (y->parent.lock() == nil) {
            root = x;
        } else if (y == y->parent.lock()->right) {
            y->parent.lock()->right = x;
        } else {
            y->parent.lock()->left = x;
        }
        x->right = y;
        y->parent = x;
        // 维护附加信息
        x->max = y->max;
        y->max = std::max(std::max(y->int_high, y->left->max), y->right->max);
    }

    void transplant(const shared &u, const shared &v) {
        if (u->parent.lock() == nil) {
            root = v;
        } else if (u == u->parent.lock()->left) {
            u->parent.lock()->left = v;
        } else {
            u->parent.lock()->right = v;
        }
        v->parent = u->parent;
        // 维护新结点
        if (v != nil) {
            v->max =
                std::max(std::max(v->int_high, v->left->max), v->right->max);
        }
        // 从 v->parent 开始维护 max
        auto cur = v->parent.lock();
        while (cur != nil) {
            cur->max = std::max(std::max(cur->int_high, cur->left->max),
                                cur->right->max);
            cur = cur->parent.lock();
        }
    }

    shared minimum(shared node) {
        while (node->left != nil) {
            node = node->left;
        }
        return node;
    }

  public:
    IntervalTree() {
        nil->setBlack();
        // 初始化 nil 结点的 max 域
        nil->max = std::numeric_limits<int>::min();
    };

    static shared mkNode(int int_low, int int_high) {
        return std::make_shared<Node>(int_low, int_high);
    };

    shared getRoot() { return root; }
    shared getNil() { return nil; }

  private:
    void traverseRecursive(
        const std::function<void(const IntervalTree::shared &)> &f,
        const shared &node, int depth) {
        if (node == nil) {
            return;
        }
        traverseRecursive(f, node->left, depth + 1);
        f(node);
        traverseRecursive(f, node->right, depth + 1);
    }

  public:
    void traverse(const std::function<void(const IntervalTree::shared &)> &f) {
        traverseRecursive(f, root, 0);
    }

    void insertNode(shared z) {
        // insert
        weak y = nil;
        shared x = root;
        while (x != nil) {
            if (x->max < z->max)
                x->max = z->max;
            y = x;
            if (z->int_low < x->int_low)
                x = x->left;
            else
                x = x->right;
        }
        z->parent = y;
        if (y.lock() == nil) {
            root = z;
        } else if (z->int_low < y.lock()->int_low) {
            y.lock()->left = z;
        } else {
            y.lock()->right = z;
        }
        z->left = nil;
        z->right = nil;
        z->setRed();

        // fixup
        while (z->parent.lock()->isRed()) {
            // z->parent is red, so z->parent is not root
            if (z->parent.lock() == z->parent.lock()->parent.lock()->left) {
                // uncle left
                auto y = z->parent.lock()->parent.lock()->right;
                if (y->isRed()) {
                    z->parent.lock()->setBlack();
                    y->setBlack();
                    z->parent.lock()->parent.lock()->setRed();
                    z = z->parent.lock()->parent.lock();
                } else {
                    if (z == z->parent.lock()->right) {
                        z = z->parent.lock();
                        leftRotate(z);
                    }
                    z->parent.lock()->setBlack();
                    z->parent.lock()->parent.lock()->setRed();
                    rightRotate(z->parent.lock()->parent.lock());
                }
            } else {
                // uncle right
                auto y = z->parent.lock()->parent.lock()->left;
                if (y->isRed()) {
                    z->parent.lock()->setBlack();
                    y->setBlack();
                    z->parent.lock()->parent.lock()->setRed();
                    z = z->parent.lock()->parent.lock();
                } else {
                    if (z == z->parent.lock()->left) {
                        z = z->parent.lock();
                        rightRotate(z);
                    }
                    z->parent.lock()->setBlack();
                    z->parent.lock()->parent.lock()->setRed();
                    leftRotate(z->parent.lock()->parent.lock());
                }
            }
        }
        root->setBlack();
    }

    void intervalInsert(int int_low, int int_high) {
        insertNode(std::make_shared<Node>(int_low, int_high));
    }

  private:
    void deleteNodeFix(shared x) {
        shared w;
        while (x != root && x->isBlack()) {
            if (x == x->parent.lock()->left) {
                w = x->parent.lock()->right;
                if (w->isRed()) {
                    w->setBlack();
                    x->parent.lock()->setRed();
                    leftRotate(x->parent.lock());
                    w = x->parent.lock()->right;
                }
                if (w->left->isBlack() && w->right->isBlack()) {
                    w->setRed();
                    x = x->parent.lock();
                } else {
                    if (w->right->isBlack()) {
                        w->left->setBlack();
                        w->setRed();
                        rightRotate(w);
                        w = x->parent.lock()->right;
                    }
                    w->color = x->parent.lock()->color;
                    x->parent.lock()->setBlack();
                    w->right->setBlack();
                    leftRotate(x->parent.lock());
                    x = root;
                }
            } else {
                w = x->parent.lock()->left;
                if (w->isRed()) {
                    w->setBlack();
                    x->parent.lock()->setRed();
                    rightRotate(x->parent.lock());
                    w = x->parent.lock()->left;
                }
                if (w->right->isBlack() && w->left->isBlack()) {
                    w->setRed();
                    x = x->parent.lock();
                } else {
                    if (w->left->isBlack()) {
                        w->right->setBlack();
                        w->setRed();
                        leftRotate(w);
                        w = x->parent.lock()->left;
                    }
                    w->color = x->parent.lock()->color;
                    x->parent.lock()->setBlack();
                    w->left->setBlack();
                    rightRotate(x->parent.lock());
                    x = root;
                }
            }
            x->setBlack();
        }
    }

    void deleteNode(const shared &z) {
        std::shared_ptr<Node> x;
        auto y = z;
        auto y_original_color = y->getColor();
        if (z->left == nil) {
            x = z->right;
            transplant(z, x);
        } else if (z->right == nil) {
            x = z->left;
            transplant(z, x);
        } else {
            y = minimum(z->right);
            y_original_color = y->color;
            x = y->right;
            if (y->parent.lock() == z) {
                x->parent = y;
            } else {
                transplant(y, x);
                y->right = z->right;
                y->right->parent = y;
            }

            transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;
        }
        if (y_original_color == Node::BLACK) {
            deleteNodeFix(x);
        }
    }

  public:
    inline void intervalDelete(const shared &node) { deleteNode(node); }

    shared intervalSearch(int int_low, int int_high) {
        auto x = root;
        while (x != nil && (int_low > x->int_high || x->int_low > int_high)) {
            if (x->left != nil && x->left->max >= int_low)
                x = x->left;
            else
                x = x->right;
        }
        return x;
    }
};
#endif