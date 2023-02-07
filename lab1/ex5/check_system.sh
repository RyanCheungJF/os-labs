#!/bin/bash

####################
# Lab 1 Exercise 5
# Name: Ryan Cheung Jing Feng
# Student No: A0217587R
# Lab Group: 18
####################

# Fill the below up
hostname=$(hostname)
# prints system info, -m flag for machine hardware name
machine_hardware=$(uname -m)
# cat concat files to std output
max_process_id=$(cat /proc/sys/kernel/pid_max)
# ps shows the processes, -u specifies user, wc -l wordcount line turns the output into lines
user_process_count=$(ps -u $USER | wc -l)
# ps -eo user gets security info, uniq -c removes repeated lines and adds a prefix/ counter (prefix, name), 
# sort -n sorts according to numerical value, tail -1 gets the last entry, awk $2 to filter out the name of the process
user_with_most_processes=$(ps -eo user | sort | uniq -c | sort -n | tail -1 | awk '{print $2}')
# free shows free, physical and swap memory, grep filters for word Mem, awk can do computations, 
# in this case it takes 4th param, free memory and 2nd param, total memory to calculate the percentage
mem_free_percentage=$(free | grep Mem | awk '{print$4/$2 * 100.0}')

echo "Hostname: $hostname"
echo "Machine Hardware: $machine_hardware"
echo "Max Process ID: $max_process_id"
echo "User Processes: $user_process_count"
echo "User With Most Processes: $user_with_most_processes"
echo "Memory Free (%): $mem_free_percentage"
