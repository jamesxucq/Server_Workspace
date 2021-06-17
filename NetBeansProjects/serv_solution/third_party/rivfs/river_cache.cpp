#include "kyotocabinet/kchashdb.h"
#include "river_cache.h"
#include "rivfs.h"

using namespace std;
using namespace kyotocabinet;

uint32 cache_initial(struct riv_live *plive) {
    char file_name[MAX_PATH * 3];
    /* create the database object */
    plive->rivdb = new TreeDB();
    // open the database
    RIVER_PATH(file_name, plive->location, RIV_INDE_DEFAULT);
    if (!plive->rivdb->open(file_name, TreeDB::OWRITER | TreeDB::OCREATE)) {
        cerr << "open error: " << plive->rivdb->error().name() << endl;
        return 0x01;
    }
    //
    return 0x00;
}

uint32 cache_final(struct riv_live *plive) {
    // close the database
    if (!plive->rivdb->close()) {
        cerr << "close error: " << plive->rivdb->error().name() << endl;
    }
    delete plive->rivdb;
    //
    return 0x00;
}

