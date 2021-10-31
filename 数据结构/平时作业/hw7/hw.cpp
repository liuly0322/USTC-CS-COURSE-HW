// 6.38 利用栈的基本操作写出后序遍历的非递归算法

// 利用标志栈 S_flag。 0 表示在访问左子树，1 表示在访问右子树
Status PostOrderTraverse(BiTree T, Status (*Visit)(TElemType e)) {
    InitStack(S_tree);
    BiTree p = T;
    InitStack(S_flag);
    int flag = 0;
    while (p || !StackEmpty(S_tree)) {
        if (p) {
            Push(S_tree, p);
            Push(S_flag, flag);
            p = flag == 0 ? p->lchild : p->rchild;
            flag = 0;
        } else {
            Pop(S_tree, p);
            Pop(S_flag, flag);
            // 出栈可能是因为这个结点的左子树访问完了（接下来访问右子树）
            // 也可能是这个结点的右子树访问完了（接下来访问它自身）
            // 用 flag 的值判断
            if (flag == 0) {
                Push(S_tree, p);
                Push(S_flag, 1);  // 接下来要访问的是这个结点的右子树
                p = p->rchild;
                flag = 0;
                continue;
            }
            // 否则 flag 为 1，访问自身，维持退栈即可
            if (!Visit(p->data)) {
                return ERROR;
            }
            p = nullptr;
        }
    }
    return OK;
}

// 6.47 编写按层顺序
