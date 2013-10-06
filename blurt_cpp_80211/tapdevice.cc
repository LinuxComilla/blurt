#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <string>
#include <cstdlib>
#include <sys/ioctl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include "tapdevice.h"

using namespace std;

TapDevice::TapDevice( std::string name )
  : fd_()
{
    struct ifreq ifr;
    int err, sockfd;

    if ( ( fd_ = open( "/dev/net/tun", O_RDWR ) ) < 0 ) {
        throw "open";
    }

    // fills enough of memory area pointed to by ifr
    memset( &ifr, 0, sizeof( ifr ) );

    // specifies to create tap device
    ifr.ifr_flags = IFF_TAP;


    // uses specified name for tap device
    strncpy( ifr.ifr_name, name.c_str(), IFNAMSIZ );

    // create interface
    if ( ( err = ioctl( fd_, TUNSETIFF, ( void * ) &ifr ) ) < 0 ) {
        close( fd_ );
        throw "ioctl failed.  You probably forgot to sudo.";
    }

    // add flag to bring interface up
    ifr.ifr_flags += IFF_UP;

    sockfd = socket( AF_INET, SOCK_DGRAM, 0 );

    if ( ( err = ioctl( sockfd, SIOCSIFFLAGS, ( void * ) &ifr ) ) < 0 ) {
        close( sockfd );
        throw "ioctl";
    }
    close( sockfd );
}

TapDevice::~TapDevice()
{
    if ( close( fd_ ) < 0 ) {
	throw "close";
    }
}
