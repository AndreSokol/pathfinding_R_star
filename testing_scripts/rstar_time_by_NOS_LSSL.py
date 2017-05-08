#!/usr/bin/env python3

import subprocess, csv

import sys
import re

import plotly
import plotly.graph_objs as go

import pandas as pd

import numpy as np

from copy import deepcopy
from time import time

#ls = ["3839748.xml", "4768079.xml", "4788268-Moscow2.xml", "3620869-WCIII.xml"]
ls = [#"map1_1.xml", "map1_2.xml", "map1_3.xml", "map1_4.xml", "map1_5.xml",
      "map2_1.xml", "map2_2.xml", "map2_3.xml", "map2_4.xml", "map2_5.xml",
      "map3_1.xml", "map3_2.xml", "map3_3.xml", "map3_4.xml", "map3_5.xml",
      "map4_1.xml", "map4_2.xml", "map4_3.xml", "map4_4.xml", "map4_5.xml"]

dist = [#487.6, 464.8, 496.7, 459.7, 469.0,
        488.6, 612.0, 483.5, 490.6, 436.0,
        449.1, 511.3, 474.8, 475.2, 471.9,
        386.9, 363.3, 442.7, 523.3, 364.7]

RUN_NUMBER = len(ls)
REPEAT_TIMES = 1

LSSL_RANGE = list(range(2, 12, 2))
NUMBER_OF_SUCC_VALUE_RANGE = list(range(1, 41, 4))

