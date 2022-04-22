cmake -S . -B build/cv32e40p -DPROJECT_NAME=hello -DPROJECT_PATH=app/cv32e40p/hello -DCORE=CV32E40P -DBOARD=RTL -DBUILD_TYPE=Debug
cmake --build build/cv32e40p -j100