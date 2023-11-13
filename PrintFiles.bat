forfiles /S /M *.cpp /C "cmd /c type @PATH" >> cpp.txt
forfiles /S /M *.h /C "cmd /c type @PATH" >> h.txt