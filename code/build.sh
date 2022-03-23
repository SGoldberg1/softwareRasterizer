
debugFlags='-DENGINE_DEBUG=1 -g'
engineFlags='-fPIC -shared'
platformFlags='-rdynamic'
commonFlags='-O0 -Wall -Wno-missing-braces -Wno-writable-strings'
platformLibraries='-lSDL2'


clang++ $debugFlags $commonFlags $engineFlags Engine.cpp -o ../build/engine.so


clang++ $commonFlags $debugFlags $platformFlags Linux_Main.cpp -o ../build/main $platformLibraries