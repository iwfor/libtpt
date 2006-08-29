#/bin/sh

echo "Buffer test"
./buffertest buffertest.cxx
echo "Parser test"
./test1 54
echo "IParser test"
./test2 2
echo "Object test"
./test3 1
