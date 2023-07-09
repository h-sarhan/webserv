/**
 * @file network.hpp
 * @author Mehrin Firdousi (mehrinfirdousi@gmail.com)
 * @brief This file contains all the necessary headers for networking
 * @date 2023-07-09
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <arpa/inet.h>
#include <cstring>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>

#define QUEUE_LIMIT 10
#define PORT        "1234"
#define LONG_MSG                                                               \
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Aenean \
venenatis dolor neque. Maecenas vel magna id nibh tristique aliquam.\
Etiam venenatis elit ultrices, iaculis nulla ut, gravida ligula. Sed\
at rhoncus turpis. Curabitur suscipit augue quis nisi vulputate, ac\
porttitor arcu ornare. Nullam suscipit maximus felis sit amet\
bibendum. Etiam nec tincidunt neque. Nunc purus urna, mollis sit amet\
accumsan et, finibus ut velit. Mauris suscipit ac augue a volutpat.\
Nulla a nulla non enim consequat pharetra. Nunc ut pharetra leo. Sed\
eu varius ante, sed aliquam massa. Cras placerat lorem sit amet leo\
euismod rhoncus. Donec fermentum est dui, vel imperdiet leo dictum\
sed. Nam tincidunt purus at porttitor viverra. Integer porta lacinia\
gravida. Curabitur maximus nibh sem, eu ullamcorper lacus hendrerit\
non. Ut lorem libero, posuere et massa id, ornare pulvinar dui.\
Aliquam sodales nisl neque. Donec convallis mi nec elit suscipit\
rhoncus. Maecenas scelerisque, ipsum non vestibulum placerat, orci\
enim porta nibh, sed lacinia odio orci ut urna. In eu nulla euismod,\
fringilla nulla non, scelerisque massa. Curabitur leo eros, iaculis\
quis pulvinar sed, dictum id diam. Aenean ultrices tellus a enim\
aliquam varius. Nam semper urna massa, eu dictum lacus ornare id. Nam\
in nunc sem. Praesent molestie eget massa eget suscipit. Curabitur\
non lorem a turpis feugiat sagittis. In nulla ante, porta sed\
tincidunt eu, vestibulum sit amet mi. Sed et neque mi. Sed at urna\
lacinia, suscipit enim vel, posuere tellus. Nunc in imperdiet dolor,\
in fermentum eros. Sed blandit nisi a ultrices venenatis. Aenean ut\
purus nec enim volutpat tristique sit amet eu orci. Morbi consectetur\
velit vel massa rhoncus sollicitudin. In nisi tellus, pretium sed\
libero vel, consequat condimentum quam. Proin interdum tellus sed\
massa pharetra lobortis. Sed vel elementum lorem, vitae tincidunt\
leo. Orci varius natoque penatibus et magnis dis parturient montes,\
nascetur ridiculus mus. Duis non ipsum ornare accumsan"

// extern bool quit;

#endif
