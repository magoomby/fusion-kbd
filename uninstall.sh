#!/bin/bash
set -e
echo "==> Stopping and disabling service..."
systemctl stop fusion-kbd.service 2>/dev/null || true
systemctl disable fusion-kbd.service 2>/dev/null || true
rm -f /etc/systemd/system/fusion-kbd.service
systemctl daemon-reload

echo "==> Removing binaries..."
rm -f /usr/local/bin/fusion-setmode
rm -f /usr/local/bin/fusion-kbd-restore
rm -f /usr/local/bin/kb

echo "==> Removing state..."
rm -rf /var/lib/fusion-kbd

echo "Done."
