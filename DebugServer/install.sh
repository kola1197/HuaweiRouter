sudo apt update && sudo apt upgrade -y
sudo apt install cmake
sudo apt install qt5-default
sudo apt install zlib1g zlib1g-dev
mkdir build
cd build
cmake ..
make -j
