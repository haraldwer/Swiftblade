#!/usr/bin/env sh

cd $1
ptyxis -x "python -m http.server 8080" &
sleep 1s
xdg-open http://localhost:8080/Game.html &