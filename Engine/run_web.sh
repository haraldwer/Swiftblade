#!/usr/bin/env sh

cd $1
ptyxis -x "emrun --no-browser --port 8080 Application.html" &
sleep 1s
flatpak run io.github.ungoogled_software.ungoogled_chromium http://localhost:8080/Application.html &
