/** @file xapian-replicate.cc
 * @brief Replicate a database from a master server to a local copy.
 */
/* Copyright (C) 2008 Olly Betts
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301
 * USA
 */

#include <config.h>

#include "replicatetcpclient.h"

#include <xapian.h>

#include "gnu_getopt.h"
#include "stringutils.h"
#include "safeunistd.h"

#include <iostream>

using namespace std;

#define PROG_NAME "xapian-replicate"
#define PROG_DESC "Replicate a database from a master server to a local copy"

#define OPT_HELP 1
#define OPT_VERSION 2

// Wait DEFAULT_INTERVAL seconds between updates unless --interval is passed.
#define DEFAULT_INTERVAL 60

static void show_usage() {
    cout << "Usage: "PROG_NAME" [OPTIONS] DATABASE\n\n"
"Options:\n"
"  -h, --host=HOST   host to connect to\n"
"  -p, --port=PORT   port to connect to\n"
"  -m, --master=DB   replicate database DB from the master\n"
"  -i, --interval=N  wait N seconds between each connection to the master\n"
"                    (default: "STRINGIZE(DEFAULT_INTERVAL)")\n"
"  -o, --one-shot    replicate only once and then exit\n"
"  -v, --verbose     be more verbose\n"
"  --help            display this help and exit\n"
"  --version         output version information and exit" << endl;
}

int
main(int argc, char **argv)
{
    const char * opts = "h:p:m:i:ov";
    const struct option long_opts[] = {
	{"host",	required_argument,	0, 'h'},
	{"port",	required_argument,	0, 'p'},
	{"master",	required_argument,	0, 'm'},
	{"interval",	required_argument,	0, 'i'},
	{"one-shot",	no_argument,		0, 'o'},
	{"verbose",	no_argument,		0, 'v'},
	{"help",	no_argument, 0, OPT_HELP},
	{"version",	no_argument, 0, OPT_VERSION},
	{NULL,		0, 0, 0}
    };

    string host;
    int port = 0;
    string masterdb;
    int interval = DEFAULT_INTERVAL;
    bool one_shot = false;
    bool verbose = false;

    int c;
    while ((c = gnu_getopt_long(argc, argv, opts, long_opts, 0)) != -1) {
	switch (c) {
	    case 'h':
		host.assign(optarg);
		break;
	    case 'p':
		port = atoi(optarg);
		break;
	    case 'm':
		masterdb.assign(optarg);
		break;
	    case 'i':
		interval = atoi(optarg);
		break;
	    case 'o':
		one_shot = true;
		break;
	    case 'v':
		verbose = true;
		break;
	    case OPT_HELP:
		cout << PROG_NAME" - "PROG_DESC"\n\n";
		show_usage();
		exit(0);
	    case OPT_VERSION:
		cout << PROG_NAME" - "PACKAGE_STRING << endl;
		exit(0);
	    default:
		show_usage();
		exit(1);
	}
    }

    if (argc - optind != 1) {
	show_usage();
	exit(1);
    }

    // Path to the database to create/update.
    string dbpath(argv[optind]);

    while (true) {
	try {
	    if (verbose) {
		cout << "Connecting to " << host << ":" << port << endl;
	    }
	    ReplicateTcpClient client(host, port, 10000);
	    if (verbose) {
		cout << "Getting update for " << dbpath << " from "
		     << masterdb << endl;
	    }
	    Xapian::ReplicationInfo info;
	    client.update_from_master(dbpath, masterdb, info);
	    if (verbose) {
		cout << "Update complete: " <<
			info.fullcopy_count << " copies, " <<
			info.changeset_count << " changesets, " <<
			(info.changed ? "new live database" : "no changes to live database") <<
			endl;
	    }
	} catch (const Xapian::NetworkError &error) {
	    // Don't stop running if there's a network error - just log to
	    // stderr and retry at next timeout.  This should make the client
	    // robust against temporary network failures.
	    cerr << argv[0] << ": " << error.get_description() << endl;

	    // If we were running as a one-shot client though, we're going to
	    // exit anyway, so let's make the return value reflect that there
	    // was a failure.
	    if (one_shot)
		exit(1);
	} catch (const Xapian::Error &error) {
	    cerr << argv[0] << ": " << error.get_description() << endl;
	    exit(1);
	} catch (const exception &e) {
	    cerr << "Caught standard exception: " << e.what() << endl;
	    exit(1);
	} catch (...) {
	    cerr << "Caught unknown exception" << endl;
	    exit(1);
	}
	if (one_shot) break;
	sleep(interval);
    }
}
