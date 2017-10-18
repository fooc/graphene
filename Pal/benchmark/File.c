/* -*- mode:c; c-file-style:"k&r"; c-basic-offset: 4; tab-width:4; indent-tabs-mode:nil; mode:auto-fill; fill-column:78; -*- */
/* vim: set ts=4 sw=4 et tw=78 fo=cqt wm=0: */

#include "api.h"
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

static void
do_write(PAL_HANDLE stream, const char * filename)
{
    char c = 1;

    if (DkStreamWrite(stream, 0, 1, &c, NULL) != 1) {
        pal_printf("DkStreamWrite failed on %s\n", filename);
        return;
    }
}

static void
do_open(const char * filename)
{
    PAL_HANDLE stream;

    stream = DkStreamOpen(filename, PAL_ACCESS_RDONLY, 0, 0, 0);
    if (!stream) {
        pal_printf("DkStreamOpen failed on %s\n", filename);
        return;
    }

    DkObjectClose(stream);
}

static void
do_attrquery(const char * filename)
{
    PAL_STREAM_ATTR attr;
    PAL_BOL result;

    result = DkStreamAttributesQuery(filename, &attr);
    if (result == PAL_FALSE) {
        pal_printf("DkStreamAttributesQuery failed on %s\n", filename);
        return;
    }
}

static void
do_attrquerybyhdl(PAL_HANDLE stream, const char * filename)
{
    PAL_STREAM_ATTR attr;
    PAL_BOL result;

    result = DkStreamAttributesQuerybyHandle(stream, &attr);
    if (result == PAL_FALSE) {
        pal_printf("DkStreamAttributesQuerybyHandle failed on %s\n", filename);
        return;
    }
}

int main (int argc, char ** argv, char ** envp)
{
    const char * uri;
    PAL_HANDLE stream;

    if (argc < 2) goto usage;
    uri = argv[2] ? argv[2] : "file:/dev/zero";

    if (memcmp(uri, "file:", 5)) {
        pal_printf("Only file uri is allowed.\n");
        return 1;
    }

    if (strcmp_static(argv[1], "read")) {
        stream = DkStreamOpen(uri, PAL_ACCESS_RDWR, 0, 0, 0);
        if (!stream) {
            pal_printf("Failed opening %s\n", uri);
            return 1;
        }
        BENCH(do_read(stream, uri), 0);;
        micro("DkStreamRead", get_n());
        DkObjectClose(stream);
    } else if (strcmp_static(argv[1], "write")) {
        stream = DkStreamOpen(uri, PAL_ACCESS_RDWR, PAL_SHARE_OWNER_W,
                              PAL_CREAT_TRY, 0);
        if (!stream) {
            pal_printf("Failed opening %s\n", uri);
            return 1;
        }
        BENCH(do_write(stream, uri), 0);;
        micro("DkStreamWrite", get_n());
        DkObjectClose(stream);
    } else if (strcmp_static(argv[1], "open")) {
        BENCH(do_open(uri), 0);;
        micro("DkStreamOpen", get_n());
    } else if (strcmp_static(argv[1], "attrquery")) {
        BENCH(do_attrquery(uri), 0);;
        micro("DkStreamAttributesQuery", get_n());
    } else if (strcmp_static(argv[1], "attrquerybyhdl")) {
        stream = DkStreamOpen(uri, PAL_ACCESS_RDONLY, 0, 0, 0);
        if (!stream) {
            pal_printf("Failed opening %s\n", uri);
            return 1;
        }
        BENCH(do_attrquerybyhdl(stream, uri), 0);;
        micro("DkStreamAttributesQeurybyHandle", get_n());
        DkObjectClose(stream);
    } else {
usage:
        pal_printf("Usage: pal_loader %s read|write|open|attrquery|attrquerybyhdl\n", argv[0]);
    }

    return 0;
}
