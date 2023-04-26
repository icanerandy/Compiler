# Compiler
icanerandy's Compiler Project

## 词法分析

## 语法分析
### 语法制导
- 给每条产生式规则附加一个语义动作（一条代码片段）
  - 语义动作在产生式“**归约**”时执行
  - 即由**右部**的值计算**左部**的值
  - 以自底向上的计数为例进行讨论（自顶向下的技术与此类似）
```
if (action[s, t] == "ri")
  ai
  pop(βi)
  state s' = stack[top]
  push (X)
  push (goto[s', X])
```
