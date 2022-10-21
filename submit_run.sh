#!/bin/bash

qsub -q ibm-nehalem.q -pe openmpi 16 -cwd ./run.sh
qstat
