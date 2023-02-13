#!/bin/bash

####################
# Lab 1 Exercise 7
# Name: Ryan Cheung Jing Feng
# Student No: A0217587R
# Lab Group: 18
####################

####################
# Strings that you may need for prompt (not in order)
####################
# Enter $N numbers:
# Enter NEW prefix (only alphabets):
# Enter prefix (only alphabets):
# INVALID
# Number of files to create:
# Please enter a valid number [0-9]:
# Please enter a valid prefix [a-z A-Z]:

#################### Steps ####################

# Fill in the code to request user for the prefix
read -p $'Enter prefix (only alphabets):\n' prefix

# Check the validity of the prefix #
if ! [[ $prefix =~ ^[A-Za-z]+$ ]]
then
    echo "INVALID"
    # Fill in the code to request user for the new prefix
    read -p $'Please enter a valid prefix [a-z A-Z]:\n' prefix
fi

# Enter numbers and create files #
read -p $'Number of files to create:\n' N

echo Enter "$N" numbers:
for (( i=1; i<=$N; i++ ))
do
    read num
    if ! [[ $num =~ ^[0-9]+$ ]]
    then
        echo "INVALID"
        # Fill in the code to request user for the new prefix
        read -p $'Please enter a valid number [0-9]:\n' num
    fi
    # Create variables
    # eval "n$i=$num"
    touch "${prefix}_${num}.txt"
done

echo ""
echo "Files Created"
ls *.txt
echo ""

# Renaming to new prefix #
read -p $'Enter NEW prefix (only alphabets):\n' newPrefix

# Check the validity of the prefix #
if ! [[ $newPrefix =~ ^[A-Za-z]+$ ]]
then
    echo "INVALID"
    # Fill in the code to request user for the new prefix
    read -p $'Please enter a valid prefix [a-z A-Z]:\n' newPrefix
fi

for f in ${prefix}_*.txt
do
    mv "$f" "${f/$prefix/$newPrefix}"
done

echo ""
echo "Files Renamed"
ls *.txt
echo ""