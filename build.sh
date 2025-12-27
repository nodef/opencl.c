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
echo "Running 01-print-version.c ..."
clang -I. -o 01.exe examples/01-print-version.c  && ./01.exe && echo -e "\n"
echo "Running 02-simple-buffer.c ..."
clang -I. -o 02.exe examples/02-simple-buffer.c  && ./02.exe && echo -e "\n"
echo "Running 03-file-compress.c ..."
clang -I. -o 03.exe examples/03-file-compress.c
./03.exe examples/03-file-compress.c && echo -e "\n"
}


# Main script
if [[ "$1" == "test" ]]; then test
elif [[ "$1" == "fetch" ]]; then fetch
else echo "Usage: $0 {fetch|test}"; fi
