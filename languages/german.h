/* This file was generated automatically by the Snowball to ISO C++ compiler */

#include "steminternal.h"

namespace Xapian {

class InternalStemGerman : public Stem::Internal {
    int I_x;
    int I_p2;
    int I_p1;
  public:
    int r_standard_suffix();
    int r_R2();
    int r_R1();
    int r_mark_regions();
    int r_postlude();
    int r_prelude();

    InternalStemGerman();
    ~InternalStemGerman();
    int stem();
    const char * get_description() const;
};

}
