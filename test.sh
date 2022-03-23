# !/bin/sh

./build/app/daemon/daemon 192.168.0.3 1337
./build/app/client/client 192.168.0.3 1337 ~/Downloads/image.jpg
killall -s SIGTERM daemon
sha256sum ~/Downloads/image.jpg ./**_image.jpg
# rm ./**_image.jpg