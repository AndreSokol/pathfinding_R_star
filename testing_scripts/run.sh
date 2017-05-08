#!/bin/bash

#./rjps_time_by_params.py -gen
./rjps_time_by_params.py
git add stats_rjps.csv
git commit -m 'auto run rjpstest 2'
git push origin master
