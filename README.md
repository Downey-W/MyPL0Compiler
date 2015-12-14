

    这是一个基于自底向上的LR分析法PL0编译器，构造PL0文法的SLR分析表，然后通过词法分析，语法分析，
判断code.txt的PL0程序是否符合规范。如果语法正确，则生成目标代码。（假定已有可以执行PL0机器指令的硬件执行）

说明：a.txt 包含所有文法符号
	  b.txt 包含所有产生式
	  标识符和文法1.docx  定义了符号说明
	  
	  示例程序：
		const a=10;
		var b,c;
		procedure p;
		begin
		c:=b+a
		end;
		begin
		read(b);
		while b#0 do
			begin
			call  p;
			write(2*c);
			read(b)
			end
		end.
		
		
		运行结果如图result.png
		
