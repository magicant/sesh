set -eu

case $1#${CC} in
  (--install#gcc)
    sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
    sudo apt-get update
    sudo apt-get -q -y install gcc-4.8 g++-4.8
    sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.8 50
    sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.8 50
    ${CC} --version
    ${CXX} --version
    ;;
  (--install#*)
    sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
    sudo apt-get update
    sudo apt-get -q -y install libstdc++-4.8-dev
    ;;
  (--run#*)
    autoreconf -i
    sh configure
    make distcheck
    ;;
esac
