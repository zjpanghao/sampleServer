#!/bin/bash
if [ $# -lt 1 ]
then 
echo "change <pro>"
exit 1
fi
ub=`cat /etc/os-release |grep ubuntu|wc -l`
pg=$1
echo $pg
pg2=${pg^}
echo $pg2
cd src
if [ $ub -gt 0 ]
then
  echo "ubuntu"
  rename "s/projecttest/$pg/g" *
else
  echo "centos"
  rename projecttest $pg *
fi

sed -i "s/projecttest/$pg/g" `find . -name "*.h" -o -name "*.cpp"`
sed -i "s/Projecttest/$pg2/g" `find . -name "*.h" -o -name "*.cpp"`

