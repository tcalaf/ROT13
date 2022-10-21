#!/bin/bash

qsub -q ibm-nehalem.q -pe openmpi*16 16 -cwd ./collect.sh
qstat
