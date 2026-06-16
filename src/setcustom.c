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

#define KIND_PRESET        0x08
#define KIND_CUSTOM_CONFIG 0x12

void fin_header(header_t *hdr) {
  hdr->reserved = 0;
  uint16_t sum = 0;
  uint8_t *data = (uint8_t*)hdr;
  for (uint8_t i = 0; i < 7; i++) sum += data[i];
  hdr->checksum = (uint8_t)(0xff - (sum & 0xff));
}

int send_custom(libusb_device_handle *handle, uint8_t *data) {
  header_t hdr;

  // step 1: custom config header
  hdr.kind        = KIND_CUSTOM_CONFIG;
  hdr.reserved    = 0x00;
  hdr.mode        = 1;
  hdr.speedLength = 8;
  hdr.brightness  = 0;
  hdr.color       = 0;
  hdr.reserved2   = 0x00;
  fin_header(&hdr);
  int r = libusb_control_transfer(handle, 0x21, 0x09, 0x0300, 0x0003,
                                  (uint8_t*)&hdr, 8, 0);
  if (r < 0) { printf("Control transfer 1 failed: %d\n", r); return -1; }

  // step 2: 8 x 64-byte interrupt transfers
  for (uint8_t i = 0; i < 8; i++) {
    int transferred = 0;
    r = libusb_interrupt_transfer(handle, (6 | LIBUSB_ENDPOINT_OUT),
                                  data + (i * 64), 64, &transferred, 0);
    if (r < 0 || transferred != 64) {
      printf("Interrupt transfer %d failed: %d\n", i, r);
      return -1;
    }
  }

  // step 3: commit header
  hdr.kind        = KIND_PRESET;
  hdr.reserved    = 0x00;
  hdr.mode        = 0x34;
  hdr.speedLength = 0x05;
  hdr.brightness  = 0x32;
  hdr.color       = 0x02;
  hdr.reserved2   = 0x01;
  fin_header(&hdr);
  r = libusb_control_transfer(handle, 0x21, 0x09, 0x0300, 0x0003,
                              (uint8_t*)&hdr, 8, 0);
  if (r < 0) { printf("Control transfer 2 failed: %d\n", r); return -1; }

  return 0;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("Usage: %s <512-byte-file>\n", argv[0]);
    return 1;
  }

  uint8_t data[512];
  memset(data, 0, 512);

  FILE *fd = fopen(argv[1], "rb");
  if (!fd) { perror("fopen"); return 1; }
  size_t n = fread(data, 1, 512, fd);
  fclose(fd);
  if (n != 512) { printf("Expected 512 bytes, got %zu\n", n); return 1; }

  libusb_context *ctx = NULL;
  libusb_init(&ctx);
  libusb_device_handle *h = libusb_open_device_with_vid_pid(ctx, 0x1044, 0x7a3f);
  if (!h) { printf("Failed to open device. Are you root?\n"); return 1; }

  libusb_set_auto_detach_kernel_driver(h, 0);
  libusb_set_auto_detach_kernel_driver(h, 3);
  if (libusb_claim_interface(h, 0) < 0) printf("Warning: claim iface 0 failed\n");
  if (libusb_claim_interface(h, 3) < 0) printf("Warning: claim iface 3 failed\n");

  int r = send_custom(h, data);

  libusb_release_interface(h, 0);
  libusb_release_interface(h, 3);
  libusb_close(h);
  libusb_exit(ctx);
  return r < 0 ? 1 : 0;
}
