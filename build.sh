#! /bin/bash

TYPE=0 # debug

while getopts ":hg" option; do
  case $option in
    h) echo "usage: $0 [-h] [-g]"; exit ;;
    g) TYPE=1 ;;
    ?) echo "error: option -$OPTARG is not implemented"; exit ;;
  esac
done

clear

# if [ $TYPE -eq 1 ]
# then
#   cmake .
# else
#   cmake --build . --config Release
# fi
cmake .
make
./test --success
