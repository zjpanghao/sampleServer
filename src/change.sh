#!/bin/bash
rename 's/ems/ems/' *
sed -i "s/face/ems/g" `grep ems -rl ./|grep -v sh`
sed -i "s/Face/Ems/g" `grep ems -rl ./|grep -v sh`

