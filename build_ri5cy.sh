cmake -S . -B build/ri5cy -DPROJECT_NAME=hello -DPROJECT_PATH=app/ri5cy/hello -DCORE=RI5CY -DBOARD=RTl -DBUILD_TYPE=Debug
cmake --build build/ri5cy -j100