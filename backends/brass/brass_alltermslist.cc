/* brass_alltermslist.cc: A termlist containing all terms in a brass database.
 *
 * Copyright (C) 2005,2007,2008,2009 Olly Betts
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

#include "brass_alltermslist.h"
#include "brass_postlist.h"

#include "pack.h"
#include "stringutils.h"

void
BrassAllTermsList::read_termfreq_and_collfreq() const
{
    DEBUGCALL(DB, void, "BrassAllTermsList::read_termfreq_and_collfreq", "");
    Assert(!current_term.empty());
    Assert(!at_end());

    // Unpack the termfreq and collfreq from the tag.  Only do this if
    // one or other is actually read.
    cursor->read_tag();
    const char *p = cursor->current_tag.data();
    const char *pend = p + cursor->current_tag.size();
    BrassPostList::read_number_of_entries(&p, pend, &termfreq, &collfreq);
}

BrassAllTermsList::~BrassAllTermsList()
{
    DEBUGCALL(DB, void, "~BrassAllTermsList", "");
    delete cursor;
}

string
BrassAllTermsList::get_termname() const
{
    DEBUGCALL(DB, string, "BrassAllTermsList::get_termname", "");
    Assert(!current_term.empty());
    Assert(!at_end());
    RETURN(current_term);
}

Xapian::doccount
BrassAllTermsList::get_termfreq() const
{
    DEBUGCALL(DB, Xapian::doccount, "BrassAllTermsList::get_termfreq", "");
    Assert(!current_term.empty());
    Assert(!at_end());
    if (termfreq == 0) read_termfreq_and_collfreq();
    RETURN(termfreq);
}

Xapian::termcount
BrassAllTermsList::get_collection_freq() const
{
    DEBUGCALL(DB, Xapian::termcount, "BrassAllTermsList::get_collection_freq", "");
    Assert(!current_term.empty());
    Assert(!at_end());
    if (termfreq == 0) read_termfreq_and_collfreq();
    RETURN(collfreq);
}

TermList *
BrassAllTermsList::next()
{
    DEBUGCALL(DB, TermList *, "BrassAllTermsList::next", "");
    Assert(!at_end());
    // Set termfreq to 0 to indicate no termfreq/collfreq have been read for
    // the current term.
    termfreq = 0;

    while (true) {
	cursor->next();
	if (cursor->after_end()) {
	    current_term.resize(0);
	    RETURN(NULL);
	}

	const char *p = cursor->current_key.data();
	const char *pend = p + cursor->current_key.size();
	if (!unpack_string_preserving_sort(&p, pend, current_term)) {
	    throw Xapian::DatabaseCorruptError("PostList table key has unexpected format");
	}

	// If this key is for the first chunk of a postlist, we're done.
	// Otherwise we need to skip past continuation chunks until we find the
	// first chunk of the next postlist.
	if (p == pend) break;
    }

    if (!startswith(current_term, prefix)) {
	// We've reached the end of the prefixed terms.
	cursor->to_end();
	current_term.resize(0);
    }

    RETURN(NULL);
}

TermList *
BrassAllTermsList::skip_to(const string &term)
{
    DEBUGCALL(DB, TermList *, "BrassAllTermsList::skip_to", term);
    Assert(!at_end());
    // Set termfreq to 0 to indicate no termfreq/collfreq have been read for
    // the current term.
    termfreq = 0;

    string key = pack_brass_postlist_key(term);
    if (cursor->find_entry_ge(key)) {
	// The exact term we asked for is there, so just copy it rather than
	// wasting effort unpacking it from the key.
	current_term = term;
    } else {
	if (cursor->after_end()) {
	    current_term.resize(0);
	    RETURN(NULL);
	}

	const char *p = cursor->current_key.data();
	const char *pend = p + cursor->current_key.size();
	if (!unpack_string_preserving_sort(&p, pend, current_term)) {
	    throw Xapian::DatabaseCorruptError("PostList table key has unexpected format");
	}
    }

    if (!startswith(current_term, prefix)) {
	// We've reached the end of the prefixed terms.
	cursor->to_end();
	current_term.resize(0);
    }

    RETURN(NULL);
}

bool
BrassAllTermsList::at_end() const
{
    DEBUGCALL(DB, bool, "BrassAllTermsList::at_end", "");
    RETURN(cursor->after_end());
}
