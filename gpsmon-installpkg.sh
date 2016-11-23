#!/bin/bash

echo "backup"
mkdir -p ./gps-bckp/home/$USER/.config/upstart
mkdir -p ./gps-bckp/storage/ca-st-ro/bin
cp /storage/ca-st-ro/bin/gpsmon ./gps-bckp/storage/ca-st-ro/bin/
cp /home/$USER/.config/upstart/gpsmon.conf ./gps-bckp/home/$USER/.config/upstart/

echo "install"
killall gpsmon
tar xvf gpsmon-pkg.tar.gz
rsync -av pkg/storage/ /storage/
rsync -av pkg/ubuntuhome/ /home/$USER/
rm -r pkg
killall gpsmon

echo "done"
