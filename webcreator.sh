#!/bin/bash

if [ $# -ne 4 ]; then
  echo 'wrong number of arguments' $#
  exit -1
fi
root_dir=$1
text_file=$2
w=$3
p=$4

#perform necessary checks here!
if [ ! -d "$root_dir" ]; then
  echo "root_dir doesn't exist, exiting..."
  exit -1
fi

if [ ! -f "$text_file" ]; then
  echo "root_dir doesn't exist, exiting..."
  exit -1
fi

case $w in
    ''|*[!0-9]*)
    echo 'w is not a number!'
    exit ;;
    *) echo 'w is a number!' ;;
esac

case $p in
    ''|*[!0-9]*)
    echo 'p is not a number!'
    exit ;;
    *) echo 'p is a number!' ;;
esac

#check if files has more than 10.000 lines
lines=$(wc -l < "$text_file")
echo "File has $lines lines"
if [ $lines -lt 10000 ]; then
  echo "File $text_file has less than 10.000 lines ($lines)! exiting..."
  exit -1
fi

#check if root_dir was empty and if not purge it
if [ "$(ls -A $root_dir)" ]; then
  echo "root_dir was not empty! I will purge it..."
  rm -rf "$root_dir"/*
fi

#compute f and q
((f = p/2 + 1))
((q = w/2 +1))
echo "f = $f q = $q"
total_links=() # declare array that will keep the links

#start creating pages
for ((i=0; i<$w; i++)); do
  dir="$root_dir"/site$i
  mkdir $dir
  #generate page numbers for website $i
  for ((j=0; j<$p; j++)); do
    declare -A links
    #choose a random number as page number
    new_link=$RANDOM
    while [ ${links[$new_link]+_} ]; do #rechoose links if they already exist
      new_link=$RANDOM
    done
    links[$new_link]=1
    #save links in temporary files
    echo site"$i"/page"$i"_"$new_link".html >> "$root_dir"/SITE$i
    unset links
  done
done
links=()
#here we create actual pages
for ((i=0; i<$w; i++)); do
  echo "Creating website $i"
  readarray -t pages < "$root_dir"/SITE$i

  for ((j=0; j<$p; j++)); do
    file="$root_dir"/"${pages[$j]}"
    ((k=$RANDOM % ($lines -1998) +2 ))
    ((m=$RANDOM % 999 + 1001))
    #no of lines to write in each iteration
    ((range = m/(f+q)))
    #remove self from links --> break into two sub arrays
    links=("${pages[@]:0:$j}" "${pages[@]:(($j+1))}")
    #keep $f random links from same website
    links=($(printf "%s\n" "${links[@]}" | shuf | head -n $f))
    # pick q links from other websites
    links+=($(cat `find "$root_dir" -not -name "SITE$i" | grep SITE*` | shuf |head -n $q))
    #echo "links" ${links[@]}
    total_links=("${total_links[@]}" "${links[@]}")
    echo "Creating page: " "$file"
    echo "<!DOCTYPE  html>" > $file
    echo "<html>" >> $file
    echo "<body>">> $file
    #write contents to page
    for ((l=0; l<$f+$q; l++)); do
      ((end=k+range))
      sed -n "$k,$end p" $text_file >> $file
      echo "Adding link: " ${links[$l]}
      echo "<br>  <a href='../${links[$l]}'>${links[$l]}</a><br>" >> $file
      ((k+=range))
    done
    echo "</body>" >> $file
    echo '</html>' >> $file
  done
done

total_links=($(printf "%s\n" "${total_links[@]}" | sort -u));
if [[ "${#total_links[@]}" -eq "$(($w * $p))" ]]; then
  echo "All files have at least one incoming link!"
fi
#sed -n 5,8p file  --> print lines 5 to 8
#$ foo=${string#$prefix}  --> to remove prefix
#cat SITE[^0] | shuf |head -n 5 --> get random links not zero
#cleanup temporary files
rm -rf  "$root_dir"/SITE*
