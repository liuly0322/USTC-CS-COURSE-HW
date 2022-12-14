define dso_local i32 @fact(i32 %0, i32 %1) {
  %nAlloca = alloca i32
  %accAlloca = alloca i32
  store i32 %0, i32* %nAlloca
  store i32 %1, i32* %accAlloca
  br label %start

start:
  %n = load i32, i32* %nAlloca
  %n_eq_1 = icmp eq i32 %n, 1
  br i1 %n_eq_1, label %true, label %false

true:
  %acc = load i32, i32* %accAlloca
  ret i32 %acc

false:
  %n_ = load i32, i32* %nAlloca
  %n_minus_1 = sub nsw i32 %n_, 1
  %acc_ = load i32, i32* %accAlloca
  %acc_times_n = mul nsw i32 %acc_, %n_
  %recur_res = call i32 @fact(i32 %n_minus_1, i32 %acc_times_n)
  ret i32 %recur_res
}

define dso_local i32 @main() {
  %r = call i32 @fact(i32 1000000, i32 1)
  ret i32 %r
}