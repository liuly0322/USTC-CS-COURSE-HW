define dso_local i32 @main() #0 {
loop:
  %i = alloca i32
  %char = call i32 @getchar()
  store i32 %char, i32* %i
  %char_ = load i32, i32* %i
  call void @putchar(i32 %char_)
  br label %loop
}

declare dso_local i32 @getchar() #1
declare dso_local void @putchar(i32) #1
