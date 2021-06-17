
/*
 * AuthCtrl.c
 *
 *  Created on: 2010-3-10
 *      Author: David
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logger.h"
#include "auth_ctrl.h"

//
#define USER_COLLECTION_NAME "user"
#define SSEV_COLLECTION_NAME "ssev"

//
struct auth_config _auth_config_;
mongoc_client_t *_auth_client_;

//

int get_avalue_by_uname(struct auth_value *avalue, mongoc_client_t *client, char *dabe_name, const char *user_name) {
    mongoc_collection_t *collection;
    mongoc_cursor_t *cursor;
    bson_error_t error;
    const bson_t *doc;
    bson_t *query;
    //
    collection = mongoc_client_get_collection(client, dabe_name, USER_COLLECTION_NAME);
    query = BCON_NEW("User_name", user_name);
    cursor = mongoc_collection_find(collection, MONGOC_QUERY_NONE,
            0, 0, 0, query, NULL, NULL);
    bson_iter_t iter;
    while (mongoc_cursor_next(cursor, &doc)) {
// char *str = bson_as_json (doc, NULL); _LOG_DEBUG("%s\n", str); bson_free (str);
        if (bson_iter_init(&iter, doc)) {
            while (bson_iter_next(&iter)) {
                if (!strcmp("User_id", bson_iter_key(&iter))) {
                    avalue->uid = bson_iter_double(&iter);
// _LOG_DEBUG("avalue->uid:%d\n", avalue->uid);
                } else if (!strcmp("Password", bson_iter_key(&iter))) {
                    strcpy(avalue->password, bson_iter_utf8(&iter, NULL));
// _LOG_DEBUG("avalue->password:%s\n", avalue->password);
                } else if (!strcmp("Salt", bson_iter_key(&iter))) {
                    strcpy(avalue->_salt_, bson_iter_utf8(&iter, NULL));
// _LOG_DEBUG("avalue->_salt_:%s\n", avalue->_salt_);
                } else if (!strcmp("Auth_host", bson_iter_key(&iter))) {
                    saddr_split(&avalue->saddr, bson_iter_utf8(&iter, NULL));
// _LOG_DEBUG("avalue->sin_addr:%s\n", avalue->saddr.sin_addr);
// _LOG_DEBUG("avalue->sin_port:%d\n", avalue->saddr.sin_port);
                } else if (!strcmp("Access_key", bson_iter_key(&iter))) {
                    strcpy(avalue->access_key, bson_iter_utf8(&iter, NULL));
// _LOG_DEBUG("avalue->access_key:%s\n", avalue->access_key);
                } else if (!strcmp("Lease_time", bson_iter_key(&iter))) {
                    avalue->lease_time = bson_iter_double(&iter);
// _LOG_DEBUG("avalue->lease_time:%ld\n", avalue->lease_time);
                } else if (!strcmp("Linked_status", bson_iter_key(&iter))) {
                    strcpy(avalue->linked_status, bson_iter_utf8(&iter, NULL));
// _LOG_DEBUG("avalue->linked_status:%s\n", avalue->linked_status);
                }
            }
        }
    }
    if (mongoc_cursor_error(cursor, &error)) {
        _LOG_WARN("cursor failure:%s", error.message);
        return -1;
    }
    //
    bson_destroy(query);
    mongoc_cursor_destroy(cursor);
    mongoc_collection_destroy(collection);
    return 0;
}

int get_cvalue_by_uid(struct cache_value *cvalue, mongoc_client_t *client, char *dabe_name, unsigned int uid) {
    mongoc_collection_t *collection;
    mongoc_cursor_t *cursor;
    bson_error_t error;
    const bson_t *doc;
    bson_t *query;
    //
    collection = mongoc_client_get_collection(client, dabe_name, USER_COLLECTION_NAME);
    query = BCON_NEW("User_id", BCON_INT32(uid));
    cursor = mongoc_collection_find(collection, MONGOC_QUERY_NONE,
            0, 0, 0, query, NULL, NULL);
    bson_iter_t iter;
    while (mongoc_cursor_next(cursor, &doc)) {
// char *str = bson_as_json (doc, NULL); _LOG_DEBUG("%s\n", str); bson_free (str);
        if (bson_iter_init(&iter, doc)) {
            while (bson_iter_next(&iter)) {
                if (!strcmp("User_id", bson_iter_key(&iter))) {
                    cvalue->uid = bson_iter_double(&iter);
// _LOG_DEBUG("cvalue->uid:%d\n", cvalue->uid);
                } else if (!strcmp("Location", bson_iter_key(&iter))) {
                    strcpy(cvalue->location, bson_iter_utf8(&iter, NULL));
// _LOG_DEBUG("cvalue->location:%s\n", cvalue->location);
                } else if (!strcmp("Access_key", bson_iter_key(&iter))) {
                    strcpy(cvalue->access_key, bson_iter_utf8(&iter, NULL));
// _LOG_DEBUG("cvalue->access_key:%s\n", cvalue->access_key);
                }
            }
        }
    }
    if (mongoc_cursor_error(cursor, &error)) {
        _LOG_WARN("cursor failure:%s", error.message);
        return -1;
    }
    //
    bson_destroy(query);
    mongoc_cursor_destroy(cursor);
    mongoc_collection_destroy(collection);
    return 0;
}

int update_access_key(mongoc_client_t *client, char *dabe_name, unsigned int uid, char *access_key, long int lease_time) {
    mongoc_collection_t *collection;
    bson_error_t error;
    bson_t *query;
    //
    collection = mongoc_client_get_collection(client, dabe_name, USER_COLLECTION_NAME);
    query = BCON_NEW("User_id", BCON_INT32(uid));
    bson_t *doc = bson_new();
    bson_t child;
    bson_append_document_begin(doc, "$set", -1, &child);
    BSON_APPEND_UTF8(&child, "Access_key", access_key);
    BSON_APPEND_DOUBLE(&child, "Lease_time", lease_time);
    bson_append_document_end(doc, &child);
// char *str = bson_as_json (doc, NULL); _LOG_DEBUG("%s\n", str); bson_free (str);
    bool upd_value = mongoc_collection_update(collection, MONGOC_UPDATE_NONE,
            query, doc, NULL, &error);
    if (!upd_value) {
        _LOG_WARN("update failure:%s", error.message);
        return -1;
    }
    //
    bson_destroy(&child);
    bson_destroy(doc);
    bson_destroy(query);
    mongoc_collection_destroy(collection);
    return 0;
}

int get_linked_status(mongoc_client_t *client, char *dabe_name, char *linked_status, unsigned int uid) {
    mongoc_collection_t *collection;
    mongoc_cursor_t *cursor;
    bson_error_t error;
    const bson_t *doc;
    bson_t *query;
    //
    collection = mongoc_client_get_collection(client, dabe_name, USER_COLLECTION_NAME);
    query = BCON_NEW("User_id", BCON_INT32(uid));
    cursor = mongoc_collection_find(collection, MONGOC_QUERY_NONE,
            0, 0, 0, query, NULL, NULL);
    bson_iter_t iter;
    while (mongoc_cursor_next(cursor, &doc)) {
// char *str = bson_as_json (doc, NULL); _LOG_DEBUG("%s\n", str); bson_free (str);
        if (bson_iter_init(&iter, doc)) {
            while (bson_iter_next(&iter)) {
                if (!strcmp("Linked_status", bson_iter_key(&iter))) {
                    strcpy(linked_status, bson_iter_utf8(&iter, NULL));
// _LOG_DEBUG("avalue->linked_status:%s\n", avalue->linked_status);
                }
            }
        }
    }
    if (mongoc_cursor_error(cursor, &error)) {
        _LOG_WARN("cursor failure:%s", error.message);
        return -1;
    }
    //
    bson_destroy(query);
    mongoc_cursor_destroy(cursor);
    mongoc_collection_destroy(collection);
    return 0;
}

int update_linked_status(mongoc_client_t *client, char *dabe_name, unsigned int uid, char *linked_status) {
    mongoc_collection_t *collection;
    bson_error_t error;
    bson_t *query;
    //
    collection = mongoc_client_get_collection(client, dabe_name, USER_COLLECTION_NAME);
    query = BCON_NEW("User_id", BCON_INT32(uid));
    bson_t *doc = bson_new();
    bson_t child;
    bson_append_document_begin(doc, "$set", -1, &child);
    BSON_APPEND_UTF8(&child, "Linked_status", linked_status);
    bson_append_document_end(doc, &child);
// char *str = bson_as_json (doc, NULL); _LOG_DEBUG("%s\n", str); bson_free (str);
    bool upd_value = mongoc_collection_update(collection, MONGOC_UPDATE_NONE,
            query, doc, NULL, &error);
    if (!upd_value) {
        _LOG_WARN("update failure:%s", error.message);
        return -1;
    }
    //
    bson_destroy(&child);
    bson_destroy(doc);
    bson_destroy(query);
    mongoc_collection_destroy(collection);
    return 0;
}

int ping_auth_dbase(mongoc_client_t *client, char *dabe_name) {
    mongoc_database_t *database;
    bson_t *command, reply;
    bson_error_t error;
    //
    database = mongoc_client_get_database(client, dabe_name);
    if (!database) {
        _LOG_WARN("couldn't connect to database.");
        return -1;
    }
    command = BCON_NEW("ping", BCON_INT32(1));
    if (!mongoc_database_command_simple(database, command, NULL, &reply, &error)) {
        _LOG_WARN("exec database command:%s", error.message);
        return -1;
    }
    int mon_value = -1;
    bson_iter_t iter;
    if (bson_iter_init(&iter, &reply)) {
        while (bson_iter_next(&iter)) {
// printf ("named: %s : %f\n", bson_iter_key (&iter), bson_iter_double (&iter));
            if (!strcmp("ok", bson_iter_key(&iter)) && 0x01 == bson_iter_double(&iter)) {
                mon_value = 0x00;
                break;
            }
        }
    }
// char *str = bson_as_json (&reply, NULL); printf ("reply:%s\n", str); bson_free(str);
    bson_destroy(&reply);
    bson_destroy(command);
    mongoc_database_destroy(database);
    return mon_value;
}

int get_svalue_by_uname(struct set_value *svalue, mongoc_client_t *client, char *dabe_name, const char *user_name) {
    mongoc_collection_t *collection;
    mongoc_cursor_t *cursor;
    bson_error_t error;
    const bson_t *doc;
    bson_t *query;
    //
    collection = mongoc_client_get_collection(client, dabe_name, USER_COLLECTION_NAME);
    query = BCON_NEW("User_name", user_name);
    cursor = mongoc_collection_find(collection, MONGOC_QUERY_NONE,
            0, 0, 0, query, NULL, NULL);
    bson_iter_t iter;
    while (mongoc_cursor_next(cursor, &doc)) {
// char *str = bson_as_json (doc, NULL); _LOG_DEBUG("%s", str); bson_free (str);
        if (bson_iter_init(&iter, doc)) {
            while (bson_iter_next(&iter)) {
                if (!strcmp("User_id", bson_iter_key(&iter))) {
                    svalue->uid = bson_iter_double(&iter);
// _LOG_DEBUG("svalue->uid:%d", svalue->uid);
                } else if (!strcmp("Password", bson_iter_key(&iter))) {
                    strcpy(svalue->password, bson_iter_utf8(&iter, NULL));
// _LOG_DEBUG("svalue->password:%s", svalue->password);
                } else if (!strcmp("Salt", bson_iter_key(&iter))) {
                    strcpy(svalue->_salt_, bson_iter_utf8(&iter, NULL));
// _LOG_DEBUG("svalue->_salt_:%s", svalue->_salt_);
                } else if (!strcmp("Pool_size", bson_iter_key(&iter))) {
                    svalue->pool_size = bson_iter_double(&iter);
// _LOG_DEBUG("svalue->pool_size:%d", svalue->pool_size);
                } else if (!strcmp("Version", bson_iter_key(&iter))) {
                    strcpy(svalue->version, bson_iter_utf8(&iter, NULL));
// _LOG_DEBUG("svalue->version:%s", svalue->version);
                } else if (!strcmp("Linked_status", bson_iter_key(&iter))) {
                    strcpy(svalue->linked_status, bson_iter_utf8(&iter, NULL));
// _LOG_DEBUG("svalue->linked_status:%s", svalue->linked_status);
                }
            }
        }
    }
    if (mongoc_cursor_error(cursor, &error)) {
        _LOG_WARN("cursor failure:%s", error.message);
        return -1;
    }
    //
    bson_destroy(query);
    mongoc_cursor_destroy(cursor);
    mongoc_collection_destroy(collection);
    return 0;
}

int get_lvalue_by_uname(struct link_value *lvalue, mongoc_client_t *client, char *dabe_name, const char *user_name) {
    mongoc_collection_t *collection;
    mongoc_cursor_t *cursor;
    bson_error_t error;
    const bson_t *doc;
    bson_t *query;
    //
    collection = mongoc_client_get_collection(client, dabe_name, USER_COLLECTION_NAME);
    query = BCON_NEW("User_name", user_name);
    cursor = mongoc_collection_find(collection, MONGOC_QUERY_NONE,
            0, 0, 0, query, NULL, NULL);
    bson_iter_t iter;
    while (mongoc_cursor_next(cursor, &doc)) {
// char *str = bson_as_json (doc, NULL); fprintf(stderr, "%s\n", str); bson_free (str);
        if (bson_iter_init(&iter, doc)) {
            while (bson_iter_next(&iter)) {
                if (!strcmp("User_id", bson_iter_key(&iter))) {
                    lvalue->uid = bson_iter_double(&iter);
// _LOG_DEBUG("lvalue->uid:%d\n", lvalue->uid);
                } else if (!strcmp("Password", bson_iter_key(&iter))) {
                    strcpy(lvalue->password, bson_iter_utf8(&iter, NULL));
// _LOG_DEBUG("lvalue->password:%s\n", lvalue->password);
                } else if (!strcmp("Salt", bson_iter_key(&iter))) {
                    strcpy(lvalue->_salt_, bson_iter_utf8(&iter, NULL));
// _LOG_DEBUG("lvalue->_salt_:%s\n", lvalue->_salt_);
                } else if (!strcmp("Linked_status", bson_iter_key(&iter))) {
                    strcpy(lvalue->linked_status, bson_iter_utf8(&iter, NULL));
// _LOG_DEBUG("lvalue->linked_status:%s\n", lvalue->linked_status);
                }
            }
        }
    }
    if (mongoc_cursor_error(cursor, &error)) {
        _LOG_WARN("cursor failure:%s", error.message);
        return -1;
    }
    //
    bson_destroy(query);
    mongoc_cursor_destroy(cursor);
    mongoc_collection_destroy(collection);
    return 0;
}

//

int get_ssev_by_auth(struct addr_list **alsth, struct ssev_config *config, const char *cache_id) {
    char uristr[URI_LENGTH];
    sprintf(uristr, "mongodb://%s:%s@%s:%d/%s", config->user_name, config->password,
            config->address.sin_addr, config->address.sin_port, config->database);
    mongoc_client_t *client = mongoc_client_new(uristr);
    if (!client) {
        _LOG_WARN("connect serv_db clent failed!");
        return -1;
    }
    //
    mongoc_collection_t *collection;
    mongoc_cursor_t *cursor;
    bson_error_t error;
    const bson_t *doc;
    bson_t *query;
    //
    collection = mongoc_client_get_collection(client, config->database, SSEV_COLLECTION_NAME);
    query = BCON_NEW("Cache_id", cache_id);
    cursor = mongoc_collection_find(collection, MONGOC_QUERY_NONE,
            0, 0, 0, query, NULL, NULL);
    bson_iter_t iter;
    serv_addr *saddr;
    while (mongoc_cursor_next(cursor, &doc)) {
// char *str = bson_as_json (doc, NULL); fprintf(stderr, "%s\n", str); bson_free (str);
        if (bson_iter_init(&iter, doc)) {
            saddr = add_new_alist(alsth);
            while (bson_iter_next(&iter)) {
                if (!strcmp("Serv_addr", bson_iter_key(&iter))) {
                    strcpy(saddr->sin_addr, bson_iter_utf8(&iter, NULL));
// _LOG_DEBUG("saddr->sin_addr:%s", saddr->sin_addr);
                } else if (!strcmp("Bind_port", bson_iter_key(&iter))) {
                    saddr->sin_port = bson_iter_double(&iter);
// _LOG_DEBUG("saddr->sin_port:%d", saddr->sin_port);
                }
            }
        }
    }
    if (mongoc_cursor_error(cursor, &error)) {
        _LOG_WARN("cursor failure:%s", error.message);
        return -1;
    }
    //
    bson_destroy(query);
    mongoc_cursor_destroy(cursor);
    mongoc_collection_destroy(collection);
    mongoc_client_destroy(client);
    return 0;
}

//

int get_auth_by_ssev(char *cache_id, struct ssev_config *config, const char *serv_addr, int serv_port) {
    char uristr[URI_LENGTH];
    sprintf(uristr, "mongodb://%s:%s@%s:%d/%s", config->user_name, config->password,
            config->address.sin_addr, config->address.sin_port, config->database);
    mongoc_client_t *client = mongoc_client_new(uristr);
    if (!client) {
        _LOG_WARN("connect serv_db clent failed!");
        return -1;
    }
    //
    mongoc_collection_t *collection;
    mongoc_cursor_t *cursor;
    bson_error_t error;
    const bson_t *doc;
    bson_t *query;
    //
    collection = mongoc_client_get_collection(client, config->database, SSEV_COLLECTION_NAME);
    query = BCON_NEW("Serv_addr", serv_addr, "Bind_port", BCON_INT32(serv_port));
    cursor = mongoc_collection_find(collection, MONGOC_QUERY_NONE,
            0, 0, 0, query, NULL, NULL);
    bson_iter_t iter;
    while (mongoc_cursor_next(cursor, &doc)) {
// char *str = bson_as_json (doc, NULL); fprintf(stderr, "%s\n", str); bson_free (str);
        if (bson_iter_init(&iter, doc)) {
            while (bson_iter_next(&iter)) {
                if (!strcmp("Cache_id", bson_iter_key(&iter))) {
                    strcpy(cache_id, bson_iter_utf8(&iter, NULL));
// _LOG_DEBUG("cache_id:%s", cache_id);
                }
            }
        }
    }
    if (mongoc_cursor_error(cursor, &error)) {
        _LOG_WARN("cursor failure:%s", error.message);
        return -1;
    }
    //
    bson_destroy(query);
    mongoc_cursor_destroy(cursor);
    mongoc_collection_destroy(collection);
    mongoc_client_destroy(client);
    return 0;
}