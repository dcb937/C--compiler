## 天津大学编译原理大作业

本项目为天津大学计算机科学与技术2020级编译原理大作业
时间：2023.4-2023.5
作者：庞业诵，丁成博，赵志坚，王琨

### 目录

```
C--compiler
   |- include     包含各种头文件
   |- grammar.txt 
   |- makefile    
   |- README.md
   |- src:
      |- LexicalAnaly.cpp     词法分析器
      |- Formula.cpp          文法解析定义
      |- SyntaxAnaly.cpp      文法解析，区分符号性质
      |- utils.cpp            实现First、Follow集和预测表
      |- main.cpp             利用词法分析结果语法分析


   |- testcases   测试用例

   |- output      文件输出地点
      |- first.txt                first集
      |- follow.txt               follow集
      |- formula.txt              文法分析结果
      |- nonterminal.txt          非终结符
      |- terminal.txt             终结符
      |- predict.txt              预测表
      |- lex.txt             词法分析结果
      |- IntermediateResults.txt  语法分析输入
      |- gra.txt                  语法分析结果
```



###  编译及测试

修改src/LexicalAnaly.cpp中第14行测试用例后先make编译生成可执行文件，再make run运行可执行文件即可在output里得到相应结果