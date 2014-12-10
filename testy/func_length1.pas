var str1 : string;
	i : integer;
	k : integer;

begin
	{ return 0 }

	{ 1 }
	str1 := 'text s delkou 16';
	i := length(str1);
	write(i, ''#10''); {16}

	{ 2 }
	str1 := '';
	i := length(str1);
	write(i, ''#10''); {0}

	{ 3 }
	i := length('x'#10'z');
	write(i, ''#10''); {3}

	{ 4 }
	str1 := ''#9''#10'';
	i := length(str1);
	write(i, ''#10''); {2}

	{ 5 }
	i := length('moc dlouhatansky text ;76859230 jhhfehqjfhew...... ,,/frr5w47i83dv12e6 1r56e155    5u6 67 6535613   65t4368**-');
	write(i, ''#10''); {110}

	{ 6 }
	str1 := 'moc dlouhatansky text ;76859230 jhhfehqjfhew...... ,,/frr5w47i83dv12e6 1r56e155    5u6 67 6535613   65t4368**-';
	i := length(str1);
	write(i, ''#10'') {110}
end.