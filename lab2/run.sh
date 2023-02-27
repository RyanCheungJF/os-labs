#!/bin/bash

salloc
srun --pty bash
make
./monitor myshell
