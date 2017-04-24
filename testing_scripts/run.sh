#!/bin/bash

./rjps_time_by_params.py -gen
./rjps_time_by_params.py
git add .
git commit -m 'auto test run'
git push origin master

