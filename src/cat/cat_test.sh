#!/bin/bash

test_files=("./tests/test_1_cat.txt" "./tests/test_2_cat.txt" "./tests/test_3_cat.txt")
opts=(-b -e -n -s -t -v)
long_opts=(--number-nonblank --number --squeeze-blank -E -T)

is_file_exist=0

if ! test -f s21_cat; then
    make s21_cat
    make clean
    $is_file_exist=1
fi

for test_file in ${test_files[@]}; do
    echo TESTING FILE $test_file
    #testing one option
    echo TESTING ONE OPTIONS
    for options in ${opts[@]}; do
        cat $options $test_file >cat_unix
        ./s21_cat $options $test_file >cat_my
        if cmp -s cat_unix cat_my; then
            echo SUCCESS
        else
            echo ERROR cat $options $test_file
        fi
    done
    #tesing long options
    echo TESTING LONG OPTIONS
    for options in ${long_opts[@]}; do
        cat $options $test_file >cat_unix
        ./s21_cat $options $test_file >cat_my
        if cmp -s cat_unix cat_my; then
            echo SUCCESS
        else
            echo ERROR cat $options $test_file
        fi
    done
    echo TESTING TWO OPTIONS
    for options1 in ${opts[@]}; do
        for options2 in ${opts[@]}; do
            if [[ $options1 != $options2 ]]; then
                cat $options1 $options2 $test_file >cat_unix
                ./s21_cat $options1 $options2 $test_file >cat_my
                if cmp -s cat_unix cat_my; then
                    echo SUCCESS
                else
                    echo ERROR cat $options1 $options2 $test_file
                fi
            fi
        done
    done

    echo TESTING THREE OPTIONS
    for options1 in ${opts[@]}; do
        for options2 in ${opts[@]}; do
            for options3 in ${opts[@]}; do
                if [[ $options1 != $options2 ]] && [[ $options2 != $options3 ]]; then
                    cat $options1 $options2 $options3 $test_file >cat_unix
                    ./s21_cat $options1 $options2 $options3 $test_file >cat_my
                    if cmp -s cat_unix cat_my; then
                        echo SUCCESS
                    else
                        echo ERROR cat $options1 $options2 $options3 $test_file
                    fi
                fi
            done
        done
    done
    echo MEMORY TEST
    for options in ${opts[@]}; do
        valgrind --tool=memcheck --leak-check=yes --show-leak-kinds=all --track-origins=yes ./s21_cat $options $test_file >valgrind_log.txt 2>&1
        if grep -q "ERROR SUMMARY: 0 errors" valgrind_log.txt; then
            echo SUCCESS
        else
            echo ERROR cat $options $test_file
        fi
    done

done

rm cat_unix
rm cat_my
rm valgrind_log.txt

if [[ $is_file_exist -eq 1 ]]; then
    rm s21_cat
    rm *.o

fi
