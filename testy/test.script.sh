#!/bin/sh
exe=project #nazev spustitelneho souboru



#=========================================================
file=test4.pas
result=0
#-------------
./$exe $file >/dev/null
realresult=$?
if [ $realresult -ne $result ]; then
         echo -e "  \e[0;31m--$file--\e[0;00m      vystup $realresult   ocekavany $result"
fi
#=========================================================
#=========================================================
file=app.pas
result=0
#-------------
./$exe $file >/dev/null
realresult=$?
if [ $realresult -ne $result ]; then
         echo -e "  \e[0;31m--$file--\e[0;00m      vystup $realresult   ocekavany $result"
fi
#=========================================================
#=========================================================
file=block.pas
result=0
#-------------
./$exe $file >/dev/null
realresult=$?
if [ $realresult -ne $result ]; then
         echo -e "  \e[0;31m--$file--\e[0;00m      vystup $realresult   ocekavany $result"
fi
#=========================================================
#=========================================================
file=block2.pas
result=0
#-------------
./$exe $file >/dev/null
realresult=$?
if [ $realresult -ne $result ]; then
         echo -e "  \e[0;31m--$file--\e[0;00m      vystup $realresult   ocekavany $result"
fi
#=========================================================
#=========================================================
file=bool.pas
result=0
#-------------
./$exe $file >/dev/null
realresult=$?
if [ $realresult -ne $result ]; then
         echo -e "  \e[0;31m--$file--\e[0;00m      vystup $realresult   ocekavany $result"
fi
#=========================================================
#=========================================================
file=div-zero.pas
result=8
#-------------
./$exe $file >/dev/null
realresult=$?
if [ $realresult -ne $result ]; then
         echo -e "  \e[0;31m--$file--\e[0;00m      vystup $realresult   ocekavany $result"
fi
#=========================================================
#=========================================================
file=easy.pas
result=0
#-------------
./$exe $file >/dev/null
realresult=$?
if [ $realresult -ne $result ]; then
         echo -e "  \e[0;31m--$file--\e[0;00m      vystup $realresult   ocekavany $result"
fi
#=========================================================
#=========================================================
file=minus.pas
result=0
#-------------
./$exe $file >/dev/null
realresult=$?
if [ $realresult -ne $result ]; then
         echo -e "  \e[0;31m--$file--\e[0;00m      vystup $realresult   ocekavany $result"
fi
#=========================================================
#=========================================================
file=only-if.pas
result=2
#-------------
./$exe $file >/dev/null
realresult=$?
if [ $realresult -ne $result ]; then
         echo -e "  \e[0;31m--$file--\e[0;00m      vystup $realresult   ocekavany $result"
fi
#=========================================================
#=========================================================
file=readln.pas
result=0
-------------
./$exe $file >/dev/null
realresult=$?
if [ $realresult -ne $result ]; then
         echo -e "  \e[0;31m--$file--\e[0;00m      vystup $realresult   ocekavany $result"
fi
#=========================================================
#=========================================================
file=sort.pas
result=0
#-------------
./$exe $file >/dev/null
realresult=$?
if [ $realresult -ne $result ]; then
         echo -e "  \e[0;31m--$file--\e[0;00m      vystup $realresult   ocekavany $result"
fi
#=========================================================
#=========================================================
file=speed.pas
result=0
#-------------
./$exe $file >/dev/null
realresult=$?
if [ $realresult -ne $result ]; then
         echo -e "  \e[0;31m--$file--\e[0;00m      vystup $realresult   ocekavany $result"
fi
#=========================================================
#=========================================================
file=test.pas
result=0
#-------------
./$exe $file >/dev/null
realresult=$?
if [ $realresult -ne $result ]; then
         echo -e "  \e[0;31m--$file--\e[0;00m      vystup $realresult   ocekavany $result"
fi
#=========================================================
#=========================================================
file=test1.pas
result=0
#-------------
./$exe $file >/dev/null
realresult=$?
if [ $realresult -ne $result ]; then
         echo -e "  \e[0;31m--$file--\e[0;00m      vystup $realresult   ocekavany $result"
fi
#=========================================================
#=========================================================
file=test2.pas
result=0
#-------------
./$exe $file >/dev/null
realresult=$?
if [ $realresult -ne $result ]; then
         echo -e "  \e[0;31m--$file--\e[0;00m      vystup $realresult   ocekavany $result"
