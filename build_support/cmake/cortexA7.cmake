set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR ARM)

# specify the cross compiler
set(CMAKE_C_COMPILER   /usr/bin/arm-linux-gnueabihf-gcc-9)
set(CMAKE_CXX_COMPILER /usr/bin/arm-linux-gnueabihf-g++-9)

# set linker
set(CMAKE_LINKER /usr/bin/arm-linux-gnueabihf-ld)

set(CMAKE_FIND_ROOT_PATH /usr/arm-linux-gnueabihf)

