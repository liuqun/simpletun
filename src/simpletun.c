/**************************************************************************
 * simpletun.c                                                            *
 *                                                                        *
 * A simplistic, simple-minded, naive tunnelling program using tun/tap    *
 * interfaces and TCP. DO NOT USE THIS PROGRAM FOR SERIOUS PURPOSES.      *
 *                                                                        *
 * You have been warned.                                                  *
 *                                                                        *
 * (C) 2010 Davide Brini.                                                 *
 *                                                                        *
 * DISCLAIMER AND WARNING: this is all work in progress. The code is      *
 * ugly, the algorithms are naive, error checking and input validation    *
 * are very basic, and of course there can be bugs. If that's not enough, *
 * the program has not been thoroughly tested, so it might even fail at   *
 * the few simple things it should be supposed to do right.               *
 * Needless to say, I take no responsibility whatsoever for what the      *
 * program might do. The program has been written mostly for learning     *
 * purposes, and can be used in the hope that is useful, but everything   *
 * is to be taken "as is" and without any kind of warranty, implicit or   *
 * explicit. See the file LICENSE for further details.                    *
 *************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <linux/if_tun.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>
#include <errno.h>
#include <stdarg.h>

/* buffer for reading from tun/tap interface, must be >= 1500 */
#define BUFSIZE 2000
#define CLIENT 0
#define SERVER 1
#define PORT 55555

int debug;
char *progname;
/**************************************************************************
 * do_debug: prints debugging stuff (doh!)                                *
 **************************************************************************/
void do_debug(char *msg, ...)
{
    va_list argp;

    if (debug) {
        va_start(argp, msg);
        vfprintf(stderr, msg, argp);
        va_end(argp);
    }
}

/**************************************************************************
 * my_err: prints custom error messages on stderr.                        *
 **************************************************************************/
void my_err(char *msg, ...)
{
    va_list argp;

    va_start(argp, msg);
    vfprintf(stderr, msg, argp);
    va_end(argp);
}

/**************************************************************************
 * tun_alloc: allocates or reconnects to a tun/tap device. The caller     *
 *            must reserve enough space in *dev.                          *
 **************************************************************************/
int tun_alloc()
{
    struct ifreq ifr;
    int tun_fd, err;

    char* tun_file_name = "/dev/net/tun";

    if ((tun_fd = open(tun_file_name, O_RDWR)) < 0) {
        perror("Open failed ***/dev/net/tun");
        return tun_fd;
    }

    memset(&ifr, 0, sizeof(ifr));

    ifr.ifr_flags = IFF_NO_PI | IFF_TUN | IFF_TAP;

    //memset(ifr.ifr_name, 0, IFNAMSIZ);
    memcpy(ifr.ifr_name, "tun0\0aabbccddaabbccdd", IFNAMSIZ);

    if ((err = ioctl(tun_fd, TUNSETIFF, (void *) &ifr)) < 0) {
        perror("ioctl(TUNSETIFF)");
        close(tun_fd);
        return err;
    }

    return tun_fd;
}

int tun_read(int tap_id)
{
    int nread = 0;

    static long tap2net  = 0;

    char read_buffer[4096];
    int read_buffer_size = sizeof(read_buffer);

    fd_set read_set;
    FD_ZERO(&read_set);
    FD_SET(tap_id, &read_set);

    if (select(tap_id + 1, &read_set, NULL, NULL, NULL) < 0) {
        perror("select()");
        exit(1);
    }

    if (FD_ISSET(tap_id, &read_set)) {
        /* data from tun/tap: just read it and write it to the network */
        nread = read(tap_id, read_buffer, read_buffer_size);

        tap2net++;
        do_debug("TAP2NET %lu: Read %d bytes from the tap interface\n",
                 tap2net, nread);
    }

    return nread;
}

int udp_socket()
{
    int sock_id;

    if ((sock_id = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket()");
        exit(1);
    }

    return sock_id;
}

void udp_send(int sock_id)
{
    char* remote_ip  = "192.168.1.16";
    unsigned short int remote_port = 55555;
    char content_write_buffer[50];
    char length_write_buffer[2] = {0x00, 0x32};

    struct sockaddr_in remote_socket_addr;

    memset(&remote_socket_addr, 0, sizeof(remote_socket_addr));
    remote_socket_addr.sin_family = AF_INET;
    remote_socket_addr.sin_addr.s_addr = inet_addr(remote_ip);
    remote_socket_addr.sin_port = htons(remote_port);

    memset(content_write_buffer, 0x41, sizeof(content_write_buffer));
    sendto(sock_id, length_write_buffer, sizeof(length_write_buffer), 0, (void*)(&remote_socket_addr), sizeof(remote_socket_addr));
    sendto(sock_id, content_write_buffer, sizeof(content_write_buffer), 0, (void*)(&remote_socket_addr), sizeof(remote_socket_addr));
}

int main(int argc, char *argv[])
{
    int tap_id = tun_alloc();
    while(1) {
        tun_read(tap_id);
    }

    return (0);
}
