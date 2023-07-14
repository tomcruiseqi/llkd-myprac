# requirements.

To build the toolchain from source, one needs:

- LLVM 3.7.1 or newer, compiled with BPF support (default=on) 
- Clang, built from the same tree as LLVM
- cmake (>=3.1), gcc (>=4.7), flex, bison
- LuaJIT, if you want Lua support

## install dependents
- ```sudo apt install -y bison build-essential cmake flex git libedit-dev libdebuginfod-dev```
- ```sudo apt install -y libllvm14 llvm-14-dev libclang-14-dev python3 zlib1g-dev```
- ```sudo apt install -y libfl-dev python3-setuptools libdebuginfod-common```
- ```sudo apt install -y luajit libluajit-5.1-dev libluajit-5.1-dev libelf-dev```

# build commands
- ```git clone https://github.com/iovisor/bcc.git 39-bcc```
- ```cd 39-bcc & mkdir build & cd build```
- ```cmake ..```
- ```make```
- ```sudo make install```
- ```cmake -DPYTHON_CMD=python3 .. # build python3 binding```
- ```pushd src/python/```
- ```make```
- ```sudo make install```
- ```popd```

