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
