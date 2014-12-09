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
	if str2 <> 'testovaci'
	begin
		write('COPY 1'#10'')
	end
	else
	begin
		k := 42
	end;

	{ 2 }
	str1 := 'testovaci text 123456';
	str2 := copy(str1, 16, 6);
	if str2 <> '123456'
	begin
		write('COPY 2'#10'')
	end
	else
	begin
		k := 42
	end;

	{ 3 }
	str1 := 'testovaci text 123456';
	str2 := copy(str1, 10, 2);
	if str2 <> ' t'
	begin
		write('COPY 3'#10'')
	end
	else
	begin
		k := 42
	end;

	{ 4 }
	str1 := '';
	str2 := copy(str1, i, 9);
	if str2 <> ''
	begin
		write('COPY 4'#10'')
	end
	else
	begin
		k := 42
	end;

	{ 5 }
	str1 := '';
	str2 := copy(str1, 1, 1);
	if str2 <> ''
	begin
		write('COPY 5'#10'')
	end
	else
	begin
		k := 42
	end;

	{ 6 }
	str1 := '...........';
	str2 := copy(str1, 9, 900);
	if str2 <> '...'
	begin
		write('COPY 6'#10'')
	end
	else
	begin
		k := 42
	end;

	{ 7 }
	str1 := ' .fd;terqt.erq[lf[eq45454df46 4++++44956 fd*/gdf*sg/-s/g-as/gd 5{}}}';
	str2 := copy(str1, 1, 68);
	if str2 <> ' .fd;terqt.erq[lf[eq45454df46 4++++44956 fd*/gdf*sg/-s/g-as/gd 5{}}}'
	begin
		write('COPY 7'#10'')
	end
	else
	begin
		k := 42
	end;

	{ 8 }
	str1 := ' .fd;terqt.erq[lf[eq45454df46 4++++44956 fd*/gdf*sg/-s/g-as/gd 5{}}}';
	str2 := copy(str1, 1, 67);
	if str2 <> ' .fd;terqt.erq[lf[eq45454df46 4++++44956 fd*/gdf*sg/-s/g-as/gd 5{}}'
	begin
		write('COPY 8'#10'')
	end
	else
	begin
		k := 42
	end;

	{ 9 }
	str1 := '1';
	str2 := copy(str1, 1, 1);
	if str2 <> '1'
	begin
		write('COPY 9'#10'')
	end
	else
	begin
		k := 42
	end;

	{ 10 }
	str1 := '1';
	str2 := copy(str1, 1, 2);
	if str2 <> '1'
	begin
		write('COPY 10'#10'')
	end
	else
	begin
		k := 42
	end;

	{ 11 }
	str2 := copy('aabb', 2, 2);
	if str2 <> 'ab'
	begin
		write('COPY 11'#10'')
	end
	else
	begin
		k := 42
	end;

	{ 12 }
	i := 8;
	str2 := copy('pppppppp', i, 9);
	if str2 <> 'p'
	begin
		write('COPY 12'#10'')
	end
	else
	begin
		k := 42
	end
end.