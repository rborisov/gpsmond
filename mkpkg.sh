#!/bin/bash

mkdir -p pkg/storage/ca-st-ro/bin

cp -r etc/* pkg/
cp build/gpsmon pkg/storage/ca-st-ro/bin/

tar czf gpsmon-pkg.tar.gz pkg
