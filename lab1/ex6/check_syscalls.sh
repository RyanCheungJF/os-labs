#!/bin/bash

####################
# Lab 1 Exercise 6
# Name: Ryan Cheung Jing Feng
# Student No: A0217587R
# Lab Group: 18
####################

echo "Printing system call report"

# Compile file
gcc -std=c99 pid_checker.c -o ex6

# Use strace to get report
strace -c ./ex6
