#!/bin/bash

set -e

make -C cmake-build-release

rm -rf cmake-build-release/smoke
mkdir -p cmake-build-release/smoke

function check() {
  local scene=$1
  local size=$2
  local spp=$3
  for way in oo fp dod; do
    echo Testing $way:
    ./cmake-build-release/bin/pt_three_ways \
      --width $size --height $size \
      --max-cpus 0 --spp $spp --scene $scene \
      --way $way cmake-build-release/smoke/${scene}_${way}.png
  done
}

check cornell 128 32
check single-sphere 96 32
check suzanne 96 16
