#!/usr/bin/env sh

cd $1
gnome-terminal -- python -m http.server 8080 &
sleep 3s
xdg-open http://localhost:8080/Game.html &