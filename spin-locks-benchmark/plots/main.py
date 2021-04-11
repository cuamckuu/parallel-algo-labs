#!/usr/bin/env wpython

import collections
import matplotlib
matplotlib.use('agg')
import matplotlib.pyplot as plt

if __name__ == '__main__':
    data_txt = None
    with open("data.txt", "r", encoding="utf-8") as file:
        data_txt = file.read()

    data = data_txt.split("===\n")[0:-1]

    time_elapsed = collections.defaultdict(list)
    for res in data:
        threads, *algos = res.split("\n")

        thread_num = int(threads.split(" ", 1)[0])

        for algo in algos:
            if not algo:
                continue
            name, time = algo.split(" ", 1)
            time_elapsed[name].append((thread_num, float(time)))

    for algo, data in time_elapsed.items():
        xs, ys = zip(*data)
        plt.plot(xs, ys, label=algo)

    plt.title("Сравнение TS, TTS, Backoff")
    plt.xlabel("Число потоков, шт")
    plt.ylabel("Время выполнения, мс")
    plt.legend()
    plt.savefig("res.png")
