import subprocess, csv

import sys
import re

import plotly
import plotly.graph_objs as go

import pandas as pd

import numpy as np

from copy import deepcopy

ls = ["3839748.xml", "4768079.xml", "4788268-Moscow2.xml", "3620869-WCIII.xml"]
RUN_NUMBER = 4
REPEAT_TIMES = 4

DIST_TO_SUCC_VALUE_RANGE = list(range(10, 151, 10))
NUMBER_OF_SUCC_VALUE_RANGE = list(range(10, 101, 10))

def gen():
    global_test_params = open("global_test_params.txt").read().split()
    print("Generating tests...")
    for i in range(RUN_NUMBER):
        I = open("../resources/" + ls[i])
        new_map = I.read()
        for p2 in NUMBER_OF_SUCC_VALUE_RANGE:
            for p1 in DIST_TO_SUCC_VALUE_RANGE:
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
                "<localsearchsteplimit>" + str(p1 * 2) + "</localsearchsteplimit>",
                new_map)

                new_map = re.sub(r"<numberofsuccessors>[\w]*</numberofsuccessors>",
                "<numberofsuccessors>" + str( (6 * p1 * p2 ) // 100) + "</numberofsuccessors>",
                new_map)

                new_map = re.sub(r"<distancetosuccessors>[\w]*</distancetosuccessors>",
                "<distancetosuccessors>" + str(p1) + "</distancetosuccessors>",
                new_map)

                O = open("tests/test_" + str(i) + "_" + str(p1) + "_" + str(p2) + ".xml", "w")
                O.write(new_map)
                O.close()


def main():
    #maps = []
    #for i in range(RUN_NUMBER):
    #    I = open("../resources/" + ls[i])
    #    maps.append(I.read())
    #    I.close()

    #template_strings = open("template.xml").read()
    #global_test_params = open("global_test_params.txt").read().split()

    #repeat_times = int(global_test_params[3])

    stats_file = open("stats.csv", "w", newline="", encoding="utf-8")
    stats = csv.writer(stats_file, delimiter=",")

    stats.writerow([""] + DIST_TO_SUCC_VALUE_RANGE)
    for p2 in NUMBER_OF_SUCC_VALUE_RANGE:
        new_stats_row = [str(p2)]
        for p1 in DIST_TO_SUCC_VALUE_RANGE:
            print("p1", p1, "p2", p2)
            avg_time = 0.0
            for i in range(0, len(ls)):
                print(i+1, "of", len(ls))
                print("../testing_scripts/tests/test_" + str(i) + "_" + str(p1) + "_" + str(p2) + ".xml")

                for j in range(REPEAT_TIMES):
                    print(" ", j + 1, "out of", REPEAT_TIMES)
                    rerun_counter = 0
                    while(True):
                        rerun_counter += 1
                        try:
                            p = subprocess.run(["../release/release/pathfinding_r_star.exe",
                            "../testing_scripts/tests/test_" + str(i) + "_" + str(p1) + "_" + str(p2) + ".xml"],
                            stdout=subprocess.PIPE,stdin=subprocess.PIPE, timeout=4)
                            out = p.stdout.decode()
                            break
                        except subprocess.TimeoutExpired:
                            print("  Unfortunate run (>3s), rerunning now...")
                        if rerun_counter == 5:
                            break
                    if rerun_counter == 5:
                        print("Limit reached, setting time as 3s")
                        avg_time += 4
                    else:
                        out = out.split("\r\n")
                        for s in out:
                            if s[:5] == "time=":
                                print(" ", s)
                                avg_time += float(s[5:])
                                break
            avg_time = avg_time / len(ls) / REPEAT_TIMES
            print("result", avg_time)
            print()
            new_stats_row += [avg_time]

        stats.writerow(new_stats_row)
    stats_file.close()
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