fi
#=========================================================
#=========================================================
file=test3.pas
result=0
#-------------
./$exe $file >/dev/null
realresult=$?
if [ $realresult -ne $result ]; then
         echo -e "  \e[0;31m--$file--\e[0;00m      vystup $realresult   ocekavany $result"
fi
#=========================================================
#=========================================================
file=testD1.pas
result=3
#-------------
./$exe $file >/dev/null
realresult=$?
if [ $realresult -ne $result ]; then
         echo -e "  \e[0;31m--$file--\e[0;00m      vystup $realresult   ocekavany $result"
fi
#=========================================================
#=========================================================
file=func_length1.pas
result=0
#-------------
./$exe $file >/dev/null
realresult=$?
if [ $realresult -ne $result ]; then
         echo -e "  \e[0;31m--$file--\e[0;00m      vystup $realresult   ocekavany $result"
fi
#=========================================================
#=========================================================
file=func_length2.pas
result=3
#-------------
./$exe $file >/dev/null
realresult=$?
if [ $realresult -ne $result ]; then
         echo -e "  \e[0;31m--$file--\e[0;00m      vystup $realresult   ocekavany $result"
fi
#=========================================================
#=========================================================
file=func_length3.pas
result=4
#-------------
./$exe $file >/dev/null
realresult=$?
if [ $realresult -ne $result ]; then
         echo -e "  \e[0;31m--$file--\e[0;00m      vystup $realresult   ocekavany $result"
fi
#=========================================================
#=========================================================
file=func_copy1.pas
result=0
#-------------
./$exe $file >/dev/null
realresult=$?
if [ $realresult -ne $result ]; then
         echo -e "  \e[0;31m--$file--\e[0;00m      vystup $realresult   ocekavany $result"
fi
#=========================================================
#=========================================================
file=func_copy2.pas
result=3
#-------------
./$exe $file >/dev/null
realresult=$?
if [ $realresult -ne $result ]; then
         echo -e "  \e[0;31m--$file--\e[0;00m      vystup $realresult   ocekavany $result"
fi
#=========================================================
#=========================================================
file=func_copy3.pas
result=4
#-------------
./$exe $file >/dev/null
realresult=$?
if [ $realresult -ne $result ]; then
         echo -e "  \e[0;31m--$file--\e[0;00m      vystup $realresult   ocekavany $result"
fi
#=========================================================
#=========================================================
file=func_sort1.pas
result=0
#-------------
./$exe $file >/dev/null
realresult=$?
if [ $realresult -ne $result ]; then
         echo -e "  \e[0;31m--$file--\e[0;00m      vystup $realresult   ocekavany $result"
fi
#=========================================================
#=========================================================
file=func_sort2.pas
result=3
#-------------
./$exe $file >/dev/null
realresult=$?
if [ $realresult -ne $result ]; then
         echo -e "  \e[0;31m--$file--\e[0;00m      vystup $realresult   ocekavany $result"
fi
#=========================================================
#=========================================================
file=func_sort3.pas
result=4
#-------------
./$exe $file >/dev/null
realresult=$?
if [ $realresult -ne $result ]; then
         echo -e "  \e[0;31m--$file--\e[0;00m      vystup $realresult   ocekavany $result"
fi
#=========================================================
#=========================================================
file=func_find1.pas
result=0
#-------------
./$exe $file >/dev/null
realresult=$?
if [ $realresult -ne $result ]; then
         echo -e "  \e[0;31m--$file--\e[0;00m      vystup $realresult   ocekavany $result"
fi
#=========================================================
#=========================================================
file=func_find2.pas
result=3
#-------------
./$exe $file >/dev/null
realresult=$?
if [ $realresult -ne $result ]; then
         echo -e "  \e[0;31m--$file--\e[0;00m      vystup $realresult   ocekavany $result"
fi
#=========================================================
#=========================================================
file=func_find3.pas
result=4
#-------------
./$exe $file >/dev/null
realresult=$?
if [ $realresult -ne $result ]; then
         echo -e "  \e[0;31m--$file--\e[0;00m      vystup $realresult   ocekavany $result"
fi
#=========================================================
