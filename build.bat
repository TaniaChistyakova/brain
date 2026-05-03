del /q "out\CMakeCache.txt"
cmake -G "Visual Studio 18 2026" -A x64 -S "." -B "out"
start /d "out" brain.slnx

