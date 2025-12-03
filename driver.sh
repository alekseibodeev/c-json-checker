#!/bin/bash

executable="json-checker"
directory="tests"

if [ -f "$executable" ]; then
    checker=$(realpath "$executable")

    if [ -d "$directory" ]; then
        cd "$directory" || exit

        for file in *; do
            if [ -f "$file" ]; then
                case ${file:0:1} in # get first letter from file name
                    "y" | "i")
                        expected=0
                        ;;
                    "n")
                        expected=1
                        ;;
                esac

                eval "$checker $file"

                if [ $? = "$expected" ]; then
                    echo "PASSED:" "$file"
                else
                    echo "FAILED:" "$file"
                fi
            fi
        done
    else
        echo "ERROR: directory $directory not found."
    fi
else
    echo "ERROR: executable $executable not found."
fi
