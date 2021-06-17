#include "kyotocabinet/kchashdb.h"
#include "river_cache.h"
#include "rivfs.h"

using namespace std;
using namespace kyotocabinet;

int cache_initial(struct riv_live *plive, char *location) {
    char file_name[MAX_PATH * 3];
    int rval = 0;
    /* create the database object */
    RIVER_PATH(file_name, location, RIV_INDE_DEFAULT);
    // open the database
    if (!plive->rivdb.open(file_name, TreeDB::OWRITER | TreeDB::OCREATE)) {
        cerr << "open error: " << plive->rivdb.error().name() << endl;
        rval = 0x01;
    }
    //
    return rval;
}

int cache_final(struct riv_live *plive) {
    // close the database
    if (!plive->rivdb.close()) {
        cerr << "close error: " << plive->rivdb.error().name() << endl;
    }
    //
    return 0x00;
}

