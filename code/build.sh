
debugFlags='-DENGINE_DEBUG=1 -g'
engineFlags='-fPIC -shared'
platformFlags='-rdynamic'
commonFlags='-O3 -Wall -Wno-missing-braces -Wno-writable-strings'
platformLibraries='-lSDL2'

ME="$(readlink -f "$0")"
CODE_HOME="$(dirname "$ME")"

SOURCE="$1"
if [ -z "$SOURCE" ]; then
    SOURCE="$(readlink -f "$CODE_HOME/Linux_Main.cpp")"
fi

#echo ME = $ME
#echo SOURCE = $SOURCE
#echo CODE_HOME = $CODE_HOME

clang++ $debugFlags $commonFlags $CODE_HOME/assetProcessor/AssetProcessor_Main.cpp -o $CODE_HOME/assetProcessor/assetProcessor
"$CODE_HOME/assetProcessor/assetProcessor" "$CODE_HOME"

clang++ $debugFlags $commonFlags $engineFlags $CODE_HOME/Engine.cpp -o ../build/engine.so

clang++ $commonFlags $debugFlags $platformFlags $SOURCE -o ../build/main $platformLibraries
