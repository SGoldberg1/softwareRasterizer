
debugFlags='-DENGINE_DEBUG=1 -g'
engineFlags='-fPIC -shared -msse4.1'
platformFlags='-rdynamic'
commonFlags='-O0 -Wall -Wno-missing-braces -Wno-writable-strings'
platformLibraries='-lSDL2'


clang++ $debugFlags $commonFlags $engineFlags ../code/Engine.cpp -o ../build/engine.so


clang++ $commonFlags $debugFlags $platformFlags ../code/Linux_Main.cpp -o ../build/main $platformLibraries