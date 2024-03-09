#!/bin/bash
if [ "$2" = "-h" ] || [ "$2" = "--help" ] || [ "$2" = "" ] || [ "$1" = "-h" ] || [ "$1" = "--help" ] || [ "$1" = "" ]; then
	if [ "$2" = "" ] || [ "$1" = "" ]; then
		echo "USAGE"
	fi
	echo "Флаг -h или --help выводит краткую инфорацию о том, как работать с скриптом"
	echo "$ - это переменная. Мы обращаемся именно к тому, что под ней"
	echo "Cкрипт переименовывает файлы с определённым суффиксом, добавляя к ним префикс"
	echo "Вам нужно будет ввести суффикс для имени файла и префикс"
	exit 0
fi
		
suffix = $2
prefix = $1


for file in *.$suffix
do
	remove $file $prefix$file
done

exit 0
