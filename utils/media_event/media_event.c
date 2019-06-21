/*
 * This file and its contents are supplied under the terms of the
 * Common Development and Distribution License ("CDDL"), version 1.0.
 * You may only use this file in accordance with the terms of version
 * 1.0 of the CDDL.
 *
 * A full copy of the text of the CDDL should have accompanied this
 * source.  A copy of the CDDL is also available via the Internet at
 * http://www.illumos.org/license/CDDL.
 */

/*
 * Copyright 2019 Alexander Pyhalov
 */

/*
 * Detect changes of mtab and send SIGUSR2 to specified pid.
 */

#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <port.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mnttab.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <unistd.h>

#define	BUFLEN	MNT_LINE_MAX+1

int
main(int argc, char *argv[])
{
	pid_t pid = 0;
	char *end, *progname;
	int port;
	port_event_t ev;
	char buf[BUFLEN];
	struct file_obj fobj;

	progname = basename(argv[0]);

	if (argc != 2) {
		fprintf(stderr, "Usage:\t%s PID\n", progname);
		return (1);
	}

	errno = 0;
	pid = (pid_t) strtol(argv[1], &end, 10);
	if (errno != 0 || *end != '\0') {
		fprintf(stderr, "%s: Argument should be a PID\n", progname);
		return (1);
	}

	port = port_create();
	if (port < 0) {
		fprintf(stderr, "%s: Couldn't start monitor for %s\n",
		    progname, MNTTAB);
		return (1);
	}

	fobj.fo_name = MNTTAB;

	if (port_associate(port, PORT_SOURCE_FILE, (uintptr_t)&fobj,
	    FILE_MODIFIED, NULL) != 0) {
		fprintf(stderr, "%s: Couldn't start monitor for %s\n",
		    progname, MNTTAB);
		close(port);
		return (1);
	}

	while (1) {
		errno = 0;

		if (port_get(port, &ev, NULL) == -1) {
			if (errno == EINTR)
				continue;
			fprintf(stderr, "%s: Error monitoring mtab changes\n",
			    progname);
			return (1);
		}

#ifdef DEBUG
		printf("Got event %d %d\n", ev.portev_source, ev.portev_events);
#endif
		kill(pid, SIGUSR2);

		if (port_associate(port, PORT_SOURCE_FILE, (uintptr_t)&fobj,
		    FILE_MODIFIED, NULL) != 0) {
			fprintf(stderr, "%s: Couldn't start monitor for %s\n",
			    progname, MNTTAB);
			close(port);
			return (1);
		}
	}

	return (0);
}
