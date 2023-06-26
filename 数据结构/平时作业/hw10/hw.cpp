// 数据结构第十次作业
// 7.22 7.27 非递归 DFS 遍历图 7.7 最小生成树 7.34

// 7.22
// 试基于图的 dfs 策略写一算法，判别按邻接表存储的有向图中是否存在两个顶点之间的路径
bool is_path(Graph& g, int i, int j, bool reset = true) {
    static bool is_visit[MAX_VERTEX_NUM];
    if (reset) {
        for (int i = 0; i < g.vexnum; i++) {
            is_visit[i] = false;
        }
    }
    if (i == j)
        return true;
    if (is_visit[i])
        return false;
    is_visit[i] = true;
    for (ArcNode* p = g.vertices[i]->firstArc; p; p = p->nextArc) {
        if (is_path(g, p->adjvex, j, false))
            return true;
    }
    return false;
}

// 7.27 采用邻接表存储结构，编写一个判别无向图中任意给定的两个顶点是否存在一条
// 长度为 k 的简单路径的算法

bool is_path(Graph& g, int i, int j, int k, int depth = 0) {
    static int* path;  // 记录当前路径
    if (depth == 0) {
        path = new int[k]();
    }
    if (i == j && depth == k)
        return true;
    if (depth == k)
        return false;
    // 如果这个点在当前路径上曾经经过，那么就不要
    for (int _i = 0; _i < depth; _i++) {
        if (i == path[_i])
            return false;
    }
    path[depth] = i;
    for (ArcNode* p = g.vertices[i]->firstArc; p; p = p->nextArc) {
        bool res = is_path(g, p->adjvex, j, k - 1, false);
        if (res && depth == 0) {
            delete path;
            return true;
        }
        if (res) {
            return true;
        }
    }
    if (depth == 0)
        delete path;
    return false;
}

// 图的非递归 DFS 遍历
bool isVisited(std::vector<int>* Visited, int t) {
    int size = visited->size();
    for (int i = 0; i < size; i++) {
        if ((*Visited)[i] == t)
            return true;
    }
    return false;
}
void Graph_DFS_Traverse(Graph* G, int S) {
    // S 是起始顶点序号
    std::vector<int> Visited;
    std::stack<int> stk;
    stk.push(S);
    while (stk.size()) {
        int cur = stk.top();
        stk.pop();
        if (!isVisited(&Visited, cur)) {
            Visited.push(cur);
            for (auto trav = G->vertices[i]->firstArc; trav != nullptr;
                 trav = trav->nextArc) {
                if (!isVisited(&Visited, trav->adjVex)) {
                    stk.push(trav->adjVex);
                }
            }
        }
    }
}

// 7.7 最小生成树（画图）

// 7.34 试给有向无环图中结点拓扑排序

// 每次找到入度为 0 的顶点即可
void get_current_in(Graph& g, int* in) {
    for (int i = 0; i < g.vexnum; i++) {
        in[i] = 0;
    }
    for (int i = 0; i < g.vexnum; i++) {
        for (Arcnode* p = g.vertices[i]->firstArc; p; p = p->nextArc) {
            in[p->adjvex]++;
        }
    }
}

void sort(Graph& g, int* out) {
    // 输出到 out 数组，从 0 开始，是结点的编号
    int now = 0;  // 现在到了 out 的第几位
    // 首先申请一个数组统计各个结点的入度
    int in = new int[g.vexnum]();
    while (true) {
        get_current_in(g, in);
        if (g.vexnum == 0)
            break;
        for (int i = 0; i < g.vexnum; i++) {
            if (in[i] == 0) {
                DeleteVex(g, i);
                out[now++] = i;
            }
        }
    }
}
