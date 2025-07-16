/* toralize.c*/

#include "toralize.h"

Req *request(const char *dstip, const int dstport)
{
    Req *req;

    req = malloc(reqsize);

    req->vn = 4;
    req->cd = 1;
    req->dstport = htons(dstport);
    req->dstip = inet_addr(dstip);
    strncpy(req->userid, USERNAME, 8);

    return req;
}

int main(int argc, char *argv[])
{
    char *host;
    int port, s;
    struct sockaddr_in sock;
    Req *req;
    Res *res;
    char buf[ressize];
    int succeess;
    char tmp[512];

    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s <host> <port>\n",
                argv[0]);

        return -1;
    }

    host = argv[1];
    port = atoi(argv[2]);

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0)
    {
        perror("socket");

        return -1;
    }

    sock.sin_family = AF_INET;
    sock.sin_port = htons(PROXYPORT);
    sock.sin_addr.s_addr = inet_addr(PROXY);

    if (connect(s, (struct sockaddr *)&sock, sizeof(sock)))
    {
        perror("connect");

        return -1;
    };

    printf("Connected to proxy\n");

    req = request(host, port);
    write(s, req, reqsize);

    memset(buf, 0, ressize);
    if (read(s, buf, ressize) < 1)
    {
        perror("read");
        free(req);
        close(s);

        return -1;
    }

    res = (Res *)buf;
    succeess = (res->cd == 90);
    if (!succeess)
    {
        fprintf(stderr, "Unable to traverse the proxy, error code: %d\n", res->cd);

        close(s);
        free(req);

        return -1;
    }

    printf("Successfully connected through the proxy to %s:%d\n", host, port);

    memset(tmp, 0, 512);
    snprintf(tmp, 511,
             "HEAD / HTTP/1.0\r\n"
             "Host: www.networktechnology.org\r\n"
             "\r\n");
    write(s, tmp, strlen(tmp));

    memset(tmp, 0, 512);
    read(s, tmp, 511);
    printf("'%s'\n", tmp);

    close(s);
    free(req);

    return 0;
}

// ./toralize 1.2.3.4 80
