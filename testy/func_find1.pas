var str1 : string;
	str2 : string;
	i : integer;
	k : integer;

begin
	{ return 0 }

	{ 1 }
	str1 := 'nejaky kratky text';
	str2 := 'nejaky';
	i := find(str1, str2);
	write(i, ''#10''); {1}

	{ 2 }
	str2 := 'kratky';
	i := find(str1, str2);
	write(i, ''#10''); {8}

	{ 3 }
	str2 := 'text';
	i := find(str1, str2);
	write(i, ''#10''); {15}

	{ 4 }
	str2 := 'ne';
	i := find(str1, str2);
	write(i, ''#10''); {1}

	{ 5 }
	str2 := 'y';
	i := find(str1, str2);
	write(i, ''#10''); {6}

	{ 6 }
	str2 := 'ext';
	i := find(str1, str2);
	write(i, ''#10''); {16}

	{ 7 }
	str2 := 'xt';
	i := find(str1, str2);
	write(i, ''#10''); {17}

	{ 8 }
	str2 := 'ky kratky';
	i := find(str1, str2);
	write(i, ''#10''); {5}

	{ 9 }
	str2 := '';
	i := find(str1, str2);
	write(i, ''#10''); {1}

	{ 10 }
	str2 := 'baba';
	i := find(str1, str2);
	write(i, ''#10''); {0}

	{ 11 }
	str2 := 'alabama fdjk kljklw jkwjk';
	i := find(str1, str2);
	write(i, ''#10''); {0}

	{ 12 }
	str2 := 'nejaky kratky text';
	i := find(str1, str2);
	write(i, ''#10''); {1}

	{ 13 }
	str1 := '';
	str2 := 'aaa';
	i := find(str1, str2);
	write(i, ''#10''); {0}

	{ 14 }
	str2 := '';
	i := find(str1, str2);
	write(i, ''#10''); {1}

	{ 15 }
	str1 := 'b';
	str2 := '';
	i := find(str1, str2);
	write(i, ''#10''); {1}

	{ 16 }
	str2 := 'b';
	i := find(str1, str2);
	write(i, ''#10''); {1}

	{ 17 }
	str2 := 'karas';
	i := find(str1, str2);
	write(i, ''#10''); {0}

	{ 18 }
	str1 := 'ahoj ja jsem borec456 a nakopu ti prdel ty demente!';
	str2 := '';
	i := find(str1, str2);
	write(i, ''#10''); {1}

	{ 19 }
	str2 := 'ahoj';
	i := find(str1, str2);
	write(i, ''#10''); {1}

	{ 20 }
	str2 := '456';
	i := find(str1, str2);
	write(i, ''#10''); {19}

	{ 21 }
	str2 := 'prdel';
	i := find(str1, str2);
	write(i, ''#10''); {35}

	{ 22 }
	str2 := '!';
	i := find(str1, str2);
	write(i, ''#10''); {51}

	{ 23 }
	str1 := '54f45  fsd;fsf;wetweqteeer   4    t4r5we464t ;][  ;  ee ;ewwww  ;tew;trey54 45675y45645445645   y4155241561545   52112 ; ;; ';
	str2 := '5211';
	i := find(str1, str2);
	write(i, ''#10''); {114}

	{ 24 }
	str2 := '54f45  ttt';
	i := find(str1, str2);
	write(i, ''#10''); {0}

	{ 25 }
	str2 := 'x4f45  fsd';
	i := find(str1, str2);
	write(i, ''#10''); {0}

	{ 26 }
	str2 := '   4    t4r5we464t ;][  ;  ee ;ewwww  ;tew;trey54 45675y45645445645   y4155241561545   52112 ; ;; ';
	i := find(str1, str2);
	write(i, ''#10''); {27}

	{ 27 }
	str2 := '   y';
	i := find(str1, str2);
	write(i, ''#10''); {94}

	{ 28 }
	str2 := '4    ';
	i := find(str1, str2);
	write(i, ''#10''); {30}

	{ 29 }
	str2 := '   4';
	i := find(str1, str2);
	write(i, ''#10''); {27}

	{ 30 }
	str2 := '54f45  fsd;fsf;wetweqteeer   4    t4r5we464t ;][  ;  ee ;ewwww  ;tew;trey54 45675y45645445645   y4155241561545   52112 ; ;; ';
	i := find(str1, str2);
	write(i, ''#10''); {1}

	{ 31 }
	i := find('kurevskydlouhytest', 'kydl');
	write(i, ''#10''); {7}

	{ 32 }
	str1 := '....tuturututu...';
	i := find(str1, 'rututu');
	write(i, ''#10''); {9}

	{ 33 }
	str2 := 'nejaky';
	i := find('jakyne', str2);
	write(i, ''#10'') {0}
end.



	