#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#define COMMAND

double dabs(double x) { return x > 0 ? x : -x; }

class Node { // 结点
  public:
    Node *next;
    double coef; // 系数
    int expn;    // 指数
    Node() : next(nullptr) {}
    Node(double c, int e) : coef(c), expn(e), next(nullptr) {}
    ~Node() {}
};

class Poly { // 多项式 (假定指数非递增)
  public:
    Node *next;
    Poly() : next(nullptr) {}
    ~Poly() {
        Node *p;
        while (next) {
            p = next->next;
            delete next;
            next = p;
        }
    }
    Poly &operator+(const Poly &p) {
        Node *node1 = this->next, *node2 = p.next;
        Poly *ans = new Poly();
        Node temp;
        while (node1 && node2) {
            if (node1->expn == node2->expn) { // 等于
                temp.coef = node1->coef + node2->coef;
                temp.expn = node1->expn;
                ans->insert(temp);
                node1 = node1->next;
                node2 = node2->next;
            } else if (node1->expn > node2->expn) { // 大于
                temp.coef = node1->coef;
                temp.expn = node1->expn;
                ans->insert(temp);
                node1 = node1->next;
            } else { // 小于
                temp.coef = node2->coef;
                temp.expn = node2->expn;
                ans->insert(temp);
                node2 = node2->next;
            }
        }
        while (node1) {
            temp.coef = node1->coef;
            temp.expn = node1->expn;
            ans->insert(temp);
            node1 = node1->next;
        }
        while (node2) {
            temp.coef = node2->coef;
            temp.expn = node2->expn;
            ans->insert(temp);
            node2 = node2->next;
        }
        return *ans;
    }
    Poly &operator-(const Poly &p) {
        Node *node1 = this->next, *node2 = p.next;
        Poly *ans = new Poly();
        Node temp;
        while (node1 && node2) {
            if (node1->expn == node2->expn) { // 等于
                temp.coef = node1->coef - node2->coef;
                temp.expn = node1->expn;
                ans->insert(temp);
                node1 = node1->next;
                node2 = node2->next;
            } else if (node1->expn > node2->expn) { // 大于
                temp.coef = node1->coef;
                temp.expn = node1->expn;
                ans->insert(temp);
                node1 = node1->next;
            } else { // 小于
                temp.coef = -node2->coef;
                temp.expn = node2->expn;
                ans->insert(temp);
                node2 = node2->next;
            }
        }
        while (node1) {
            temp.coef = node1->coef;
            temp.expn = node1->expn;
            ans->insert(temp);
            node1 = node1->next;
        }
        while (node2) {
            temp.coef = -node2->coef;
            temp.expn = node2->expn;
            ans->insert(temp);
            node2 = node2->next;
        }
        return *ans;
    }
    Poly &operator*(const Poly &p) {
        Poly *ans = new Poly();
        Node temp;                                 // 暂存，留待插入
        Node *head1 = this->next, *head2 = p.next; // 乘数和被乘数的开头
        for (auto pi = head1; pi; pi = pi->next) {
            for (auto pj = head2; pj; pj = pj->next) {
                temp.expn = pi->expn + pj->expn;
                temp.coef = pi->coef * pj->coef;
                ans->insert(temp);
            }
        }
        return *ans;
    }
    void operator+=(const Poly &p) { // 自增
        Poly ans;
        ans = *this + p;
        copy(ans);
    }
    void operator-=(const Poly &p) { // 自减
        Poly ans;
        ans = *this - p;
        copy(ans);
    }
    Poly &operator/=(const Poly &p) { // 自除。自己会变成商，返回的是余数。
        Poly *dividend = new Poly();
        dividend->copy(*this); // 被除数拷贝当前值
        this->~Poly();         // 先析构，回归空多项式，变成商。
        Poly multi;
        Node *tmp = new Node(); // 新建乘的因子
        multi.next = tmp;
        while (dividend->next) {                       // 非空
            if (dividend->next->expn < p.next->expn) { // 无法再除
                break;
            }
            tmp->coef = dividend->next->coef / p.next->coef;
            tmp->expn = dividend->next->expn - p.next->expn;
            this->operator+=(multi); // 自增因子
            Poly multi_res = multi * p;
            dividend->operator-=(multi_res);
        }
        return *dividend;
    }
    void copy(const Poly &p) { // 拷贝
        this->~Poly();         // 先释放自己内存
        Node *n = p.next, *next = (Node *)this;
        while (n) {
            Node *now = new Node(n->coef, n->expn);
            next->next = now;
            next = now;
            n = n->next;
        }
    }
    int length() {
        Node *p = next;
        int count = 0;
        while (p) {
            count++;
            p = p->next;
        }
        return count;
    }
    double getValue(double x) {
        double res = 0;
        Node *p = next;
        while (p) {
            res += pow(x, p->expn) * p->coef;
            p = p->next;
        }
        return res;
    }
    void diff() {
        Node *p = (Node *)this;
        while (p->next) {
            if (p->next->expn == 0) { // 如果后继次数是 0（常数项）
                Node *temp = p->next->next;
                delete p->next;
                p->next = temp;
                continue;
            }
            p->next->coef *= (p->next->expn)--;
            p = p->next;
        }
    }
    void insert(Node &node) { // 插入到链表内，自动寻找合适的位置（顺便去重）
        Node *p = next,
             *pre =
                 (Node *)this; // 找到 node 应该插入在哪个 p 的前面（如果需要）
        if (dabs(node.coef) < 0.0001) {
            return;
        }
        while (p) {
            if (p->expn == node.expn) {
                p->coef += node.coef;
                if (dabs(p->coef) < 0.0001) { //  此时认为该结点应该删除
                    pre->next = p->next;
                    delete p;
                }
                return;
            }
            if (p->expn < node.expn)
                break;
            p = p->next;
            pre = pre->next;
        }
        // 应该 node 插入在 p 前
        Node *now =
            new Node(node.coef, node.expn); // 统一在链表内进行内存分配操作
        now->next = p;
        pre->next = now;
    }
    void show() {
        Node *p = next;
#ifdef COMMAND
        if (!p) {
            std::cout << '0';
            return;
        }
        while (p) {
            std::cout << p->coef << ' ' << p->expn << ' ';
            p = p->next;
        }
#endif // COMMAND
#ifndef COMMAND
        if (!p) {
            std::cout << "0.0";
            return;
        }
        while (p) {
            printf("%.1lf %d ", p->coef, p->expn);
            p = p->next;
        }
#endif // !COMMAND
    }
};

