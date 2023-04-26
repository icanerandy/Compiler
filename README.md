# Compiler
icanerandy's Compiler Project

## 词法分析

## 语法分析
### 语法制导
- 给每条产生式规则附加一个语义动作（一条代码片段）
  - 语义动作在产生式“**归约**”时执行
  - 即由**右部**的值计算**左部**的值
  - 以自底向上的计数为例进行讨论（自顶向下的技术与此类似）
```cpp
if (action[s, t] == "ri")
  ai  // 语义动作
  pop(βi)
  state s' = stack[top]
  push (X)
  push (goto[s', X])
```
在分析栈上维护三元组：<symbol, value, state>
其中**symbol**是终结符或非终结符，**value**是symbol所拥有的值，**state**是当前的分析状态
### 抽象语法树
#### 分析树
- 分析树编码了句子的推导过程
- 但是包含了很多不必要的信息（注：这些节点要占用额外的存储空间）
- 本质上，这里的哪些信息是重要的？
- ![image](https://user-images.githubusercontent.com/97227323/234457726-8aee58e7-e0e8-4935-a3a4-89db7d076e50.png)
- 对于**表达式**而言，编译器只需要知道运算符和运算数（优先级、结合性等已经在语法分析部分处理掉了）
- 对于**语句**、**函数**等语言其它构造也一样
  - 例如，编译器不关心赋值符号是=还是:=或其它
