# cmake
cd /tmp

if [ ! -d "cmdfx" ]; then
    git clone https://github.com/gmitch215/cmdfx.git
    cd cmdfx
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DTEST_CMDFX=OFF -DKN_CMDFX=OFF
    cmake --build build --config Release --target install
    cd ..
    rm -rf cmdfx
fi