def gen():
    start_time = time()
    global_test_params = open("global_test_params.txt").read().split()
    print("Generating tests...")
    for i in range(len(ls)):
        I = open("templates/" + ls[i])
        new_map = I.read()
        for p2 in NUMBER_OF_SUCC_VALUE_RANGE:
            for p1 in LSSL_RANGE:
                new_map = re.sub(r"<searchtype>[\w]*</searchtype>",
                "<searchtype>" + global_test_params[0] + "</searchtype>",
                new_map)

                new_map = re.sub(r"<metrictype>[\w]*</metrictype>",
                "<metrictype>" + global_test_params[1] + "</metrictype>",
                new_map)

                new_map = re.sub(r"<hweight>[\w]*</hweight>",
                "<hweight>" + global_test_params[2] + "</hweight>",
                new_map)

                new_map = re.sub(r"<localsearchsteplimit>[\w]*</localsearchsteplimit>",
                "<localsearchsteplimit>" + str(int(dist[i] * 0.28 * p1)) + "</localsearchsteplimit>",
                new_map)

                new_map = re.sub(r"<numberofsuccessors>[\w]*</numberofsuccessors>",
                "<numberofsuccessors>" + str( int(2 * 3.1415 * dist[i] * 0.28 * p2) // 100) + "</numberofsuccessors>",
                new_map)

                new_map = re.sub(r"<distancetosuccessors>[\w]*</distancetosuccessors>",
                "<distancetosuccessors>" + str(int(dist[i] * 0.28)) + "</distancetosuccessors>",
                new_map)

                new_map = re.sub(r"<linecost>[\w]*</linecost>",
                "<linecost>1</linecost>",
                new_map)

                new_map = re.sub(r"<diagonalcost>[\w]*</diagonalcost>",
                "<diagonalcost>1.41421356237</diagonalcost>",
                new_map)

                O = open("tests/test_" + str(i) + "_" + str(p1) + "_" + str(p2) + ".xml", "w")
                O.write(new_map)
                O.close()
    print("Elapsed time:", time() - start_time)


def main():
    start_time = time()

    stats_file = open("stats.csv", "w", newline="", encoding="utf-8", bufferring=1)
    stats = csv.writer(stats_file, delimiter=",")

    stats.writerow([""] + LSSL_RANGE)
    for p2 in NUMBER_OF_SUCC_VALUE_RANGE:
        new_stats_row = [str(p2)]
        for p1 in LSSL_RANGE:
            print("p1", p1, "p2", p2)
            avg_time = 0.0
            for i in range(0, len(ls)):
                #if i == 1:
                #    print("Skipping 2...")
                #    continue
                print(i+1, "of", len(ls))
                print("../testing_scripts/tests/test_" + str(i) + "_" + str(p1) + "_" + str(p2) + ".xml")

                for j in range(REPEAT_TIMES):
                    print(" ", j + 1, "out of", REPEAT_TIMES)
                    try:
                        p = subprocess.run(["../release/PathfindingRStar",
                        "../testing_scripts/tests/test_" + str(i) + "_" + str(p1) + "_" + str(p2) + ".xml"],
                        stdout=subprocess.PIPE,stdin=subprocess.PIPE, timeout=10)
                        out = p.stdout.decode()
                        out = out.split("\n")
                        for s in out:
                            if s[:5] == "time=":
                                print("  ", s)
                                avg_time += float(s[5:])
                                break
                    except subprocess.TimeoutExpired:
                        print("   Too long run (>10s), terminated. Time count as 10s")
                        avg_time += 10.0
            avg_time = avg_time / (len(ls)) / REPEAT_TIMES
            print("result", avg_time)
            print()
            new_stats_row += [avg_time]

        stats.writerow(new_stats_row)
    stats_file.close()
    print("Elapsed time:", time() - start_time)
    return


def plot():
    z_data = pd.read_csv('stats.csv', index_col=0)
    test_stats = open("global_test_params.txt").read().split()

    data = [
        go.Surface(
            z=z_data.as_matrix(),#z_data2
            x=z_data.columns,
            y=z_data.index,
            colorscale=[[0, 'rgb(230,230,230)'], [1, 'rgb(20,20,20)']],
            zmax=0.1,
            zmin=0,
        )
    ]
    layout = go.Layout(
        title='R* heatmap, metrics=' + test_stats[1] + ', hweight=' + test_stats[2] + ', average of ' + test_stats[3],
        xaxis= dict(
            title= 'LSSL',
            #ticklen= 5,
            #dtick=1,
            #ticks='outside',
        ),
        yaxis=dict(
            title= 'Number of successors',
            #ticklen= 5,
            #dtick=1,
            #ticks='outside',
        ),
        scene=dict(
            xaxis= dict(
                title= 'LSSL',
            ),
            yaxis=dict(
                title= 'Number of successors',
            ),
            zaxis=dict(
                title= 'Time',
            ),
        ),
        autosize=True,
        #width=600,
        #height=600,

        #margin=dict(
        #    l=65,
        #    r=50,
        #    b=65,
        #    t=90
        #    )
        )
    fig = go.Figure(data=data, layout=layout)
    plotly.offline.plot(fig, filename='elevations-3d-surface')
    return


def fit():
    z_data = pd.read_csv('stats.csv', index_col=0)
    test_stats = open("global_test_params.txt").read().split()

    z_data_old = deepcopy(z_data)

    x = list(z_data.keys())
    print(*x)
    y = list(z_data[z_data.keys()[0]].keys())
    print(*y)

    for i in range(len(x)):
        args = np.polyfit(z_data[x[i]].keys(), z_data[x[i]], 2)
        for y_1 in y:
            z_data[x[i]][y_1] = args[0] * (y_1 ** 2) + args[1] * (y_1 ** 1) + args[2]# * y_1 + args[3]

    z_data = z_data.transpose()

    x = list(z_data.keys())
    print(*x)
    y = list(z_data[z_data.keys()[0]].keys())
    print(*y)

    for i in range(len(x)):
        args = np.polyfit(list(map(float, z_data[x[i]].keys())), list(z_data[x[i]]), 2)
        for y_1 in y:
            z_data[x[i]][y_1] = args[0] * (int(y_1) ** 2) + args[1] * (int(y_1) ** 1) + args[2]# * int(y_1) + args[3]

    z_data = z_data.transpose()

    data = [
        go.Surface(
            z=z_data.as_matrix(),#z_data2
            x=z_data.columns,
            y=z_data.index,
            colorscale=[[0, 'rgb(230,0,0)'], [1, 'rgb(20,20,20)']],
            zmax=1,
            zmin=0,
            opacity=0.9999,
        ),

        go.Surface(
            z=z_data_old.as_matrix(),#z_data2
            x=z_data_old.columns,
            y=z_data_old.index,
            colorscale=[[0, 'rgb(230,230,230)'], [1, 'rgb(20,20,20)']],
            zmax=1,
            zmin=0,
            opacity=0.9999,
        ),
    ]

    layout = go.Layout(
    title='R* heatmap, fitted, metrics=' + test_stats[1] + ', hweight=' + test_stats[2] + ', average of ' + test_stats[3],
    xaxis= dict(
    title= 'Distance to successors',
    #ticklen= 5,
    #dtick=1,
    #ticks='outside',
    ),
    yaxis=dict(
    title= 'Number of successors',
    #ticklen= 5,
    #dtick=1,
    #ticks='outside',
    ),
    scene=dict(
    xaxis= dict(
    title= 'Distance to successors',
    ),
    yaxis=dict(
    title= 'Number of successors',
    ),
    zaxis=dict(
    title= 'Time',
    range=[0, 0.5]
    ),
    ),
    autosize=True,
    #width=600,
    #height=600,

    #margin=dict(
    #    l=65,
    #    r=50,
    #    b=65,
    #    t=90
    #    )
    )
    fig = go.Figure(data=data, layout=layout)
    plotly.offline.plot(fig, filename='elevations-3d-surface')

    return


if __name__ == '__main__':
    if len(sys.argv) == 1:
        main()
    elif sys.argv[1] == '-plot':
        plot()
    elif sys.argv[1] == '-gen':
        gen()
    elif sys.argv[1] == '-fit':
        fit()
