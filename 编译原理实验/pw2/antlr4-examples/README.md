### Compile

- mkdir build
- cd build
- cmake .. -D=/full/path/to/antlr-4.8-complete.jar
  - you can download antlr-4.8-complete.jar from [ANTLR Download](http://www.antlr.org/download/)
  - If yun run in educoder, you should modify `target_link_libraries(demo ${CMAKE_CURRENT_SOURCE_DIR}/lib/libantlr4-runtime.so.4.8)` to `target_link_libraries(demo antlr4-runtime)`
- make
- LD_LIBRARY_PATH=../lib ./demo
  - If you run it in educoder, you can just run `./demo`.
  - The lib/libantlr4-runtime.so.4.8 is built in wsl(ubuntu).
  - If it don't work, you can follow the guide of [antlr4 cpp](https://github.com/antlr/antlr4/tree/master/runtime/Cpp#compiling-on-linux) and use the generated libantlr4-runtime.so.4.8.
- output

```
[@0,1:1='*',<22>,1:1]
[@1,2:2='(',<8>,1:2]
[@2,3:3='+',<20>,1:3]
[@3,4:4='-',<21>,1:4]
[@4,5:5='/',<23>,1:5]
[@5,6:6='*',<22>,1:6]
[@6,7:7='%',<24>,1:7]
[@7,8:8=')',<9>,1:8]
[@8,9:9='*',<22>,1:9]
[@9,11:10='<EOF>',<-1>,2:0]
```



