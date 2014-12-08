var str1 : string;
	str2 : string;
	i : integer;

begin
	{ return 0 }

	{ 1 }
	str1 := 'nejaky kratky text';
	str2 := 'nejaky';
	i := find(str1, str2);
	if i <> 1 then
	begin
		write('FIND 1'#10'')
	end;

	{ 2 }
	str2 := 'kratky';
	i := find(str1, str2);
	if i <> 8 then
	begin
		write('FIND 2'#10'')
	end;

	{ 3 }
	str2 := 'text';
	i := find(str1, str2);
	if i <> 15 then
	begin
		write('FIND 3'#10'')
	end;

	{ 4 }
	str2 := 'ne';
	i := find(str1, str2);
	if i <> 1 then
	begin
		write('FIND 4'#10'')
	end;

	{ 5 }
	str2 := 'y';
	i := find(str1, str2);
	if i <> 6 then
	begin
		write('FIND 5'#10'')
	end;

	{ 6 }
	str2 := 'ext';
	i := find(str1, str2);
	if i <> 16 then
	begin
		write('FIND 6'#10'')
	end;

	{ 7 }
	str2 := 'xt';
	i := find(str1, str2);
	if i <> 17 then
	begin
		write('FIND 7'#10'')
	end;

	{ 8 }
	str2 := 'ky kratky';
	i := find(str1, str2);
	if i <> 5 then
	begin
		write('FIND 8'#10'')
	end;

	{ 9 }
	str2 := '';
	i := find(str1, str2);
	if i <> 1 then
	begin
		write('FIND 9'#10'')
	end;

	{ 10 }
	str2 := 'baba';
	i := find(str1, str2);
	if i <> 0 then
	begin
		write('FIND 10'#10'')
	end;

	{ 11 }
	str2 := 'alabama fdjk kljklw jkwjk';
	i := find(str1, str2);
	if i <> 0 then
	begin
		write('FIND 11'#10'')
	end;

	{ 12 }
	str2 := 'nejaky kratky text';
	i := find(str1, str2);
	if i <> 1 then
	begin
		write('FIND 12'#10'')
	end;

	{ 13 }
	str1 := '';
	str2 := 'aaa';
	i := find(str1, str2);
	if i <> 0 then
	begin
		write('FIND 13'#10'')
	end;

	{ 14 }
	str2 := '';
	i := find(str1, str2);
	if i <> 1 then
	begin
		write('FIND 14'#10'')
	end;

	{ 15 }
	str1 := 'b';
	str2 := '';
	i := find(str1, str2);
	if i <> 1 then
	begin
		write('FIND 15'#10'')
	end;

	{ 16 }
	str2 := 'b';
	i := find(str1, str2);
	if i <> 1 then
	begin
		write('FIND 16'#10'')
	end;

	{ 17 }
	str2 := 'karas';
	i := find(str1, str2);
	if i <> 0 then
	begin
		write('FIND 17'#10'')
	end;

	{ 18 }
	str1 := 'ahoj ja jsem borec456 a nakopu ti prdel ty demente!';
	str2 := '';
	i := find(str1, str2);
	if i <> 1 then
	begin
		write('FIND 18'#10'')
	end;

	{ 19 }
	str2 := 'ahoj';
	i := find(str1, str2);
	if i <> 1 then
	begin
		write('FIND 19'#10'')
	end;

	{ 20 }
	str2 := '456';
	i := find(str1, str2);
	if i <> 19 then
	begin
		write('FIND 20'#10'')
	end;

	{ 21 }
	str2 := 'prdel';
	i := find(str1, str2);
	if i <> 35 then
	begin
		write('FIND 21'#10'')
	end;

	{ 22 }
	str2 := '!';
	i := find(str1, str2);
	if i <> 51 then
	begin
		write('FIND 22'#10'')
	end;

	{ 23 }
	str1 := '54f45  fsd;fsf;wetweqteeer   4    t4r5we464t ;][  ;  ee ;ewwww  ;tew;trey54 45675y45645445645   y4155241561545   52112 ; ;; ';
	str2 := '5211';
	i := find(str1, str2);
	if i <> 114 then
	begin
		write('FIND 23'#10'')
	end;

	{ 24 }
	str2 := '54f45  ttt';
	i := find(str1, str2);
	if i <> 0 then
	begin
		write('FIND 24'#10'')
	end;

	{ 25 }
	str2 := 'x4f45  fsd';
	i := find(str1, str2);
	if i <> 0 then
	begin
		write('FIND 25'#10'')
	end;

	{ 26 }
	str2 := '   4    t4r5we464t ;][  ;  ee ;ewwww  ;tew;trey54 45675y45645445645   y4155241561545   52112 ; ;; ';
	i := find(str1, str2);
	if i <> 27 then
	begin
		write('FIND 26'#10'')
	end;

	{ 27 }
	str2 := '   y';
	i := find(str1, str2);
	if i <> 94 then
	begin
		write('FIND 27'#10'')
	end;

	{ 28 }
	str2 := '4    ';
	i := find(str1, str2);
	if i <> 30 then
	begin
		write('FIND 28'#10'')
	end;

	{ 29 }
	str2 := '   4';
	i := find(str1, str2);
	if i <> 27 then
	begin
		write('FIND 29'#10'')
	end;

	{ 30 }
	str2 := '54f45  fsd;fsf;wetweqteeer   4    t4r5we464t ;][  ;  ee ;ewwww  ;tew;trey54 45675y45645445645   y4155241561545   52112 ; ;; ';
	i := find(str1, str2);
	if i <> 1 then
	begin
		write('FIND 30'#10'')
	end;

	{ 31 }
	i := find('kurevskydlouhytest', 'kydl');
	if i <> 7 then
	begin
		write('FIND 31'#10'')
	end;

	{ 32 }
	str1 := '....tuturututu...';
	i := find(str1, 'rututu');
	if i <> 9 then
	begin
		write('FIND 32'#10'')
	end;

	{ 33 }
	str2 := 'nejaky';
	i := find('jakyne', str2);
	if i <> 0 then
	begin
		write('FIND 33'#10'')
	end
end.



	