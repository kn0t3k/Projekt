var str1 : string;
	str2 : string;
	k : integer;

begin
	{ return 0 }

	{ 1 }
	str1 := 'podelany text ktery chci do prdele seradit 42';
	str1 := sort(str1);
	write(str1, ''#10''); {'       24aaccddddeeeeeehiikllnoopprrrsttttxyy'}

	{ 2 }
	str1 := '987654321';
	str1 := sort(str1);
	write(str1, ''#10''); {'123456789'}

	{ 3 }
	str1 := '';
	str1 := sort(str1);
	write(str1, ''#10''); {''}

	{ 4 }
	str1 := ';789]  -+*/***-+534 n 34k2   kfjkanmncvmx,./;[]][[[!@(*&$#,,,??++__';
	str2 := sort(str1); 
	write(str2, ''#10''); {'       !#$&(*****++++,,,,--.//233445789;;??@[[[[]]]__acfjkkkmmnnnvx'}

	{ 5 }
	str1 := 'abcdefghijklmno';
	str1 := sort(str1);
	write(str1, ''#10'') {'abcdefghijklmno'}

end.