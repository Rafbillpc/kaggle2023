#!/bin/sh
git show HEAD:submission.csv > old.csv
./build/release/stats
