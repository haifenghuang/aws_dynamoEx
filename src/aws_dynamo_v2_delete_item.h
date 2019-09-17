#ifndef _AWS_DYNAMO_V2_DELETE_ITEM_H_
#define _AWS_DYNAMO_V2_DELETE_ITEM_H_

#include "aws_dynamo.h"

#ifdef __cplusplus
extern "C" {
#endif

struct aws_dynamo_v2_delete_item_response {
    struct consumedCapacityDel {
        double capacityUnits;
        char *tableName;
    } consumedCapacityDel;

    struct aws_dynamo_item item;
};

struct aws_dynamo_v2_delete_item_response *aws_dynamo_v2_delete_item(struct aws_handle *aws, const char *request);

/* Do not parse the response, only care about success or fail.
 * 0: successful   1: failure
 */
int aws_dynamo_v2_delete_item2(struct aws_handle *aws, const char *request);

void aws_dynamo_free_v2_delete_item_response(struct aws_dynamo_v2_delete_item_response *r);

void aws_dynamo_dump_v2_delete_item_response(struct aws_dynamo_v2_delete_item_response *r);

#ifdef  __cplusplus
}
#endif

#endif /* _AWS_DYNAMO_V2_DELETE_ITEM_H_ */

