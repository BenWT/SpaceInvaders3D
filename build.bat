mkdir build
cd build
conan install .. --build missing
cmake .. -G "Visual Studio 15 Win64"
cmake --build .
cd bin
.\SpaceInvaders.exe
cd ../..
