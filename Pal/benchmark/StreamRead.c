/* -*- mode:c; c-file-style:"k&r"; c-basic-offset: 4; tab-width:4; indent-tabs-mode:nil; mode:auto-fill; fill-column:78; -*- */
/* vim: set ts=4 sw=4 et tw=78 fo=cqt wm=0: */

#include "pal.h"
#include "pal_debug.h"
#include "bench.h"

static void
do_read(PAL_HANDLE stream, const char * filename)
{
    char c;

    if (DkStreamRead(stream, 0, 1, &c, NULL, 0) != 1) {
        pal_printf("DkStreamRead failed on %s\n", filename);
        return;
    }
}

int main (int argc, char ** argv, char ** envp)
{
    const char * uri = argv[1] ? argv[1] : "file:/dev/zero";

    PAL_HANDLE stream = DkStreamOpen(uri, PAL_ACCESS_RDONLY, 0, 0, 0);
    if (!stream) {
        pal_printf("Failed opening %s\n", uri);
        return 1;
    }

    BENCH(do_read(stream, uri), 0);;
    micro("DkStreamRead", get_n());
    DkObjectClose(stream);
    return 0;
}
