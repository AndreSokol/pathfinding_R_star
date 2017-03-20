import subprocess, csv

import sys

import plotly
import plotly.graph_objs as go

import pandas as pd

TEMPLATE_ENDING = """
"""

DIST_TO_SUCC_VALUE_RANGE = list(range(5, 151, 5))
NUMBER_OF_SUCC_VALUE_RANGE = list(range(10, 301, 10))

def main():
    template_strings = open("template.xml").read()
    global_test_params = open("global_test_params.txt").read().split()

    repeat_times = int(global_test_params[3])

    stats_file = open("stats.csv", "w", newline="", encoding="utf-8")
    stats = csv.writer(stats_file, delimiter=",")

    stats.writerow([""] + DIST_TO_SUCC_VALUE_RANGE)
    for p2 in NUMBER_OF_SUCC_VALUE_RANGE:
        new_stats_row = [str(p2)]
        for p1 in DIST_TO_SUCC_VALUE_RANGE:

            input_text = template_strings.replace("# PASTE PARAMS #", """
            <searchtype>""" + global_test_params[0] + """</searchtype>
            <metrictype>""" + global_test_params[1] + """</metrictype>
            <hweight>""" + global_test_params[2] + """</hweight>
            <breakingties>g-max</breakingties>
            <linecost>1</linecost>
            <diagonalcost>1.41421356237</diagonalcost>
            <allowdiagonal>1</allowdiagonal>
            <allowsqueeze>0</allowsqueeze>
            <localsearchsteplimit>100</localsearchsteplimit>
            <distancetosuccessors>""" + str(p1) + """</distancetosuccessors>
            <numberofsuccessors>""" + str(p2) + """</numberofsuccessors>
            """)

            input_file = open("input.xml", "w")
            print(input_text, file=input_file)
            input_file.close()

            print("Generated test for:\ndistance to successors", p1, "\nnumber of successors", p2)

            avg_time = 0
            for i in range(repeat_times):
                print(i+1, "of", repeat_times)

                p = subprocess.Popen(["../release/release/asearch.exe", "../testing_scripts/input.xml"],stdout=subprocess.PIPE,stdin=subprocess.PIPE)
                out = p.communicate()
                p.stdin.close()

                out = out[0].decode().split("\r\n")
                for s in out:
                    if s[:5] == "time=":
                        print(s)
                        avg_time += float(s[5:])
                        break
                #print()
            avg_time = avg_time / repeat_times
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
        go.Heatmap(
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


if __name__ == '__main__':
    if len(sys.argv) == 1:
        main()
    elif sys.argv[1] == '-plot':
        plot()
