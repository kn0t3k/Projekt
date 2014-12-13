#!/bin/sh
exe=project #nazev spustitelneho souboru



#=========================================================
file=func_length1.pas
file_out="$file.out"
file_cor="$file.cor"
#-------------
./$exe $file > $file_out

if ! diff $file_out $file_cor >/dev/null ; then
  echo "CHYBA $file"
fi
#=========================================================
#=========================================================
file=func_copy1.pas
file_out="$file.out"
file_cor="$file.cor"
#-------------
./$exe $file > $file_out

if ! diff $file_out $file_cor >/dev/null ; then
  echo "CHYBA $file"
fi
#=========================================================
#=========================================================
file=func_find1.pas
file_out="$file.out"
file_cor="$file.cor"
#-------------
./$exe $file > $file_out

if ! diff $file_out $file_cor >/dev/null ; then
  echo "CHYBA $file"
fi
#=========================================================
#=========================================================
file=func_sort1.pas
file_out="$file.out"
file_cor="$file.cor"
#-------------
./$exe $file > $file_out

if ! diff $file_out $file_cor >/dev/null ; then
  echo "CHYBA $file"
fi
#=========================================================
#=========================================================
file=readln.pas
file_out="$file.out"
file_cor="$file.cor"
#-------------
cat "$file.input" | ./$exe $file > $file_out

if ! diff $file_out $file_cor >/dev/null ; then
  echo "CHYBA $file"
fi
#=========================================================
#=========================================================
file=readln1.pas
file_out="$file.out"
file_cor="$file.cor"
#-------------
cat "$file.input" | ./$exe $file > $file_out

if ! diff $file_out $file_cor >/dev/null ; then
  echo "CHYBA $file"
fi
#=========================================================
#=========================================================
file=readln2.pas
file_out="$file.out"
file_cor="$file.cor"
#-------------
cat "$file.input" | ./$exe $file > $file_out

if ! diff $file_out $file_cor >/dev/null ; then
  echo "CHYBA $file"
fi
#=========================================================
#=========================================================
file=readln3.pas
file_out="$file.out"
file_cor="$file.cor"
#-------------
cat "$file.input" | ./$exe $file > $file_out

if ! diff $file_out $file_cor >/dev/null ; then
  echo "CHYBA $file"
fi
#=========================================================
#=========================================================
file=block.pas
file_out="$file.out"
file_cor="$file.cor"
#-------------
./$exe $file > $file_out

if ! diff $file_out $file_cor >/dev/null ; then
  echo "CHYBA $file"
fi
#=========================================================
#=========================================================
file=block2.pas
file_out="$file.out"
file_cor="$file.cor"
#-------------
./$exe $file > $file_out

if ! diff $file_out $file_cor >/dev/null ; then
  echo "CHYBA $file"
fi
#=========================================================
#=========================================================
file=bool.pas
file_out="$file.out"
file_cor="$file.cor"
#-------------
./$exe $file > $file_out

if ! diff $file_out $file_cor >/dev/null ; then
  echo "CHYBA $file"
fi
#=========================================================
#=========================================================
file=easy.pas
file_out="$file.out"
file_cor="$file.cor"
#-------------
./$exe $file > $file_out

if ! diff $file_out $file_cor >/dev/null ; then
  echo "CHYBA $file"
fi
#=========================================================
#=========================================================
file=end.pas
file_out="$file.out"
file_cor="$file.cor"
#-------------
./$exe $file > $file_out

if ! diff $file_out $file_cor >/dev/null ; then
  echo "CHYBA $file"
fi
#=========================================================
#=========================================================
file=sort.pas
file_out="$file.out"
file_cor="$file.cor"
#-------------
./$exe $file > $file_out

if ! diff $file_out $file_cor >/dev/null ; then
  echo "CHYBA $file"
fi
#=========================================================
#=========================================================
file=speed.pas
file_out="$file.out"
file_cor="$file.cor"
#-------------
./$exe $file > $file_out

if ! diff $file_out $file_cor >/dev/null ; then
  echo "CHYBA $file"
fi
#=========================================================
#=========================================================
file=test.pas
file_out="$file.out"
file_cor="$file.cor"
#-------------
./$exe $file > $file_out

if ! diff $file_out $file_cor >/dev/null ; then
  echo "CHYBA $file"
fi
#=========================================================
#=========================================================
file=test3.pas
file_out="$file.out"
file_cor="$file.cor"
#-------------
cat "$file.input" | ./$exe $file > $file_out

if ! diff $file_out $file_cor >/dev/null ; then
  echo "CHYBA $file"
fi
#=========================================================
#=========================================================
file=vars.pas
file_out="$file.out"
file_cor="$file.cor"
#-------------
./$exe $file > $file_out

if ! diff $file_out $file_cor >/dev/null ; then
  echo "CHYBA $file"
fi
#=========================================================