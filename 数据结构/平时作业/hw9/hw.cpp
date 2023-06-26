// 6.73:
// 若用大写字母标识树的结点，则可用带标号的广义表形式表示一棵树，语法图如题图所示。
// 试编写一个递归算法，由这种广义表表示的字符序列构造树的孩子 - 兄弟链表（提示：
// 按照森林和树相互递归的定义写两个互相递归调用的算法，语法图中一对圆括号内的部分
// 可看成森林的语法图）

Tree InitTree(char* s, int length) {
    // 这里拓展成构造森林的算法
    // 举例一个合理的输入：A(B(E,F),C),B,C
    Tree head = new TreeNode();
    head->data = s[0];
    // 先确定头结点的孩子
    if (length > 1 && s[1] = '(') {
        // 括号内部也是一个森林
        int lbrace = 1, i;
        for (i = 2; lbrace; i++) {
            if (s[i] == '(')
                lbrace++;
            if (s[i] == ')')
                lbrace--;
        }
        // 括号内部从 2 到 i - 2
        // 这里假设输入合法，也就是假设括号内部长度至少为 1
        head->firstChild = InitTree(s + 2, i - 3);
    } else {
        head->firstChild = nullptr;
    }
    // 再确定头结点的兄弟
    head->nextSibling = nullptr;
    for (int i = 0; i < length; i++) {
        if (s[i] == ',') {
            head->nextSibling = InitTree(s + i + 1, length - i - 1);
            break;
        }
    }
    return head;
}

// 7.16: 试在邻接表存储结构上实现图的基本操作：InsertVex(G, v), InsertArc(G, v,
// w), DeleteVex(G, v) 和 DeleteArc(G, v, w)

Status InsertVex(Graph& G, VexNode v) {
    if (LocateVex(G, v) != -1 || G.vexnum = MAX_VERTEX_NUM)
        return ERROR;
    G.vertices[G.vexnum++] = v;
    return OK:
}

Status InsertArc(Graph& G, VexNode v, VexNode w) {
    int v_index = LocateVex(G, v), w_index = LocateVex(G, w),
        if (v_index == -1 || w_index == -1) return ERROR;
    // 先添加 v 到 w 的边
    ArcNode* arc = new ArcNode();
    arc->adjvex = w_index;
    arc->InfoType = nullptr;
    arc->nextarc = G.vertices[v_index]->firstarc;
    G.vertices[v_index]->firstarc = arc;
    // 如果是无向图/网（UDG，UDN），则两条边都要添加
    if (G.kind == UDG || G.kind == UDN) {
        arc = new ArcNode();
        arc->adjvex = v_index;
        arc->InfoType = nullptr;
        arc->nextarc = G.vertices[w_index]->firstarc;
        G.vertices[w_index]->firstarc = arc;
    }
    return OK;
}

Status DeleteVex(Graph& G, VexNode v) {
    // 先遍历所有顶点，把 v 和 最后一个顶点交换
    // 考虑到这不是本题的重点，交换过程略去
    for (int i = 0; i < G.vexnum - 1; i++) {
        // 这里遍历除了 v 之外的所有顶点
        DeleteArc(G, G.vertices[i], v);
    }
    // 接下来删除 v 的所有边和 v 这个点
    int n = G.vexnum - 1;  // v 点
    while (G.vertices[n]->firstarc) {
        auto temp = G.vertices[n]->firstarc;
        G.vertices[n]->firstarc = G.vertices[n]->firstarc->nextarc;
        delete temp;
        G.arcnum--;
    }
    G.vexnum--;
}

Status DeleteArc(Graph& G, VexNode v, VexNode w) {
    int v_index = LocateVex(G, v), w_index = LocateVex(G, w);
    if (v_index == -1 || w_index == -1)
        return ERROR;
    // 先删除 v 到 w 的边
    if (G.vertices[v_index]->firstarc &&
        G.vertices[v_index]->firstarc->adjvex == w_index) {
        auto temp = G.vertices[v_index]->firstarc;
        G.vertices[v_index]->firstarc = G.vertices[v_index]->firstarc->nextarc;
        delete temp;
        G.arcnum--;
    } else {
        for (auto p = G.vertices[v_index]->firstarc; p->nextarc;
             p = p->nextarc) {
            if (p->nextarc->adjvex == w_index) {
                auto temp = p->nextarc;
                p->nextarc = p->nextarc->nextarc;
                delete temp;
                G.arcnum--;
                break;
            }
        }
    }
    // 如果是无向图/网（UDG，UDN），则还要删除 w 到 v 的边
    if (G.vertices[w_index]->firstarc &&
        G.vertices[w_index]->firstarc->adjvex == v_index) {
        auto temp = G.vertices[w_index]->firstarc;
        G.vertices[w_index]->firstarc = G.vertices[w_index]->firstarc->nextarc;
        delete temp;
        G.arcnum--;
    } else {
        for (auto p = G.vertices[w_index]->firstarc; p->nextarc;
             p = p->nextarc) {
            if (p->nextarc->adjvex == v_index) {
                auto temp = p->nextarc;
                p->nextarc = p->nextarc->nextarc;
                delete temp;
                G.arcnum--;
                break;
            }
        }
    }
    return OK;
}
