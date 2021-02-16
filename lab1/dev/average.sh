#!/bin/bash

echo "You entered ${#} parameters"
sum=0
for param in $@
do
sum=$(( $sum+$param ))
done
ans=$(( $sum/$# ))
echo "Their arithmetic mean $ans"