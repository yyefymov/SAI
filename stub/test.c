#include <stdio.h>
#include "sai.h"

const char* test_profile_get_value(
    _In_ sai_switch_profile_id_t profile_id,
    _In_ const char* variable)
{
    return 0;
}

int test_profile_get_next_value(
    _In_ sai_switch_profile_id_t profile_id,
    _Out_ const char** variable,
    _Out_ const char** value)
{
    return -1;
}

const service_method_table_t test_services = {
    test_profile_get_value,
    test_profile_get_next_value
};

int main()
{
    sai_status_t              status;
    sai_switch_api_t         *switch_api;
    sai_lag_api_t            *lag_api;
    sai_object_id_t           vr_oid;
    sai_attribute_t           attrs[2];
    sai_switch_notification_t notifications;
    sai_object_id_t           port_list[64];

    status = sai_api_initialize(0, &test_services);
    if (status != SAI_STATUS_SUCCESS) {
      printf("SAI API initialize call failed with status %d\n", status);
      return 1;
    }

    status = sai_api_query(SAI_API_SWITCH, (void**)&switch_api);
    if (status != SAI_STATUS_SUCCESS) {
      printf("SAI API query failed with status %d\n", status);
        return 1;
    }

    status = switch_api->initialize_switch(0, "HW_ID", 0, &notifications);
    if (status != SAI_STATUS_SUCCESS) {
      printf("SAI API call initialize_switch failed with status %d\n", status);
      return 1;
    }

    attrs[0].id = SAI_SWITCH_ATTR_PORT_LIST;
    attrs[0].value.objlist.list = port_list;
    attrs[0].value.objlist.count = 64;
    status = switch_api->get_switch_attribute(1, attrs);
    if (status != SAI_STATUS_SUCCESS) {
      printf("SAI API call get_switch_attribute failed with status %d\n", status);
      return 1;
    }

    for (int32_t ii = 0; ii < attrs[0].value.objlist.count; ii++) {
      printf("Port #%d OID: 0x%lX\n", ii, attrs[0].value.objlist.list[ii]);
    }

    // Test LAG

    status = sai_api_query(SAI_API_LAG, (void**)&lag_api);
    if (status != SAI_STATUS_SUCCESS) {
      printf("Failed to query LAG API, status=%d\n", status);
      return 1;
    }

    // create first LAG and its members
    sai_object_id_t first_lag_id = 0;
    status = lag_api->create_lag(&first_lag_id, 0, NULL);
    if (status != SAI_STATUS_SUCCESS) {
      printf("Failed to create a LAG, status=%d\n", status);
      return 1;
    }

    sai_object_id_t first_lag_members[2] = {0};
    for (uint32_t i = 0; i < 2; ++i) {
      sai_attribute_t lag_member_attr = {0};
      status = lag_api->create_lag_member(&first_lag_members[i], 1, &lag_member_attr);
      if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to create a LAG MEMBER, status=%d\n", status);
        return 1;
      }
    }

    // create second LAG and its members
    sai_object_id_t second_lag_id = 0;
    status = lag_api->create_lag(&second_lag_id, 0, NULL);
    if (status != SAI_STATUS_SUCCESS) {
      printf("Failed to create a LAG, status=%d\n", status);
      return 1;
    }

    sai_object_id_t second_lag_members[2] = {0};
    for (uint32_t i = 0; i < 2; ++i) {
      status = lag_api->create_lag_member(&second_lag_members[i], 1, NULL);
      if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to create a LAG MEMBER, status=%d\n", status);
        return 1;
      }
    }

    // get LAG parameters
    status = lag_api->get_lag_attribute(first_lag_id, 1, NULL); // NOT IMPLEMENTED YET
    status = lag_api->get_lag_attribute(second_lag_id, 1, NULL); // NOT IMPLEMENTED YET

    // get LAG member parameters
    status = lag_api->get_lag_member_attribute(first_lag_members[0], 1, NULL); // NOT IMPLEMENTED YET
    status = lag_api->get_lag_member_attribute(second_lag_members[0], 1, NULL); // NOT IMPLEMENTED YET

    // remove members and check LAG parameters
    status = lag_api->remove_lag_member(first_lag_members[1]);
    if (status != SAI_STATUS_SUCCESS) {
      printf("Failed to remove the LAG MEMBER 0x%lX, status=%d\n", first_lag_members[1], status);
      return 1;
    }
    status = lag_api->get_lag_attribute(first_lag_id, 1, NULL); // NOT IMPLEMENTED YET

    status = lag_api->remove_lag_member(second_lag_members[0]);
    if (status != SAI_STATUS_SUCCESS) {
      printf("Failed to remove the LAG MEMBER 0x%lX, status=%d\n", second_lag_members[0], status);
      return 1;
    }
    status = lag_api->get_lag_attribute(second_lag_id, 1, NULL); // NOT IMPLEMENTED YET

    // remove remaining LAG members and LAGs
    status = lag_api->remove_lag_member(first_lag_members[0]);
    if (status != SAI_STATUS_SUCCESS) {
      printf("Failed to remove the LAG MEMBER 0x%lX, status=%d\n", first_lag_members[1], status);
      return 1;
    }

    status = lag_api->remove_lag_member(second_lag_members[1]);
    if (status != SAI_STATUS_SUCCESS) {
      printf("Failed to remove the LAG MEMBER 0x%lX, status=%d\n", second_lag_members[1], status);
      return 1;
    }

    status = lag_api->remove_lag(second_lag_id);
    if (status != SAI_STATUS_SUCCESS) {
      printf("Failed to remove the LAG  0x%lX, status=%d\n", second_lag_id, status);
      return 1;
    }

    status = lag_api->remove_lag(first_lag_id);
    if (status != SAI_STATUS_SUCCESS) {
      printf("Failed to remove the LAG  0x%lX, status=%d\n", first_lag_id, status);
      return 1;
    }

    switch_api->shutdown_switch(0);
    status = sai_api_uninitialize();
    if (status != SAI_STATUS_SUCCESS) {
      printf("SAI API uninitialize call failed with status %d\n", status);
      return 1;
    }

    return 0;
}