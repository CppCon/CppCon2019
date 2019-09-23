#!/bin/bash

set -euo pipefail

BIN=${1}
OUT_DIR=${2}/seed

rm -rf "${OUT_DIR}"
mkdir -p "${OUT_DIR}"

for way in oo fp dod; do
  echo Testing $way:
  ${BIN} \
    --width 16 --height 16 \
    --seed 1 \
    --max-cpus 1 --spp 16 --scene cornell \
    --way $way "${OUT_DIR}"/${way}-1.raw

  ${BIN} \
    --width 16 --height 16 \
    --seed 1 \
    --max-cpus 1 --spp 16 --scene cornell \
    --way $way "${OUT_DIR}"/${way}-1-again.raw
  diff -s "${OUT_DIR}"/${way}-1.raw "${OUT_DIR}"/${way}-1-again.raw

  ${BIN} \
    --width 16 --height 16 \
    --seed 2 \
    --max-cpus 1 --spp 16 --scene cornell \
    --way $way "${OUT_DIR}"/${way}-2.raw
  if diff -s "${OUT_DIR}"/${way}-1.raw "${OUT_DIR}"/${way}-2.raw; then
    echo "Files are identical and should differ"
    exit 1
  fi
done
