#! /bin/bash
name=
if [ $# -lt 1 ]
then
echo "start <name>"
exit 1
fi
name=$1
st=`pidof $name`
if [ -n "$st" ]
then
echo "kill"$st
kill -9 $st
fi
sleep 1
./"$name"
echo "now start" $name  `pidof $name`

