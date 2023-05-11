.class public CSourceToJava
.super java/lang/Object


.method public static main([Ljava/lang/String;)V
	sipush	24
	istore	0
	getstatic	java/lang/System/out Ljava/io/PrintStream;
	ldc	"result is "
	invokevirtual	java/io/PrintStream/print(Ljava/lang/String;)V
	getstatic	java/lang/System/out Ljava/io/PrintStream;
	iload	0
	invokestatic	CSourceToJava/f(I)I
	invokevirtual	java/io/PrintStream/print(I)V
	getstatic	java/lang/System/out Ljava/io/PrintStream;
	ldc	"
"
	invokevirtual	java/io/PrintStream/print(Ljava/lang/String;)V
	return
.end method
.method public static f(I)I
	iload	0
	sipush	1
if_icmpne branch1
	sipush	1
	ireturn
goto branch_out1
branch1:
	iload	0
	sipush	2
if_icmpne branch2
	sipush	2
	ireturn
goto branch_out2
branch2:
	iload	0
	sipush	3
if_icmpne branch3
	sipush	4
	ireturn
goto branch_out3
branch3:
branch_out3:
branch_out2:
branch_out1:
	iload	0
	sipush	1
	isub
	invokestatic	CSourceToJava/f(I)I
	iload	0
	sipush	2
	isub
	invokestatic	CSourceToJava/f(I)I
	iload	0
	sipush	3
	isub
	invokestatic	CSourceToJava/f(I)I
	iadd
	iadd
	istore	1
	iload	1
	ireturn
.end method

.end class
