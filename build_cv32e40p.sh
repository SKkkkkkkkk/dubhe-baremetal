cmake -S . -B build/cv32e40p -DDEVICE=CV32E40P -DPROJECT_NAME=hello -DPROJECT_PATH=app/hello_world
cmake --build build/cv32e40p -j100 