#!/bin/bash

for i in {0..20} ; do
    ./run_test 0.4s 16 examples_build/send_recv

done