
#!/bin/bash
set -e
INSTALL_BIN=/usr/local/bin
SERVICE_DIR=/etc/systemd/system
STATE_DIR=/var/lib/fusion-kbd

echo "==> Installing dependencies..."
apt-get install -y libusb-1.0-0 libusb-1.0-0-dev pkg-config gcc

echo "==> Compiling fusion-setmode..."
gcc src/setmode.c $(pkg-config --libs --cflags libusb-1.0) -o /tmp/fusion-setmode
install -m 755 /tmp/fusion-setmode "$INSTALL_BIN/fusion-setmode"

echo "==> Installing kb command..."
install -m 755 scripts/kb "$INSTALL_BIN/kb"

echo "==> Installing restore script..."
install -m 755 systemd/fusion-kbd-restore "$INSTALL_BIN/fusion-kbd-restore"

echo "==> Installing systemd service..."
install -m 644 systemd/fusion-kbd.service "$SERVICE_DIR/fusion-kbd.service"
systemctl daemon-reload
systemctl enable fusion-kbd.service

echo "==> Creating state directory..."
mkdir -p "$STATE_DIR"

echo "==> Compiling fusion-setcustom..."
gcc src/setcustom.c $(pkg-config --libs --cflags libusb-1.0) -o /tmp/fusion-setcustom
install -m 755 /tmp/fusion-setcustom "$INSTALL_BIN/fusion-setcustom"

echo "==> Installing kb-mkpayload..."
install -m 755 scripts/kb-mkpayload "$INSTALL_BIN/kb-mkpayload"

mkdir -p /tmp/fusion-kbd
chmod 777 /tmp/fusion-kbd

echo ""
echo "Done! Try: kb red  or  kb neon  or  kb breathing blue"
echo "Run 'kb --help' for all modes and colors."
