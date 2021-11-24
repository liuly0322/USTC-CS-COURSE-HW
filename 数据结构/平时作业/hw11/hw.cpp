// 7.28 7.31 7.37

// 7.28 已知有向图和图中两个顶点 u, v, 编写算法求 u 到 v 所有简单路径
// 假设所有路径存储在二维数组 int[100][100] 中，num 标识有多少条路径

int path_num = 0;
int path[100][100] = { { 0 } };

void find_path(Graph& G, int u, int v, int depth = 0) {
	if (u == v) {
		path_num++;
		path[path_num][depth] == v;
		for (int i = 0; i < depth; i++) {
			path[path_num][i] = path[path_num - 1][i];
		}
		return;
	}
	// 每次先看当前的 u 在之前路径中是否出现过
	for (int i = 0; i < depth; i++) {
		if (path[path_num][i] == u)
			return;
	}
	path[path_num][depth] = u;
	// 递归遍历相邻顶点
	for (ArcNode* p = G.vertices[u]->firstArc; p; p = p-> nextArc) {
		find_path(G, p->adjVex, v, depth + 1);
	}
}

// 7.31 试完善求有向图的强连通分量的算法
