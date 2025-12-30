#!/usr/bin/env bash
# Download the release
download() {
ZIP="$1"
URL="$2"
if [ ! -f "$ZIP" ]; then
  echo "Downloading $ZIP from $URL ..."
  curl -L "$URL" -o "$ZIP"
fi
}

# Extract the release
extract() {
ZIP="$1"
DIR="$2"
if [ ! -d "$DIR" ]; then
  echo "Unzipping $ZIP to .build/$DIR ..."
  cp "$ZIP" "$ZIP.bak"
  unzip -q "$ZIP"
  rm "$ZIP"
  mv "$ZIP.bak" "$ZIP"
fi
}

# Fetch the latest version of the library
fetch() {
if [ -d "CL" ]; then return; fi
URL1="https://github.com/KhronosGroup/OpenCL-Headers/archive/refs/heads/main.zip"
URL2="https://github.com/KhronosGroup/OpenCL-CLHPP/archive/refs/heads/main.zip"
URL3="https://github.com/KhronosGroup/OpenCL-ICD-Loader/archive/refs/heads/main.zip"
URL4="https://github.com/KhronosGroup/OpenCL-SDK/archive/refs/heads/main.zip"
ZIP1="OpenCL-Headers-main.zip"
ZIP2="OpenCL-CLHPP-main.zip"
ZIP3="OpenCL-ICD-Loader-main.zip"
ZIP4="OpenCL-SDK-main.zip"
DIR1="OpenCL-Headers-main"
DIR2="OpenCL-CLHPP-main"
DIR3="OpenCL-ICD-Loader-main"
DIR4="OpenCL-SDK-main"
mkdir -p .build
cd .build

# Download the releases
download "$ZIP1" "$URL1"
download "$ZIP2" "$URL2"
download "$ZIP3" "$URL3"
download "$ZIP4" "$URL4"
echo ""

# Extract the releases
extract "$ZIP1" "$DIR1"
extract "$ZIP2" "$DIR2"
extract "$ZIP3" "$DIR3"
extract "$ZIP4" "$DIR4"
echo ""
cd ..

# Copy the libs to the package directory
echo "Copying libs to CL/ ..."
rm -rf CL
mkdir -p CL CL/loader CL/SDK CL/Utils
cp -rf ".build/$DIR1/CL"/* CL/
cp -rf ".build/$DIR2/include/CL"/* CL/
cp -rf ".build/$DIR3/include"/* CL/
cp -rf ".build/$DIR3/loader"/* CL/loader/
cp -rf ".build/$DIR4/lib/include/CL/SDK"/* CL/SDK/
cp -rf ".build/$DIR4/lib/include/CL/Utils"/* CL/Utils/
echo ""
}


# Test the project
test() {
echo "Running 01-vector_addition.c ..."
clang -I. -o 01.exe examples/01-vector_addition.c        && ./01.exe && echo -e "\n"
# echo "Running 02-matrix_multiplication.c ..."
# clang -I. -o 02.exe examples/02-matrix_multiplication.c  && ./02.exe && echo -e "\n"
# echo "Running 03-image_processing.c ..."
# clang -I. -o 03.exe examples/03-image_processing.c       && ./03.exe && echo -e "\n"
# echo "Running 04-pi_estimation.c ..."
# clang -I. -o 04.exe examples/04-pi_estimation.c          && ./04.exe && echo -e "\n"
# echo "Running 05-nbody_simulation.c ..."
# clang -I. -o 05.exe examples/05-nbody_simulation.c       && ./05.exe && echo -e "\n"
}


# Main script
if [[ "$1" == "test" ]]; then test
elif [[ "$1" == "fetch" ]]; then fetch
else echo "Usage: $0 {fetch|test}"; fi
