/*
 *  HW_IF 
 *
 * This code is licensed under the GPL.
 */

#include "hw/sysbus.h"
#include "sysemu/char.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>

#define TYPE_HW_IF "hw_if"
#define HW_IF(obj) OBJECT_CHECK(HW_IFState, (obj), TYPE_HW_IF)
//#define DEBUG_PRINT(fmt, ...) printf("%s(): "fmt, __func__, ## __VA_ARGS__)
#define DEBUG_PRINT(fmt, ...)

typedef struct __attribute__((packed)) {   
    unsigned int offset;
    unsigned int data;
    int we; //Read 0 Write 1
} rw_packet_t; 

typedef struct HW_IFState {
    SysBusDevice parent_obj;

    MemoryRegion iomem;
    int sock0;
    int sock;
    CharDriverState *chr;
    qemu_irq irq;
    const unsigned char *id;
} HW_IFState;


#if 0
static void hw_if_update(HW_IFState *s)
{
    DEBUG_PRINT("%s : %d\n",__FILE__,__LINE__);
    uint32_t flags;

    flags = 0;
    //flags = s->int_level & s->int_enabled;
    qemu_set_irq(s->irq, flags != 0);
}
#endif

static void hw_if_dut_read(void *opaque , rw_packet_t *rw_packet){
    int ret;
    
    HW_IFState *s = (HW_IFState *)opaque;
    
    rw_packet->we = 0;
    
    DEBUG_PRINT("send:: offse = 0x%x\n",rw_packet->offset);
    ret = send(s->sock,rw_packet, sizeof(*rw_packet), 0);
    if (ret < 1) {
	perror("[hw_if] write error");
        exit(EXIT_FAILURE);
    } 
    DEBUG_PRINT("send::ret = %d\n",__LINE__,ret);

    sleep(1);

    /* サーバからデータを受信 */
    memset(rw_packet, 0, sizeof(*rw_packet));
    ret = recv(s->sock, rw_packet, sizeof(*rw_packet), 0);
    if (ret < 0) {
	perror("[hw_if] read error");
        exit(EXIT_FAILURE);
    } 

    DEBUG_PRINT("recv::ret = %x read data = 0x%x\n",ret,rw_packet->data);
    
}

static uint64_t hw_if_read(void *opaque, hwaddr offset,
                           unsigned size)
{
    rw_packet_t rw_packet;
    HW_IFState *s = (HW_IFState *)opaque;

    DEBUG_PRINT("READ ACCESS\n");
    
    rw_packet.offset = offset;
    rw_packet.data = 0;
    rw_packet.we = 0;
    hw_if_dut_read(s,&rw_packet);
    
    return rw_packet.data;
    
}

static void hw_if_dut_write(void *opaque , rw_packet_t *rw_packet){
    int ret;
    DEBUG_PRINT();
    HW_IFState *s = (HW_IFState *)opaque;

    ret = send(s->sock,rw_packet, sizeof(*rw_packet), 0);
    if (ret < 1) {
	perror("[hw_if] write error");
        exit(EXIT_FAILURE);
    } 
    DEBUG_PRINT("send::ret = %d\n",ret);
    
    //memset(rw_packet, 0, sizeof(*rw_packet));
    //ret = recv(s->sock, rw_packet, sizeof(*rw_packet), 0);

}

static void hw_if_write(void *opaque, hwaddr offset,
                        uint64_t value, unsigned size)
{
    rw_packet_t rw_packet;
    
    DEBUG_PRINT("%s : %d\n",__FILE__,__LINE__);
    DEBUG_PRINT("WRITE ACCESS\n");
    HW_IFState *s = (HW_IFState *)opaque;
    
    rw_packet.offset = offset;
    rw_packet.data = value;
    rw_packet.we = 1;
    DEBUG_PRINT("offset = 0x%x,value = 0x%x\n",offset,value);
    hw_if_dut_write(s,&rw_packet);
}

static const MemoryRegionOps hw_if_ops = {
    .read = hw_if_read,
    .write = hw_if_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static void hw_if_init_socket(void* opaque){
    
    struct sockaddr_in addr;
    int on;
    int len;
    int ret;
    struct timeval socket_timeout;
    struct sockaddr_in client;
    fd_set readfds;
    HW_IFState *s = (HW_IFState *)opaque;
    DEBUG_PRINT("%s : %d\n",__FILE__,__LINE__);
    /*socket*/
    s->sock0 = socket(AF_INET, SOCK_STREAM, 0);
    if (s->sock0 < 0) {
	perror("[hw_if] socket error");
    }
    bzero((char *)&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(15785);
    addr.sin_addr.s_addr = INADDR_ANY;

    /* TIME_WAIT 回避*/
    on = 1;
    setsockopt( s->sock0, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );
    
    /*bind*/
    if (bind(s->sock0, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
    	perror("[hw_if] bind error");
	exit(EXIT_FAILURE);
    }
        
    /*listen*/
    if (listen(s->sock0, 5) != 0) {
	perror("[hw_if] listen error");
	exit(EXIT_FAILURE);
    }

    printf("[hw_if] Socket is listening for connections... Please start the rtl simulation.\n");
    printf("        Timeout after 15 sec \n");

    /*Time Out*/
    socket_timeout.tv_sec = 15;
    socket_timeout.tv_usec = 0;
    FD_ZERO(&readfds);
    FD_SET(s->sock0, &readfds);
    DEBUG_PRINT("%s : %d\n",__FILE__,__LINE__);
    ret = select((s->sock0)+1 , &readfds , NULL , NULL , &socket_timeout );
    DEBUG_PRINT("%s : %d : ret = %d\n",__FILE__,__LINE__,ret);
    if (ret == 0) {
        printf("[hw_if] connection timeout!\n");
        exit(EXIT_FAILURE);
    }
    if ( FD_ISSET( s->sock0 , &readfds ) ){
        len = sizeof(client);
        DEBUG_PRINT("%s : %d\n",__FILE__,__LINE__);
        s->sock = accept(s->sock0,(struct sockaddr *)&client, &len);
        //close(sock);
        DEBUG_PRINT("%s : %d\n",__FILE__,__LINE__);
    }
}


static void hw_if_init(Object *obj)
{
    
    DEBUG_PRINT("%s : %d\n",__FILE__,__LINE__);
    SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
    HW_IFState *s = HW_IF(obj);

    memory_region_init_io(&s->iomem, OBJECT(s), &hw_if_ops, s, "hw_if", 0x1000);
    sysbus_init_mmio(sbd, &s->iomem);
    sysbus_init_irq(sbd, &s->irq);
    
    hw_if_init_socket(s);

    //s->id = hw_if_id_arm;
}

#if 0

static void hw_if_realize(DeviceState *dev, Error **errp)
{
    DEBUG_PRINT("%s : %d\n",__FILE__,__LINE__);
}

#endif

static void hw_if_class_init(ObjectClass *oc, void *data)
{
    
    DEBUG_PRINT("%s : %d\n",__FILE__,__LINE__);
    //DeviceClass *dc = DEVICE_CLASS(oc);

    //dc->realize = hw_if_realize;
    //dc->vmsd = &vmstate_hw_if;
    //dc->vmsd = NULL;
}

static const TypeInfo hw_if_arm_info = {
    .name          = TYPE_HW_IF,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(HW_IFState),
    .instance_init = hw_if_init,
    .class_init    = hw_if_class_init,
};

static void hw_if_register_types(void)
{
    type_register_static(&hw_if_arm_info);
}

type_init(hw_if_register_types)

