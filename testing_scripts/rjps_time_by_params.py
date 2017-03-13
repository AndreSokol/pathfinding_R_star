import subprocess, csv

TEMPLATE_ENDING = """<localsearchsteplimit>100</localsearchsteplimit>
    </algorithm>
    <options>
        <loglevel>1</loglevel>
        <logpath />
        <logfilename />
    </options>
</root>
"""

DIST_TO_SUCC_VALUE_RANGE = [1, 2, 3, 4, 5, 7, 10, 15, 20, 25, 30, 40, 50]
NUMBER_OF_SUCC_VALUE_RANGE = list(range(1, 101))


def main():
    template_strings = open("template.xml").read()
    stats_file = open("stats.csv", "w", encoding="utf-8")
    stats = csv.writer(stats_file, delimiter=",")

    stats.writerow(["_"] + DIST_TO_SUCC_VALUE_RANGE)
    for p2 in NUMBER_OF_SUCC_VALUE_RANGE:
        new_stats_row = [str(p2)]
        for p1 in DIST_TO_SUCC_VALUE_RANGE:
            input_file = open("input.xml", "w")

            print(template_strings, file=input_file)

            print("<distancetosuccessors>", p1, "</distancetosuccessors>", sep="", file=input_file)
            print("<numberofsuccessors>", p2, "</numberofsuccessors>", sep="", file=input_file)

            print(TEMPLATE_ENDING, file=input_file)
            input_file.close()

            print("Generated test for:\ndistance to successors", p1, "\nnumber of successors", p2)

            
            p = subprocess.Popen(["../debug/debug/asearch.exe", "../testing_scripts/input.xml"],stdout=subprocess.PIPE,stdin=subprocess.PIPE)
            #p.stdin.write(inpstr.encode())
            out = p.communicate()
            p.stdin.close()

            out = out[0].decode().split("\r\n")
            for s in out:
                if s[:5] == "time=":
                    print(s)
                    new_stats_row += [s[5:]]
                    break
            print()
        stats.writerow(new_stats_row)

    stats_file.close()
    return


if __name__ == '__main__':
    main()