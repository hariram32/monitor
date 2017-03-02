#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <mysql/mysql.h>
#include <time.h>
#include <string.h>

#define COUNT_DONE  10
#define COUNT_HALT1  3
#define COUNT_HALT2  6
#define NUM_THREADS	8
#define PACKETSIZE	64
#define MAX_HOSTS 4000
#define MAX_CSV_COLUMNS 10
#define MAX_CSV_LINE_LENGTH 400
#define MAX_CSV_ENTRY_LENGTH 40
#define SOCK_PATH "/dev/socket/echo_socket"
#define PING_TIMEOUT 20000
#define MAX_SQL_QUERY_LENGTH 1000
#define MAX_LONG_SQL_QUERY_LENGTH 100000
#define MAX_MULTI_STATEMENT_SQL_QUERY_LENGTH 100000
#define MAX_SQL_VALUES_LENGTH 50000
#define HOSTS_DATA_PATH "/usr/bin/monitor/hosts.csv"
#define REINITIALISE_TIMEOUT 20000000
#define MAX_CONSECUTIVE_DIFFERENTIAL_DATABASE_UPDATES 50

#include <initialise.h>
#include <process.h>
#include <database.h>
#include <network.h>
#include <file.h>

struct packet
{
	struct icmphdr hdr;
	char msg [ PACKETSIZE - sizeof ( struct icmphdr ) ];
};

typedef enum HOST_STATUS
{
	uninitialised,
	up,
	unreachable,
	timed_out,
	not_found,
	just_lost_contact
} host_status;

typedef enum PING_STATUS
{
	sending,
	sent
} ping_status;


struct host_info
{
	char canonical_name [ 64 ];
	char ip [ 16 ];
	unsigned short send_count;
	unsigned short reply_count;
	host_status previous_status;
	host_status status;
	ping_status ping_status;
	struct addrinfo *address_info;
	clock_t clock_at_send;
};

extern char g_host_data [ MAX_HOSTS ][ MAX_CSV_COLUMNS ][ MAX_CSV_ENTRY_LENGTH ];

extern int g_sd_send;
extern int g_sd_receive;
extern struct host_info g_hosts [ MAX_HOSTS ];
extern int g_host_count;
extern MYSQL *g_conn;
extern clock_t g_reinitialise_timer;
extern int g_consecutive_database_updates;
