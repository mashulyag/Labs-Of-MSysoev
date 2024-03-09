import os
import sys

if (sys.argv[2] == "-h") or (sys.argv[2] == "--help") or (sys.argv[2] == "") or (sys.argv[1] == "-h") or (sys.argv[1] == "--help") or (sys.argv[1] == ""):
    if (sys.argv[2] == "") or (sys.argv[1] == ""):
        print("USAGE")
    print("Флаг -h или --help выводит краткую инфорацию о том, как работать с скриптом")
    print("$ - это переменная. Мы обращаемся именно к тому, что под ней")
    print("Cкрипт переименовывает файлы с определённым суффиксом, добавляя к ним префикс")
    print("Вам нужно будет ввести суффикс для имени файла и префикс")
    sys.exit()
else:
    prefix = sys.argv[1]
    suffix = sys.argv[2]
    current_directory = os.getcwd()
    directory = os.fsdecode(current_directory)
    for file in os.listdir(directory):
        if file.endswith(suffix):
            name_of_file = os.fsdecode(file)
            new_name_of_file = prefix + name_of_file
            os.system(f'remove {name_of_file} {new_name_of_file}')
    sys.exit()
