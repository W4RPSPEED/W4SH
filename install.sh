echo "Welcome to the TabSH auto-builder!"
echo "This will write the binary and relevant cmake files to ./build."

echo "cleaning build..."
rm -rf build
echo "creating and changing to directory.."
mkdir build
cd build
cmake ..
make
echo "Enjoy!"