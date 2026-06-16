#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <libusb-1.0/libusb.h>

#pragma pack(push, 1)
typedef struct {
  uint8_t kind;
  uint8_t reserved;
  uint8_t mode;
  uint8_t speedLength;
  uint8_t brightness;
  uint8_t color;
  uint8_t reserved2;
  uint8_t checksum;
} header_t;
#pragma pack(pop)

#define KIND_PRESET     0x08
#define RESERVED1       0x00

void fin_header(header_t *hdr) {
  hdr->reserved = 0;
  uint16_t sum = 0;
  uint8_t *data = (uint8_t*)hdr;
  for (uint8_t i = 0; i < 7; i++) sum += data[i];
  hdr->checksum = (uint8_t)(0xff - (sum & 0xff));
}

int set_mode(libusb_device_handle *handle, uint8_t mode, uint8_t color, uint8_t brightness, uint8_t speed) {
  header_t hdr;

  // Init transfer
  hdr.kind       = KIND_PRESET;
  hdr.reserved   = RESERVED1;
  hdr.mode       = 0x33;
  hdr.speedLength = 0x05;
  hdr.brightness = 0x32;
  hdr.color      = 0x02;
  hdr.reserved2  = 0x01;
  fin_header(&hdr);
  int r = libusb_control_transfer(handle, 0x21, 0x09, 0x0300, 0x0003, (uint8_t*)&hdr, 8, 0);
  if (r < 0) { printf("Control transfer 1 failed: %d\n", r); return -1; }

  // Mode transfer
  hdr.mode       = mode;
  hdr.speedLength = speed;
  hdr.brightness = brightness;
  hdr.color      = color;
  hdr.reserved2  = 0;
  fin_header(&hdr);
  r = libusb_control_transfer(handle, 0x21, 0x09, 0x0300, 0x0003, (uint8_t*)&hdr, 8, 0);
  if (r < 0) { printf("Control transfer 2 failed: %d\n", r); return -1; }

  return 0;
}

int main(int argc, char **argv) {
  if (argc < 5) {
    printf("Usage: %s <mode> <color> <brightness 0-50> <speed 1-10>\n", argv[0]);
    return 1;
  }
  uint8_t mode   = (uint8_t)atoi(argv[1]);
  uint8_t color  = (uint8_t)atoi(argv[2]);
  uint8_t bright = (uint8_t)atoi(argv[3]);
  uint8_t speed  = (uint8_t)atoi(argv[4]);

  libusb_context *ctx = NULL;
  libusb_init(&ctx);
  libusb_device_handle *h = libusb_open_device_with_vid_pid(ctx, 0x1044, 0x7a3f);
  if (!h) { printf("Failed to open device. Are you root?\n"); return 1; }

  libusb_set_auto_detach_kernel_driver(h, 0);
  libusb_set_auto_detach_kernel_driver(h, 3);
  if (libusb_claim_interface(h, 0) < 0) printf("Warning: claim iface 0 failed\n");
  if (libusb_claim_interface(h, 3) < 0) printf("Warning: claim iface 3 failed\n");

  int r = set_mode(h, mode, color, bright, speed);

  libusb_release_interface(h, 0);
  libusb_release_interface(h, 3);
  libusb_close(h);
  libusb_exit(ctx);
  return r < 0 ? 1 : 0;
}
