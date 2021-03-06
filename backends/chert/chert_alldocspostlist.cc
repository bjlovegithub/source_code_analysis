/** @file chert_alldocspostlist.cc
 * @brief A PostList which iterates over all documents in a ChertDatabase.
 */
/* Copyright (C) 2006,2007,2008,2009 Olly Betts
 * Copyright (C) 2008 Lemur Consulting Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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

#include <string>

#include "chert_alldocspostlist.h"
#include "chert_database.h"

#include "utils.h"

using namespace std;

ChertAllDocsPostList::ChertAllDocsPostList(Xapian::Internal::RefCntPtr<const ChertDatabase> db_,
					   Xapian::doccount doccount_)
	: ChertPostList(db_, string(), true),
	  doccount(doccount_)
{
    DEBUGCALL(DB, void, "ChertAllDocsPostList::ChertAllDocsPostList", db_.get() << ", " << doccount_);
}

Xapian::doccount
ChertAllDocsPostList::get_termfreq() const
{
    DEBUGCALL(DB, Xapian::doccount, "ChertAllDocsPostList::get_termfreq", "");
    RETURN(doccount);
}

Xapian::termcount
ChertAllDocsPostList::get_doclength() const
{
    DEBUGCALL(DB, Xapian::termcount, "ChertAllDocsPostList::get_doclength", "");

    RETURN(ChertPostList::get_wdf());
}

Xapian::termcount
ChertAllDocsPostList::get_wdf() const
{
    DEBUGCALL(DB, Xapian::termcount, "ChertAllDocsPostList::get_wdf", "");
    AssertParanoid(!at_end());
    RETURN(1);
}

PositionList *
ChertAllDocsPostList::read_position_list()
{
    DEBUGCALL(DB, Xapian::termcount, "ChertAllDocsPostList::read_position_list", "");
    throw Xapian::InvalidOperationError("ChertAllDocsPostList::read_position_list() not meaningful");
}

PositionList *
ChertAllDocsPostList::open_position_list() const
{
    DEBUGCALL(DB, Xapian::termcount, "ChertAllDocsPostList::open_position_list", "");
    throw Xapian::InvalidOperationError("ChertAllDocsPostList::open_position_list() not meaningful");
}

string
ChertAllDocsPostList::get_description() const
{
    string desc = "ChertAllDocsPostList(did=";
    desc += om_tostring(get_docid());
    desc += ",doccount=";
    desc += om_tostring(doccount);
    desc += ')';
    return desc;
}
