#ifndef _AWS_DYNAMO_V2_PUT_ITEM_H_
#define _AWS_DYNAMO_V2_PUT_ITEM_H_

#include "aws_dynamo.h"

#ifdef __cplusplus
extern "C" {
#endif

struct aws_dynamo_v2_put_item_response {
    struct consumedCapacityPut {
        double capacityUnits;
        char *tableName;
    } consumedCapacityPut;

    struct aws_dynamo_item item;
};

struct aws_dynamo_v2_put_item_response *aws_dynamo_parse_v2_put_item_response(const char *response, int response_len);

struct aws_dynamo_v2_put_item_response *aws_dynamo_v2_put_item(struct aws_handle *aws, const char *request);

void aws_dynamo_free_v2_put_item_response(struct aws_dynamo_v2_put_item_response *r);

void aws_dynamo_dump_v2_put_item_response(struct aws_dynamo_v2_put_item_response *r);

#ifdef  __cplusplus
}
#endif

#endif /* _AWS_DYNAMO_V2_PUT_ITEM_H_ */
