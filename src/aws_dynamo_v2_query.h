#ifndef _AWS_DYNAMO_V2_QUERY_H_
#define _AWS_DYNAMO_V2_QUERY_H_

#include "aws_dynamo.h"

#ifdef  __cplusplus
extern "C" {
#endif

struct aws_dynamo_v2_query_response {
    struct consumedCapacityQry {
        double capacityUnits;
        char *tableName;
    } consumedCapacityQry;

    int count;
    int scanned_count;

    struct aws_dynamo_item *items;

    /* Last evaluated keys. */
    struct aws_dynamo_item lastEvaluatedKeys;
};

struct aws_dynamo_v2_query_response *aws_dynamo_parse_v2_query_response(const char *response, int response_len);

struct aws_dynamo_v2_query_response *aws_dynamo_v2_query(struct aws_handle *aws, const char *request);

void aws_dynamo_free_v2_query_response(struct aws_dynamo_v2_query_response *r);

void aws_dynamo_dump_v2_query_response(struct aws_dynamo_v2_query_response *r);

#ifdef  __cplusplus
}
#endif

#endif /* _AWS_DYNAMO_V2_QUERY_H_ */

