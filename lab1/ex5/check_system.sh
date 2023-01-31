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
#
max_process_id=$(cat /proc/sys/kernel/pid_max)
# ps shows the processes, -u specifies user, wc -l wordcount line turns the output into lines
user_process_count=$(ps -u $USER | wc -l)
#
user_with_most_processes=$(ps -eo user|sort|uniq -c|sort -n|tail -1|awk '{print $2}')
#
mem_free_percentage=$(free | grep Mem | awk '{print$4/$2 * 100.0}')

echo "Hostname: $hostname"
echo "Machine Hardware: $machine_hardware"
echo "Max Process ID: $max_process_id"
echo "User Processes: $user_process_count"
echo "User With Most Processes: $user_with_most_processes"
echo "Memory Free (%): $mem_free_percentage"
