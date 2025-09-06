#!/bin/bash

count=$# #$# — количество аргументов, переданных скрипту.
sum=0

for num in "$@"; do
    sum=$(awk -v s="$sum" -v n="$num" 'BEGIN {print s + n}')
done

average=$(awk -v s="$sum" -v c="$count" 'BEGIN {printf "%.3f", s/c}')

echo "Количество аргументов: $count"
echo "Среднее арифметическое: $average"

