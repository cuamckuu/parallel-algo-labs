# Лабораторная 1. Алгоритмы блокировки потоков

**Задание на 5. Вариант 2.** Сравнить алгоритмы TestAndSet, TestAndTestAndSet, Backoff

## Сборка

```bash
# Загрузка исходников
git clone https://github.com/cuamckuu/parallel-algo-labs.git
cd parallel-algo-labs

# Сборка проекта
mkdir lab1_build
cd lab1_build
cmake ../spin-locks-benchmark
make

# Запуск
./SpinLocksBenchmark

# Построение графиков
./SpinLocksBenchmark > ./../spin-locks-benchmark/plots/data.txt
cd ./../spin-locks-benchmark/plots
python3 main.py

# Открыть файл res.png
```

## Теоретический результат

![ideal](./spin-locks-benchmark/plots/ideal_res.png)

## Результат эксперимента

![res](./spin-locks-benchmark/plots/res.png)

## Выводы

> TODO

# Лабораторная 2. Разделяемые структуры данных

**Задание на 5. Вариант 2.** Применить метод ленивой синхронизации к другим структурам данных (подобрать структуру, реализовать)


