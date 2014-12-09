var str1 : string;
	str2 : string;
	k : integer;

begin
	{ return 0 }

	{ 1 }
	str1 := 'podelany text ktery chci do prdele seradit 42';
	str1 := sort(str1)
	if str1 <> '       24aaccddddeeeeeehiikllnoopprrrsttttxyy'
	begin
		write('SORT 1'#10'')
	end
	else
	begin
		k := 42
	end;

	{ 2 }
	str1 := '987654321';
	str1 := sort(str1)
	if str1 <> '123456789'
	begin
		write('SORT 2'#10'')
	end
	else
	begin
		k := 42
	end;

	{ 3 }
	str1 := '';
	str1 := sort(str1)
	if str1 <> ''
	begin
		write('SORT 3'#10'')
	end
	else
	begin
		k := 42
	end;

	{ 4 }
	str1 := ';789]  -+*/***-+534 n 34k2   kfjkanmncvmx,./;[]][[[!@(*&$#,,,??++__';
	str2 := sort(str1)
	if str2 <> '       !#$&(*****++++,,,,--.//233445789;;??@[[[[]]]__acfjkkkmmnnnvx'
	begin
		write('SORT 4'#10'')
	end
	else
	begin
		k := 42
	end;

	{ 5 }
	str1 := 'abcdefghijklmno';
	str1 := sort(str1)
	if str1 <> 'abcdefghijklmno'
	begin
		write('SORT 5'#10'')
	end
	else
	begin
		k := 42
	end;

	{ 6 } { NEVIM JESTLI TOTO MA FUNGOVAT NEBO NE }
	str1 := sort('bbbb'#9'aaaa')
	if str1 <> ''#9'aaaabbbb'
	begin
		write('SORT 6'#10'')
	end
	else
	begin
		k := 42
	end
end.