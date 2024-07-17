mkdir -p $1
sudo minicom -C "$1/$2" -D /dev/ttyUSB0
