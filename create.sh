#!/bin/bash

cd reuben3/data
rm *
wget https://www.sorunome.de/reuben3-meta/out/area_enemies.h
wget https://www.sorunome.de/reuben3-meta/out/sprites.h
wget https://www.sorunome.de/reuben3-meta/out/maps.h
wget https://www.sorunome.de/reuben3-meta/out/defines.h
cd ../..
