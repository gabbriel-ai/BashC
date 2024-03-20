#!/bin/bash

test_files=("./tests/test_1_grep.txt" "./tests/test_2_grep.txt" "./tests/test_3_grep.txt")
opts=(-i -v -c -l -n -h -s -o)

patterns=(test ipsum dolor a line)

pattern_file="./tests/patterns"

is_file_exist=0

if ! test -f s21_grep; then
    make s21_grep
    make clean
    $is_file_exist=1
fi

for test_file in ${test_files[@]}; do
    echo TESTING FILE $test_file
    #testing one option
    echo TESTING ONE OPTIONS
    for options in ${opts[@]}; do
        for patterns in ${patterns[@]}; do
            grep $options $patterns $test_file >grep_unix
            ./s21_grep $options $patterns $test_file >grep_my
            if cmp -s grep_unix grep_my; then
                echo SUCCESS
            else
                echo ERROR grep $options $patterns $test_file
            fi
        done
    done

    #testing two options
    echo TESTING TWO OPTIONS
    for options1 in ${opts[@]}; do
        for options2 in ${opts[@]}; do
            if [[ $options1 != $options2 ]]; then
                for patterns in ${patterns[@]}; do
                    grep $options1 $options2 $patterns $test_file >grep_unix
                    ./s21_grep $options1 $options2 $patterns $test_file >grep_my
                    if cmp -s grep_unix grep_my; then
                        echo SUCCESS
                    else
                        echo ERROR grep $options1 $options2 $patterns $test_file
                    fi
                done
            fi
        done
    done

    #testing three options
    echo TESTING THREE OPTIONS
    for options1 in ${opts[@]}; do
        for options2 in ${opts[@]}; do
            for options3 in ${opts[@]}; do
                if [[ $options1 != $options2 ]] && [[ $options2 != $options3 ]]; then
                    for patterns in ${patterns[@]}; do
                        grep $options1 $options2 $patterns $test_file >grep_unix
                        ./s21_grep $options1 $options2 $patterns $test_file >grep_my
                        if cmp -s grep_unix grep_my; then
                            echo SUCCESS
                        else
                            echo ERROR grep $options1 $options2 $patterns $test_file
                        fi
                    done
                fi
            done
        done
    done

    #testing -f with one:
    echo TESTING -f WITH ONE OPTIONS
    for options in ${opts[@]}; do
        grep $options -f $pattern_file $test_file >grep_unix
        ./s21_grep $options -f $pattern_file $test_file >grep_my
        if cmp -s grep_unix grep_my; then
            echo SUCCESS
        else
            echo ERROR grep $options -f $pattern_file $test_file
        fi
    done
    #testing -f with two:
    echo TESTING -f WITH two OPTIONS
    for options1 in ${opts[@]}; do
        for options2 in ${opts[@]}; do
            if [[ $options1 != $options2 ]]; then
                grep $options1 $options2 -f $pattern_file $test_file >grep_unix
                ./s21_grep $options1 $options2 -f $pattern_file $test_file >grep_my
                if cmp -s grep_unix grep_my; then
                    echo SUCCESS
                else
                    echo ERROR grep $options1 $options2 -f $pattern_file $test_file
                fi
            fi
        done
    done

    #testing -e with one:

    echo TESTING -e WITH one OPTIONS
    for options in ${opts[@]}; do
        grep $options -e . $test_file >grep_unix
        ./s21_grep $options -e . $test_file >grep_my
        if cmp -s grep_unix grep_my; then
            echo SUCCESS
        else
            echo ERROR grep $options -e . $test_file
        fi
    done
    #testing -e with two:
    echo TESTING -e WITH two OPTIONS
    for options2 in ${opts[@]}; do
        if [[ $options1 != $options2 ]]; then
            grep $options1 $options2 -e test $test_file >grep_unix
            ./s21_grep $options1 $options2 -e test $test_file >grep_my
            if cmp -s grep_unix grep_my; then
                echo SUCCESS
            else
                echo ERROR grep $options1 $options2 -e test $test_file
            fi
        fi
    done
    #testing double -e with one:
    echo TESTING DOUBLE -e with one options
    for options in ${opts[@]}; do
        grep $options -e test -e grep $test_file >grep_unix
        ./s21_grep $options -e test -e grep $test_file >grep_my
        if cmp -s grep_unix grep_my; then
            echo SUCCESS
        else
            echo ERROR grep $options -e test -e grep $test_file
        fi
    done

    #testing memory test:

    echo MEMORY TEST
    for options in ${opts[@]}; do
        for patterns in ${patterns[@]}; do
            valgrind --tool=memcheck --leak-check=yes --show-leak-kinds=all --track-origins=yes ./s21_grep $options $patterns $test_file >valgrind_log.txt 2>&1
            if grep -q "ERROR SUMMARY: 0 errors" valgrind_log.txt; then
                echo SUCCESS
            else
                echo ERROR grep $options $patterns $test_file
            fi
        done
    done
done

rm valgrind_log.txt
rm grep_my
rm grep_unix
if [[ $is_file_exist -eq 1 ]]; then
    rm s21_grep
    rm *.o
fi
