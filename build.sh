#!/usr/bin/env bash
# Fetch the latest version of the library
fetch() {
if [ -d "CL" ]; then return; fi
URL="https://github.com/KhronosGroup/OpenCL-Headers/archive/refs/heads/main.zip"
ZIP="main.zip"
DIR="OpenCL-Headers-main"
mkdir -p .build
cd .build

# Download the release
if [ ! -f "$ZIP" ]; then
  echo "Downloading $ZIP from $URL ..."
  curl -L "$URL" -o "$ZIP"
  echo ""
fi

# Unzip the release
if [ ! -d "$DIR" ]; then
  echo "Unzipping $ZIP to .build/$DIR ..."
  cp "$ZIP" "$ZIP.bak"
  unzip -q "$ZIP"
  rm "$ZIP"
  mv "$ZIP.bak" "$ZIP"
  echo ""
fi
cd ..

# Copy the libs to the package directory
echo "Copying libs to CL/ ..."
rm -rf CL
mkdir -p CL
cp -f ".build/$DIR/CL"/* CL/
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
