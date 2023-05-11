.class public CSourceToJava
.super java/lang/Object


.method public static main([Ljava/lang/String;)V
	sipush	5
	istore	0
	getstatic	java/lang/System/out Ljava/io/PrintStream;
	ldc	""
	invokevirtual	java/io/PrintStream/print(Ljava/lang/String;)V
	getstatic	java/lang/System/out Ljava/io/PrintStream;
	sipush	1
	invokestatic	CSourceToJava/seq(I)I
	invokevirtual	java/io/PrintStream/print(I)V
	getstatic	java/lang/System/out Ljava/io/PrintStream;
	ldc	"
"
	invokevirtual	java/io/PrintStream/print(Ljava/lang/String;)V
	getstatic	java/lang/System/out Ljava/io/PrintStream;
	ldc	"The value is "
	invokevirtual	java/io/PrintStream/print(Ljava/lang/String;)V
	getstatic	java/lang/System/out Ljava/io/PrintStream;
	iload	0
	invokestatic	CSourceToJava/seq(I)I
	invokevirtual	java/io/PrintStream/print(I)V
	getstatic	java/lang/System/out Ljava/io/PrintStream;
	ldc	"
"
	invokevirtual	java/io/PrintStream/print(Ljava/lang/String;)V
	return
.end method
.method public static seq(I)I
	iload	0
	sipush	3
if_icmpge branch1
	sipush	1
	istore	1
goto branch_out1
branch1:
	iload	0
	sipush	1
	isub
	invokestatic	CSourceToJava/seq(I)I
	iload	0
	sipush	2
	isub
	invokestatic	CSourceToJava/seq(I)I
	iadd
	istore	1
branch_out1:
	iload	1
	ireturn
.end method

.end class
