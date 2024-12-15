#!/bin/bash

function make_checker {
    make -C checker checker
}

function init {
    total_score=0
    pad=$(printf '%.1s' "."{1..70})
    padlength=71

    TESTS_OUT=tests-out
    TESTS=tests
    NUM_TASKS_7_TO_10=0

    # Cleanup the previous run's temporary files
    rm -rf ${TESTS_OUT}

    make_checker
}

function print_result {
    printf "%s" "$1"
    printf "%*.*s" 0 $((padlength - ${#1} - ${#2} )) "$pad"
    printf "%s\n" "$2"
}

function check_homework {
    test_score=$2
    task_str=$1
    task_num=${task_str:4}

    echo "................................TASK $task_num................................."

    mkdir -p ${TESTS_OUT}/$1

    for (( i = 0; i < $3; ++i))
    do
        in_file="${TESTS}/input/$1/$i.in"
        ref_file="${TESTS}/ref_output/$1/$i.ref"
        params_file="${TESTS}/params/$1/$i.param"
        out_file="${TESTS_OUT}/$1/$i.out"

        test_id=$i

        build/check $task_num ${params_file} < ${in_file} > ${out_file}

        if [ $task_num -eq 10 ]
        then
            diff ${out_file} ${ref_file} > /dev/null
        else
            diff --ignore-all-space --ignore-trailing-space \
                ${out_file} ${ref_file} > /dev/null
        fi
        
        if [ $? -eq 0 ]
        then
            print_result "Test ${test_id}" "${test_score}/${test_score}p passed"
            let "total_score += $test_score"

            if [ $task_num -ge 7 ] && [ $task_num -le 10 ]
            then
                let "NUM_TASKS_7_TO_10++"
            fi
        else
            print_result "Test ${test_id}" "0/${test_score}p failed"
        fi
    done

    echo
}

function check_valgrind {
    echo "..............................TASK 11..............................."

    if [ $NUM_TASKS_7_TO_10 -lt 20 ]
    then
        echo "Skipping Valgrind check - you must first have maximum score for tasks 7 to 10."
        return
    fi

    target_tasks=(7 7 8 8 9 9 9 10 10 10)
    target_tests=(2 3 2 3 2 3 4  2  3  4)

    for (( i = 0; i < 10; ++i))
    do
        task_number=${target_tasks[$i]}
        test_number=${target_tests[$i]}

        in_file="${TESTS}/input/task${task_number}/${test_number}.in"
        params_file="${TESTS}/params/task${task_number}/${test_number}.param"

        test_score=$1

        valgrind --tool=memcheck --leak-check=full --error-exitcode=1 build/check ${task_number} ${params_file} < ${in_file} > /dev/null 2>&1

        if [ $? -eq 0 ]
        then
            let "total_score += 1"
            print_result "Test ${i}" "${test_score}/${test_score}p passed"
        else
            print_result "Test ${i}" "0/${test_score}p failed"
        fi
    done
}

function check_readme {
    echo -e "\n..............................TASK README.............................."

    ls . | grep -i "readme" &>/dev/null
    if [ $? -eq 0 ]; then
        print_result "Test README" "$1/$1p passed"
        let "total_score += $1"
    else
        print_result "Test README" "0/$1p failed"
    fi
}

function check_style {
    echo -e "\n..............................CODING STYLE............................."
    is_style_ok=1
    result_temp=$(mktemp)
    echo -e "\nRun cpplint\n" > "$result_temp"

    if [ -d .venv ]; then
        source .venv/bin/activate
    fi

    cpplint --filter=-legal/copyright,-readability/casting,-build/include_subdir,-runtime/threadsafe_fn,-build/header_guard,-runtime/int \
        --linelength=120 $( find ./libchunk/ -name \*.c ) &>> "$result_temp"

    if [ ! $? -eq 0 ]; then
        is_style_ok=0
    fi

    if [ -d .venv ]; then
        deactivate
    fi

    echo -e "\nRun clang-tidy\n" >> "$result_temp"
    clang-tidy -checks='-*,cppcoreguidelines*,-cppcoreguidelines-macro-to-enum' --warnings-as-errors='-*,cppcoreguidelines*,-cppcoreguidelines-macro-to-enum' $( find ./libchunk -name \*.h -or -name \*.c ) -- &>> "$result_temp"

    if [ ! $? -eq 0 ]; then
        is_style_ok=0
    fi

    if [ $is_style_ok -eq 0 ]; then
        print_result "Test Coding Style" "-20/0p failed"
        let "total_score -= 20"
    else
        print_result "Test Coding Style" "0/0p passed"
    fi

    # Display run summary
    awk -f checker/parse.awk "$result_temp"

    # Display detailed report
    echo -e "\n================================"
    echo -e "= Detailed coding style report ="
    echo -e "================================\n"
    cat "$result_temp"
    echo " "
    echo " "

    rm "$result_temp"
}

function run_checker {
    init
    echo
	check_homework task1 1 4 # 1 pct, 4 tests
	check_homework task2 2 3 # 2 pct, 3 tests
    check_homework task3 2 3 # 2 pct, 3 tests
    check_homework task4 2 3 # 2 pct, 3 tests
    check_homework task5 2 4 # 2 pct, 4 tests
    check_homework task6 2 5 # 2 pct, 5 tests
    check_homework task7 2 4 # 2 pct, 4 tests
    check_homework task8 2 6 # 2 pct, 6 tests
    check_homework task9 2 5 # 2 pct, 5 tests
    check_homework task10 2 5 # 2 pct, 5 tests

    check_valgrind 1 # 1 pct each test (10 tests total)
    check_readme 10 # 10 pct
    check_style

    awk '{
        total_score = $1
        printf("Final score.....................................................%.02f/100\n",total_score)
    }' <<< "$total_score"
}

TIMEOUT_VAL=1

export -f run_checker init check_homework check_readme check_style print_result
run_checker

if [ ! $? -eq 0 ]
then
    echo "TIMEOUT. Tests exceeded maximum allowed time of $TIMEOUT_VAL"
fi
