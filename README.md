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
