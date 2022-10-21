
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include "gpiosysfs.h"

#define SYSFS_GPIO_DIR "/sys/class/gpio"

GpioSysFs::GpioSysFs(GPIO_BANK bank, int number, QObject *parent) : QObject(parent)
{
    gpio = ((bank -1)*32) + number;
}

int GpioSysFs::gpio_export()
{
    int fd, len;
    char buf[MAX_BUF];

    fd = open(SYSFS_GPIO_DIR "/export", O_WRONLY);

    if (fd < 0)
    {
      perror("gpio/export");
      return fd;
    }

    len = snprintf(buf, sizeof(buf), "%d", gpio);
    write(fd, buf, len);
    close(fd);

    return 0;
}

int GpioSysFs::gpio_unexport()
{
    int fd, len;
    char buf[MAX_BUF];

    fd = open(SYSFS_GPIO_DIR "/unexport", O_WRONLY);
    if (fd < 0)
    {
      perror("gpio/export");
      return fd;
    }

    len = snprintf(buf, sizeof(buf), "%d", gpio);
    write(fd, buf, len);
    close(fd);

    return 0;
}

int GpioSysFs::gpio_set_dir(GPIO_DIR dir)
{
    int fd;
     char buf[MAX_BUF];

     snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR  "/gpio%d/direction", gpio);

     fd = open(buf, O_WRONLY);
     if (fd < 0) {
       perror("gpio/direction");
       return fd;
     }

     if(dir == GpioSysFs::GPIO_DIR::SET_OUT)
       write(fd, "out", 4);
     else
       write(fd, "in", 3);

     close(fd);
    return 0;
}

int GpioSysFs::gpio_set_value(GPIO_VAL value)
{
    int fd;
    char buf[MAX_BUF];

    snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);

    fd = open(buf, O_WRONLY);
    if (fd < 0) {
      perror("gpio/set-value");
      return fd;
    }

    if(value == GpioSysFs::GPIO_VAL::SET_HIGH)
      write(fd, "1", 2);
    else
      write(fd, "0", 2);

    close(fd);

    return 0;
}

int GpioSysFs::gpio_get_value(unsigned int &value)
{
    int fd;
    char buf[MAX_BUF];
    char ch;

    snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);

    fd = open(buf, O_RDONLY);

    if (fd < 0)
    {
      perror("gpio/get-value");
      return fd;
    }

    read(fd, &ch, 1);

    if (ch != '0') {
      value = 1;
    } else {
      value = 0;
    }

    close(fd);
    return 0;
}

int GpioSysFs::gpio_set_edge(const char *edge)
{
    int fd;
     char buf[MAX_BUF];

     snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/edge", gpio);

     fd = open(buf, O_WRONLY);
     if (fd < 0) {
       perror("gpio/set-edge");
       return fd;
     }

     write(fd, edge, strlen(edge) + 1);
     close(fd);
    return 0;
}

int GpioSysFs::gpio_fd_open()
{
    int fd;
    char buf[MAX_BUF];

    snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);

    fd = open(buf, O_RDONLY | O_NONBLOCK );
    if (fd < 0) {
      perror("gpio/fd_open");
    }
    return fd;
}

int GpioSysFs::gpio_poll(int gpio_fd, int timeout)
{
    struct pollfd fdset[1];
    int nfds = 1;
    int rc;
    char buf[MAX_BUF];

    memset((void*)fdset, 0, sizeof(fdset));

    fdset[0].fd = gpio_fd;
    fdset[0].events = POLLPRI;

    rc = poll(fdset, nfds, timeout);

    if (fdset[0].revents & POLLPRI) {
      // dummy read
      read(fdset[0].fd, buf, MAX_BUF);
    }

    return rc;
}
