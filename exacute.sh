#!/bin/sh
IP=pi@10.141.19.99
DPI="/home/pi/Desktop/scrivejack"
echo Exacuting
ssh $IP "cd $DPI; sudo ./avc"