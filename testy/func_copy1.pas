var str1 : string;
	str2 : string;
	i : integer;
	k : integer;

begin
	{ return 0 }

	{ 1 }
	i := 1;
	str1 := 'testovaci text 123456';
	str2 := copy(str1, i, 9);
	write(str2, ''#10''); {'testovaci'}

	{ 2 }
	str1 := 'testovaci text 123456';
	str2 := copy(str1, 16, 6);
	write(str2, ''#10''); {'123456'}

	{ 3 }
	str1 := 'testovaci text 123456';
	str2 := copy(str1, 10, 2);
	write(str2, ''#10''); {' t'}

	{ 4 }
	str1 := '';
	str2 := copy(str1, i, 9);
	write(str2, ''#10''); {''}

	{ 5 }
	str1 := '';
	str2 := copy(str1, 1, 1);
	write(str2, ''#10''); {''}

	{ 6 }
	str1 := '...........';
	str2 := copy(str1, 9, 900);
	write(str2, ''#10''); {'...'}

	{ 7 }
	str1 := ' .fd;terqt.erq[lf[eq45454df46 4++++44956 fd*/gdf*sg/-s/g-as/gd 5{}}}';
	str2 := copy(str1, 1, 68);
	write(str2, ''#10''); {str1}

	{ 8 }
	str1 := ' .fd;terqt.erq[lf[eq45454df46 4++++44956 fd*/gdf*sg/-s/g-as/gd 5{}}}';
	str2 := copy(str1, 1, 67);
	write(str2, ''#10''); {str1 - posledni znak}

	{ 9 }
	str1 := '1';
	str2 := copy(str1, 1, 1);
	write(str2, ''#10''); {'1'}

	{ 10 }
	str1 := '1';
	str2 := copy(str1, 1, 2);
	write(str2, ''#10''); {'1'}

	{ 11 }
	str2 := copy('aabb', 2, 2);
	write(str2, ''#10''); {'ab'}

	{ 12 }
	i := 8;
	str2 := copy('pppppppp', i, 9);
	write(str2, ''#10''); {'p'}

	{ 13 }
	str1 := 'ahoj toto mi kopiruj';
	str1 := copy(str1, 1, 9);
	write(str1, ''#10'') {'ahoj toto'}
end.