#ifndef COMMAND
void readPoly(Poly &p1, Poly &p2) { // 读入前两行（包含末尾换行）
    Node temp;
    char c;
    // 读入第一行
    for (int i = 0; true; i++) {
        std::cin >> temp.coef;
        if ((c = getchar()) == '\n') { // 防止空多项式
            break;
        }
        std::cin >> temp.expn;
        p1.insert(temp);
        if ((c = getchar()) == '\n') {
            break;
        }
    }
    // 读入第二行
    for (int i = 0; true; i++) {
        std::cin >> temp.coef;
        if ((c = getchar()) == '\n') {
            break;
        }
        std::cin >> temp.expn;
        p2.insert(temp);
        if ((c = getchar()) == '\n') {
            break;
        }
    }
}
#endif // !COMMAND

#ifdef COMMAND
void readPolyCommand(Poly &p1, Poly &p2, int argc, char **argv) {
    int num1 = atoi(argv[2]), num2 = atoi(argv[3]); // 两行的数字数，0 表示空
    Node temp; // 为了插入到链表内建立临时变量
    for (int i = 1; i < num1; i += 2) {
        temp.coef = atof(argv[i + 3]);
        temp.expn = atoi(argv[i + 4]);
        p1.insert(temp);
    }
    for (int i = 1; i < num2; i += 2) {
        temp.coef = atof(argv[i + num1 + 3]);
        temp.expn = atoi(argv[i + num1 + 4]);
        p2.insert(temp);
    }
}

void readPolyCommand(Poly &p, int argc, char **argv) {
    int num = atoi(argv[2]); // 数字数，0 表示空
    Node temp;               // 为了插入到链表内建立临时变量
    for (int i = 1; i < num; i += 2) {
        temp.coef = atof(argv[i + 3]);
        temp.expn = atoi(argv[i + 4]);
        p.insert(temp);
    }
}
#endif // COMMAND

int main(int argc, char **argv) {
    // argv[1] 为 指定的操作（op）
    // 对于多行输入而言：argv[2] 为 第一行数字个数，argv[3] 为
    // 第二行数字个数，后续为具体数字 对于单行输入而言：argv[2] 为
    // 第一行数字个数，argv[3] 为 在哪一点求值（对于求导，值为 0
    // ），后续为具体数字
    Poly p1, p2, ans;
#ifdef COMMAND
    int op = atoi(argv[1]);
    double res; // 求值用
    switch (op) {
    case 0: // 加法
        readPolyCommand(p1, p2, argc, argv);
        ans = p1 + p2;
        ans.show();
        break;
    case 1: // 减法
        readPolyCommand(p1, p2, argc, argv);
        ans = p1 - p2;
        ans.show();
        break;
    case 2: // 乘法
        readPolyCommand(p1, p2, argc, argv);
        ans = p1 * p2;
        ans.show();
        break;
    case 3: // 求值
        readPolyCommand(p1, argc, argv);
        res = p1.getValue(atof(argv[3]));
        std::cout << res;
        break;
    case 4: // 微分
        readPolyCommand(p1, argc, argv);
        p1.diff();
        p1.show();
        break;
    case 5: // 除法
        readPolyCommand(p1, p2, argc, argv);
        ans = p1 /= p2; // ans 是余数，p1 是商
        p1.show();
        std::cout << '\n';
        ans.show();
        break;
    default:
        break;
    }
#endif // COMMAND
#ifndef COMMAND
    readPoly(p1, p2);
    char c = getchar();
    if (c == '0') {
        ans = p1 + p2;
    } else {
        ans = p1 /= p2;
        p1.show(); // 余数
        std::cout << std::endl;
    }
    ans.show();
#endif // !COMMAND
}