#!/bin/bash

srun make

LD_LIBRARY_PATH=. ./runner
