#!/bin/bash
for i in ./out/in$1/P$2/*; do diff -q "$i" ./out/in$1/serial.txt; done