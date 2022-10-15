# weak_ptr

## 若释放导致引用计数归零

则无法再次访问对象

如 `test1` 所示，在 `p2: shared_ptr<int>` 作用域外，该共享指针被释放，因而 `p1: weak_ptr<int>` 获取到的引用计数为 0，即判断指针对应资源已经被释放

```cpp
long
use_count() const noexcept
{ return _M_refcount._M_get_use_count(); }

bool
expired() const noexcept
{ return _M_refcount._M_get_use_count() == 0; }
```

因此通过 `lock` 获取到的是一个空指针，无法再次访问资源

## 若释放后引用计数不为零

如 `test2` 所示，虽然 `p3: shared_ptr<int>` 被释放，但是该共享资源的引用计数没有归零，`p2: shared_ptr<int>` 仍然持有资源，所以 `p1: weak_ptr<int>` 在 `p3` 释放后仍然可以访问资源