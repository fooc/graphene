/* -*- mode:c; c-file-style:"k&r"; c-basic-offset: 4; tab-width:4; indent-tabs-mode:nil; mode:auto-fill; fill-column:78; -*- */
/* vim: set ts=4 sw=4 et tw=78 fo=cqt wm=0: */

#include "api.h"
#include "pal.h"
#include "pal_debug.h"
#include "bench.h"

#define PORT_BASE       8000
#define NCHILDREN       8

static void doit (int n, PAL_HANDLE * handles)
{
    PAL_HANDLE sock, newsock;

    sock = DkObjectsWaitAny(n, handles, NO_TIMEOUT);
    if (!sock) {
        pal_printf("DkObjectsWaitAny failed\n");
        DkProcessExit(1);
    }

    newsock = DkStreamWaitForClient(sock);
    if (!newsock) {
        pal_printf("lat_select: Could not accept tcp connection\n");
        DkProcessExit(1);
    }

    DkObjectClose(newsock);
}

int main (int argc, const char ** argv, const char ** envp)
{
    char buf[256];
    const char * report = "DkObjectsWaitAny on %d tcp sockets";
    const char * usage = "pal_loader lat_select n\n";
    PAL_HANDLE children[NCHILDREN];
    PAL_HANDLE * handles;
    int nhandles, i;
    char c;
    const char * newargv[4];

    if (argc != 2 && argc != 3) {
        pal_printf(usage);
        DkProcessExit(1);
    }

    nhandles = bytes(argv[1]);
    handles = __alloca(sizeof(PAL_HANDLE) * nhandles);

    if (argv[2]) {
        if (!strcmp_static(argv[2], "-c"))
            DkProcessExit(1);

        if (DkStreamWrite(pal_control.parent_process, 0, 1, &c, NULL) != 1)
            DkProcessExit(1);

        i = 0;
        for (;; i = (i + 1) % nhandles) {
            snprintf(buf, 256, "tcp:127.0.0.1:%d", PORT_BASE + i);
            handles[i] = DkStreamOpen(buf, PAL_ACCESS_RDWR, 0, 0, 0);
            if (!handles[i])
                DkProcessExit(0);
            DkStreamRead(handles[i], 0, 1, &c, NULL, 0);
            DkObjectClose(handles[i]);
        }

        DkStreamRead(pal_control.parent_process, 0, 1, &c, NULL, 0);
        DkProcessExit(0);
    }

    for (i = 0; i < nhandles; i++) {
        snprintf(buf, 256, "tcp.srv:127.0.0.1:%d", PORT_BASE + i);
        handles[i] = DkStreamOpen(buf, 0, 0, 0, 0);
        if (!handles[i]) {
            pal_printf("lat_select: Could not open tcp server socket\n");
            DkProcessExit(1);
        }
    }

    newargv[0] = argv[0];
    newargv[1] = argv[1];
    newargv[2] = "-c";
    newargv[3] = NULL;

    for (i = 0; i < NCHILDREN; i++) {
        children[i] = DkProcessCreate(argv[0], 0, newargv);

        if (!children[i]) {
            pal_printf("DkProcessCreate failed\n");
            return 1;
        }

        if (DkStreamRead(children[i], 0, 1, &c, NULL, 0) != 1) {
            pal_printf("Wait for child failed\n");
            return 1;
        }
    }

    BENCH(doit(nhandles, handles), 0);
    snprintf(buf, 256, report, nhandles);
    micro(buf, get_n());

    for (i = 0; i < nhandles; i++)
        DkObjectClose(handles[i]);

    for (i = 0; i < NCHILDREN; i++)
        DkObjectClose(children[i]);

    return 0;
}
