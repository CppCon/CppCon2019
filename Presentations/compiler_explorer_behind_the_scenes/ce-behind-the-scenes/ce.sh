#!/bin/bash

cat > /tmp/test.cc <<EOF
// Edit me!
int square(int x)
{
  return x * x;
}
EOF
tmux new -s ce -d watch -n 0.5 "g++ /tmp/test.cc -O2 -std=c++0x -c -S -o - -masm=intel | c++filt | grep -vE '\s+\.'"
tmux split-window -h vim /tmp/test.cc
tmux -2 attach-session -d
