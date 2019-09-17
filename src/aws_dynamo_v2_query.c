#define _GNU_SOURCE

#include "aws_dynamo_utils.h"

#include <stdlib.h>

#include <yajl/yajl_parse.h>

#include "http.h"
#include "aws_dynamo.h"
#include "aws_dynamo_v2_query.h"
#include "aws_dynamo_json.h"

struct aws_dynamo_v2_query_response *aws_dynamo_parse_v2_query_response(const char *response, int response_len)
{
    yajl_val root;
    char errbuf[1024];

    struct aws_dynamo_v2_query_response *qryResponse = NULL;
    qryResponse = calloc(1, sizeof(*(qryResponse)));
    if (qryResponse == NULL) {
        Warnx("aws_dynamo_parse_query_response: alloc failed.");
        return NULL;
    }

    memset(errbuf, 0x00, sizeof(errbuf));
    root = yajl_tree_parse(response, errbuf, sizeof(errbuf));
    if (root == NULL) {
        if (strlen(errbuf)) {
            Warnx("aws_dynamo_parse_query_response: json parse failed, '%s'", errbuf);
        } else {
            Warnx("aws_dynamo_parse_query_response: json parse failed, 'unknown error'");
        }
        return NULL;
    }

    qryResponse->count = YAJL_GET_INTEGER(yajl_getNode(root, AWS_DYNAMO_JSON_COUNT));
    if (qryResponse->count <= 0) {
        return qryResponse;
    }

    qryResponse->scanned_count = YAJL_GET_INTEGER(yajl_getNode(root, AWS_DYNAMO_JSON_SCANNED_COUNT));

    char tmp[64];
    sprintf(tmp, "%s/%s", AWS_DYNAMO_JSON_V2_CONSUMED_CAPACITY, AWS_DYNAMO_JSON_V2_CAPACITY_UNITS);
    yajl_val capacityUnit = yajl_getNode(root, tmp);
    if (capacityUnit != NULL) {
        qryResponse->consumedCapacityQry.capacityUnits = YAJL_GET_INTEGER(capacityUnit);

        sprintf(tmp, "%s/%s", AWS_DYNAMO_JSON_V2_CONSUMED_CAPACITY, AWS_DYNAMO_JSON_TABLE_NAME);
        char *tblName = YAJL_GET_STRING(yajl_getNode(root, tmp));
        qryResponse->consumedCapacityQry.tableName = strdup(tblName);
    }

    struct aws_dynamo_item *items = NULL;
    items = calloc(qryResponse->count, sizeof(*items));
    if (items == NULL) {
        yajl_tree_free(root);
        Warnx("aws_dynamo_parse_v2_query_response: alloc failed.");
        return NULL;
    }

    qryResponse->items = items;

    getArrayItems(root, qryResponse->items, AWS_DYNAMO_JSON_ITEMS);
    getObjectItem(root, &qryResponse->lastEvaluatedKeys, AWS_DYNAMO_JSON_LAST_EVALUATED_KEY);

    yajl_tree_free(root);

    return qryResponse;
}

struct aws_dynamo_v2_query_response *aws_dynamo_v2_query(struct aws_handle *aws, const char *request)
{
    const char *response = NULL;
    int response_len = 0;
    struct aws_dynamo_v2_query_response *r = NULL;

    if (aws_dynamo_request(aws, AWS_DYNAMO_V2_QUERY, request) == -1) {
        return NULL;
    }

    response = http_get_data(aws->http, &response_len);
    if (response == NULL) {
        Warnx("aws_dynamo_v2_query: Failed to get response.");
        return NULL;
    }

    if ((r = aws_dynamo_parse_v2_query_response(response, response_len)) == NULL) {
        Warnx("aws_dynamo_v2_query: Failed to parse response: '%s'", response);
        return NULL;
    }

    return r;
}

void aws_dynamo_dump_v2_query_response(struct aws_dynamo_v2_query_response *r) {
#ifdef DEBUG_PARSER
    int i;
    struct aws_dynamo_item *items;

    if (r == NULL) {
        Debug("Empty response.");
        return;
    }

    Debug("consumed_capacity_units = %f", r->consumedCapacityQry.capacityUnits);
    Debug("TableName = %s", r->consumedCapacityQry.tableName);
    Debug("item count = %d, item scanned count = %d", r->count, r->scanned_count);

    items = r->items;
    for (i = 0; i < r->count; i++) {
        struct aws_dynamo_item *item = &(items[i]);
        Debug("Item %d, %d attributes", i, item->num_attributes);
        aws_dynamo_dump_attributes(item->attributes, item->num_attributes);
    }
#endif
}

void aws_dynamo_free_v2_query_response(struct aws_dynamo_v2_query_response *r) {
    int i;
    struct aws_dynamo_item *items;

    if (r == NULL) return;

    if (r->consumedCapacityQry.tableName != NULL) {
        free(r->consumedCapacityQry.tableName);
    }

    items = r->items;
    for (i = 0; i < r->count; i++) {
        struct aws_dynamo_item *item = &(items[i]);
        aws_dynamo_free_attributes(item->attributes, item->num_attributes);
    }

    free(r->items);


    if (r->lastEvaluatedKeys.attributes != NULL) {
        aws_dynamo_free_attributes(r->lastEvaluatedKeys.attributes, r->lastEvaluatedKeys.num_attributes);
    }

    free(r);
}

