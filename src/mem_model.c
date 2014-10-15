#include <stdio.h>
#include "svdpi.h"

#define DAT_TYPE int
#define ADR_TYPE int
#define FLG_TYPE int

// DPI-C verification model

typedef struct {
	DAT_TYPE offset;
	ADR_TYPE data;
	FLG_TYPE rw;
} packet_t;

void recieve_packet( packet_t *trans ) {
  static int i=1;
  static int j=10;
  static int k=1;
  trans->offset = i;
  trans->data = j;
  trans->rw = k;
  i++;
  j++;
  if ( k == 0 ) {
  	k = 1;
  } else {
  	k = 0;
  }
}

void send_packet( packet_t *trans ) {
  printf("offset=%d\n", trans->offset);
  printf("data  =%d\n", trans->data);
  printf("rw    =%d\n", trans->rw);
}

void init_socket( void ) {
}

