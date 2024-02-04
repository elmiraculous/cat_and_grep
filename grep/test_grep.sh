#!/bin/bash
grep="grep "
s21_grep="./s21_grep"
SUCCESS=0
FAIL=0
COUNTER=0

RED="\033[31m"
GREEN="\033[32m"
BLUE="\033[36m"
ENDCOLOR="\033[0m"

grep_c="s21_grep.c"

echo "start" > log.txt

testing(){
    ./s21_grep $var $grep_c> test_s21_grep.log 
    grep $var $grep_c > test_grep.log
    if cmp -s test_s21_grep.log test_grep.log
    then
    (( SUCCESS++ ))
    (( COUNTER++ ))

    echo "${GREEN}Test №$COUNTER SUCCESS${ENDCOLOR} ${BLUE}$s21_grep $var $grep_c ${ENDCOLOR}"
    else
    (( FAIL++ ))
    (( COUNTER++ ))

    echo "${RED}Test №$COUNTER FAIL${ENDCOLOR} ${BLUE}$s21_grep $var $grep_c ${ENDCOLOR}"  >> log.txt
    fi
    rm -f ./test_s21_grep.log ./test_grep.log
}

# проверки с одними флагами без значений
for var1 in v c l i h o n # 7 из 10
do  
    var="0 -$var1"
    testing "$var"
done

# проверки с несколькими флагами без значений
for var1 in v c l i h o n
do  
    for var2 in v c l i h o n
    do  
        if [ $var1 != $var2 ]
        then
            var="0 -$var1 -$var2"
            testing "$var"
        fi
    done
done

# проверки с одними флагами без значений двух файлов
for var1 in v c l i h # 7 из 10
do  
    var="0 -$var1 s21_grep.c"
    testing "$var s21_grep.c"
done

# проверки с несколькими флагами и несколькими файлами
for var1 in v c l i h o n
do  
    for var2 in v c l i h o n
    do  
        if [ $var1 != $var2 ]
        then
            var="0 -$var1 s21_grep.c -$var2 s21_grep.c"
            testing "$var"
        fi
    done
done

# проверки для флага s который глушит сообщения об ошибках с файлом
for var1 in s21_grep.c grep.h gre.b grep.tlsdf s21  # 8 из 10
do  
    var="A -s $var1"
    testing "$var"
done

# проверка на е флаг который задает паттерны 9 из 10
for var1 in " '[0-9]'" " '[a-z]'" "'a'" " '[0-9].*'"
do  
    var="-e$var1"
    testing "$var"
done

# проверка на f флаг который задает паттерны
for var1 in "1.txt" "2.txt"
do  
    var="-f $var1"
    testing "$var"
done

# проверка на f флаг который задает паттерны
for var1 in "1.txt" "2.txt"
do
    for var2 in "3.txt" "4.txt"    
    do  
        var="-f $var1 -f $var2"
        testing "$var"
    done
done

# проверка на е флаг который задает паттерны
for var1 in " '[0-9]'" " '[a-z]'" "'a'" " '[0-9].*'"
do
    for var2 in " 1" " '[a-x]'" "'vo'" "'.*ap.*'"    
    do  
        var="-e$var1 -e$var2"
        testing "$var"
    done
done

echo "${GREEN}SUCCESS $SUCCESS ${ENDCOLOR} | ${RED} FAILED $FAIL${ENDCOLOR}";