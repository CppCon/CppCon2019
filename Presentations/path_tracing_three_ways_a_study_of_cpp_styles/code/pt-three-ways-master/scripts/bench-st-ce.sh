#!/bin/bash

set -e

make -C cmake-build-release pt_three_ways

for way in oo fp dod; do
  echo Testing $way:
  time ./cmake-build-release/bin/pt_three_ways \
    --save-every 0 \
    --width 64 --height 64 \
    --max-cpus 1 --spp 1 --scene ce \
    --way $way /dev/null
done
