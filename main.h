#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>

#define SOCKBUF 512

#include "str.h"
#include "config.h"
#include "msg.h"
#include "log.h"
#include "mysql.h"
#include "dcc.h"
#include "message_processing.h"