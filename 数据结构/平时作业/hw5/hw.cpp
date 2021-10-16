// 5.30: 基于拓展线性链表的定义编写求广义表深度的递归算法
int GListDepth(Glist L) {
	if (L->tag == 0) {
		return 0;
	}
	int max = 0;
	for (Glist* p = L->hp; p; p = p->tp) {
		int depth = GListDepth(p);
		if (depth > max) {
			max = depth;
		}
	}
	return max + 1;
}	// GListDepth

// 5.32: 试编写判断两个广义表是否相等的递归算法
int GEqual(Glist L1, Glist L2) {
	if (L1->tag != L2->tag) {
		return 0;
	}
	if (L1->tag == 0) {		// 如果是原子
		return L1->atom == L2->atom ? 1 : 0;
	}
	Glist* p1 = L1->hp, *p2 = L2->hp;
	while (p1 || p2) {
		if (!p1 || !p2) {	// 如果表长度不一致
			return 0;
		}
		if (!GEqual(p1, p2)) {	// 如果元素不一样
			return 0;
		}
	}
	return 1;
}
