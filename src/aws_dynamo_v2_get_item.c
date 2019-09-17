#define _GNU_SOURCE

#include "aws_dynamo_utils.h"

#include <stdlib.h>
#include <string.h>

#include <yajl/yajl_parse.h>

#include "http.h"
#include "aws_dynamo.h"
#include "aws_dynamo_v2_get_item.h"
#include "aws_dynamo_json.h"

struct aws_dynamo_v2_get_item_response *aws_dynamo_parse_v2_get_item_response(const char *response, int response_len)
{
    yajl_val root;
    char errbuf[1024];

    struct aws_dynamo_v2_get_item_response *getResponse = NULL;
    getResponse = calloc(1, sizeof(*(getResponse)));
    if (getResponse == NULL) {
        Warnx("aws_dynamo_parse_v2_get_item_response: alloc failed.");
        return NULL;
    }

    memset(errbuf, 0x00, sizeof(errbuf));
    root = yajl_tree_parse(response, errbuf, sizeof(errbuf));
    if (root == NULL) {
        if (strlen(errbuf)) {
            Warnx("aws_dynamo_v2_get_item_response: json parse failed, '%s'", errbuf);
        } else {
            Warnx("aws_dynamo_v2_get_item_response: json parse failed, 'unknown error'");
        }
        return NULL;
    }

    char tmp[64];
    sprintf(tmp, "%s/%s", AWS_DYNAMO_JSON_V2_CONSUMED_CAPACITY, AWS_DYNAMO_JSON_V2_CAPACITY_UNITS);
    yajl_val capacityUnit = yajl_getNode(root, tmp);
    if (capacityUnit != NULL) {
        getResponse->consumedCapacityGet.capacityUnits = YAJL_GET_INTEGER(capacityUnit);

        sprintf(tmp, "%s/%s", AWS_DYNAMO_JSON_V2_CONSUMED_CAPACITY, AWS_DYNAMO_JSON_TABLE_NAME);
        char *tblName = YAJL_GET_STRING(yajl_getNode(root, tmp));
        getResponse->consumedCapacityGet.tableName = strdup(tblName);
    }

    getObjectItem(root, &getResponse->item, AWS_DYNAMO_JSON_ITEM);

    yajl_tree_free(root);

    return getResponse;
}

struct aws_dynamo_v2_get_item_response *aws_dynamo_v2_get_item(struct aws_handle *aws, const char *request)
{
    const char *response = NULL;
    int response_len = 0;
    struct aws_dynamo_v2_get_item_response *r = NULL;

    if (aws_dynamo_request(aws, AWS_DYNAMO_V2_GET_ITEM, request) == -1) {
        return NULL;
    }

    response = http_get_data(aws->http, &response_len);

    if (response == NULL) {
        Warnx("aws_dynamo_v2_get_item: Failed to get response.");
        return NULL;
    }

    if ((r = aws_dynamo_parse_v2_get_item_response(response, response_len)) == NULL) {
        Warnx("aws_dynamo_v2_get_item: Failed to parse response: '%s'", response);
        return NULL;
    }

    return r;
}

void aws_dynamo_dump_v2_get_item_response(struct aws_dynamo_v2_get_item_response *r) {
#ifdef DEBUG_PARSER

    if (r == NULL) {
        Debug("Empty response.");
        return;
    }

    Debug("consumed_capacity_units = %f", r->consumedCapacityGet.capacityUnits);
    Debug("TableName = %s", r->consumedCapacityGet.tableName);
    if (r->item.attributes != NULL) {
        aws_dynamo_dump_attributes(r->item.attributes, r->item.num_attributes);
    } else {
        Debug("no attributes");
    }
#endif
}

void aws_dynamo_free_v2_get_item_response(struct aws_dynamo_v2_get_item_response *r) {
    struct aws_dynamo_item *item;

    if (r == NULL) {
        return;
    }

    item = &(r->item);

    if (item->attributes != NULL) {
        aws_dynamo_free_attributes(item->attributes, item->num_attributes);
    }

    if (r->consumedCapacityGet.tableName != NULL) {
        free(r->consumedCapacityGet.tableName);
    }

    free(r);
}

