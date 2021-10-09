#include <iostream>
typedef int Status;
#define OK 1
#define ERROR 0

#define CHUNKSIZE 80
typedef struct Chunk {
    char ch[CHUNKSIZE];
    struct Chunk* next;
} Chunk;

typedef struct {
    Chunk *head, *tail;  // 串的头和尾指针
    int curlen;          // 串当前长度
} LString;

// 4.22: 编写将串 s 插入到串 t 中某个字符之后的算法（不存在则 s 连接到 t
// 末尾）的算法 这里约定用 '\0' 表示当前块储存的字符到此结束

Status insert(LString& s, LString& t, char target) {
    // 首先要找到 target 在哪一块哪一位
    int index;
    for (Chunk* now = t.head; now; now = now->next) {
        bool flag = false;
        for (index = 0; index < CHUNKSIZE && now->ch[index]; index++) {
            if (now->ch[index] == target) {
                flag = true;
                break;
            }
        }
        if (flag)
            break;
    }
    if (!now) {  // 找不到
        t.tail->next = s.head;
        t.curlen += s.curlen;
        return OK;
    }
    Chunk* t_left = new Chunk();  // 新建块储存 target 之后的内容
    int i;
    for (i = 0; now->ch[index + i + 1] && index + i + 1 < CHUNKSIZE; i++) {
        t_left->ch[i] = now->ch[index + i + 1];
    }
    t_left->ch[i] = '\0';
    t_left->next = nullptr;
    t.tail->ch[index + 1] = '\0';
    t.tail->next = s.head;
    s.tail->next = t_left;
    t.curlen += s.curlen;
    return OK;
}