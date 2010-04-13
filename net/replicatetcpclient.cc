/** @file replicatetcpclient.cc
 *  @brief TCP/IP replication client class.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <config.h>

#include "replicatetcpclient.h"

#include "replication.h"

#include "omtime.h"
#include "tcpclient.h"
#include "utils.h"

using namespace std;

ReplicateTcpClient::ReplicateTcpClient(const string & hostname, int port,
				       int msecs_timeout_connect)
    : socket(open_socket(hostname, port, msecs_timeout_connect)),
      remconn(-1, socket, "")
{
}

int
ReplicateTcpClient::open_socket(const string & hostname, int port,
				int msecs_timeout_connect)
{
    return TcpClient::open_socket(hostname, port, msecs_timeout_connect, false);
}

void
ReplicateTcpClient::update_from_master(const std::string & path,
				       const std::string & masterdb,
				       Xapian::ReplicationInfo & info)
{
    Xapian::DatabaseReplica replica(path);
    remconn.send_message('R', replica.get_revision_info(), OmTime());
    remconn.send_message('D', masterdb, OmTime());
    replica.set_read_fd(socket);
    info.clear();
    Xapian::ReplicationInfo subinfo;
    while (replica.apply_next_changeset(&subinfo)) {
	sleep(30);
	info.changeset_count += subinfo.changeset_count;
	info.fullcopy_count += subinfo.fullcopy_count;
	if (subinfo.changed)
	    info.changed = true;
    }
    info.changeset_count += subinfo.changeset_count;
    info.fullcopy_count += subinfo.fullcopy_count;
    if (subinfo.changed)
	info.changed = true;
}

ReplicateTcpClient::~ReplicateTcpClient()
{
    remconn.do_close(true);
}
