echo "number of tests"
read num
for i in $(seq $num)
do
        echo "! thing ran $i"
            ./msort 509 < ./Alice.txt 2& > /dev/null
done
