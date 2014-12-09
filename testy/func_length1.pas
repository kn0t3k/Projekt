var str1 : string;
	i : integer;
	k : integer;

begin
	{ return 0 }

	{ 1 }
	str1 := 'text s delkou 16';
	i := length(str1);
	if i <> 16 then
	begin
		write('LENGTH 1'#10'')
	end
	else
	begin
		k := 42
	end;

	{ 2 }
	str1 := '';
	i := length(str1);
	if i <> 0 then
	begin
		write('LENGTH 2'#10'')
	end
	else
	begin
		k := 42
	end;

	{ 3 }
	i := length('x'#10'z');
	if i <> 3 then
	begin
		write('LENGTH 3'#10'')
	end
	else
	begin
		k := 42
	end;

	{ 4 }
	str1 := ''#9''#10'';
	i := length(str1);
	if i <> 2 then
	begin
		write('LENGTH 4'#10'')
	end
	else
	begin
		k := 42
	end;

	{ 5 }
	i := length('moc dlouhatansky text ;76859230 jhhfehqjfhew...... ,,/frr5w47i83dv12e6 1r56e155    5u6 67 6535613   65t4368**-');
	if i <> 110 then
	begin
		write('LENGTH 5'#10'')
	end
	else
	begin
		k := 42
	end;

	{ 6 }
	str1 := 'moc dlouhatansky text ;76859230 jhhfehqjfhew...... ,,/frr5w47i83dv12e6 1r56e155    5u6 67 6535613   65t4368**-';
	i := length(str1);
	if i <> 110 then
	begin
		write('LENGTH 6'#10'')
	end
	else
	begin
		k := 42
	end
end.