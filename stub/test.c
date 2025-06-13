#include <stdio.h>
#include <stdlib.h>
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
    printf("\n");

    printf("===== Create first LAG and its members\n");
    sai_object_id_t first_lag_id = 0;
    status = lag_api->create_lag(&first_lag_id, 0, NULL);
    if (status != SAI_STATUS_SUCCESS) {
      printf("Failed to create a LAG, status=%d\n", status);
      return 1;
    }

    sai_object_id_t first_lag_first_member = 0;
    sai_attribute_t lag_member_attrs[2] = {0};
    lag_member_attrs[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;
    lag_member_attrs[0].value.oid = first_lag_id;
    lag_member_attrs[1].id = SAI_LAG_MEMBER_ATTR_PORT_ID;
    lag_member_attrs[1].value.oid = 0x1;
    status = lag_api->create_lag_member(&first_lag_first_member, 2, lag_member_attrs);
    if (status != SAI_STATUS_SUCCESS) {
      printf("Failed to create a LAG MEMBER, status=%d\n", status);
      return 1;
    }

    sai_object_id_t first_lag_second_member = 0;
    lag_member_attrs[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;
    lag_member_attrs[0].value.oid = first_lag_id;
    lag_member_attrs[1].id = SAI_LAG_MEMBER_ATTR_PORT_ID;
    lag_member_attrs[1].value.oid = 0x100000001;
    status = lag_api->create_lag_member(&first_lag_second_member, 2, lag_member_attrs);
    if (status != SAI_STATUS_SUCCESS) {
      printf("Failed to create a LAG MEMBER, status=%d\n", status);
      return 1;
    }
    printf("\n");


    printf("===== Create second LAG and its members\n");
    sai_object_id_t second_lag_id = 0;
    status = lag_api->create_lag(&second_lag_id, 0, NULL);
    if (status != SAI_STATUS_SUCCESS) {
      printf("Failed to create a LAG, status=%d\n", status);
      return 1;
    }

    sai_object_id_t second_lag_first_member = 0;
    lag_member_attrs[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;
    lag_member_attrs[0].value.oid = second_lag_id;
    lag_member_attrs[1].id = SAI_LAG_MEMBER_ATTR_PORT_ID;
    lag_member_attrs[1].value.oid = 0x200000001;
    status = lag_api->create_lag_member(&second_lag_first_member, 2, lag_member_attrs);
    if (status != SAI_STATUS_SUCCESS) {
      printf("Failed to create a LAG MEMBER, status=%d\n", status);
      return 1;
    }

    sai_object_id_t second_lag_second_member = 0;
    lag_member_attrs[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;
    lag_member_attrs[0].value.oid = second_lag_id;
    lag_member_attrs[1].id = SAI_LAG_MEMBER_ATTR_PORT_ID;
    lag_member_attrs[1].value.oid = 0x300000001;
    status = lag_api->create_lag_member(&second_lag_second_member, 2, lag_member_attrs);
    if (status != SAI_STATUS_SUCCESS) {
      printf("Failed to create a LAG MEMBER, status=%d\n", status);
      return 1;
    }
    printf("\n");


    printf("===== Get LAG parameters\n");
    sai_attribute_t lag_attr = {0};
    lag_attr.id = SAI_LAG_ATTR_PORT_LIST;
    lag_attr.value.objlist.count = 32;
    lag_attr.value.objlist.list = calloc(32, sizeof(sai_object_id_t));
    status = lag_api->get_lag_attribute(first_lag_id, 1, &lag_attr);
    if (status != SAI_STATUS_SUCCESS) {
      printf("Failed to get a LAG parameter, status=%d\n", status);
      return 1;
    }
    printf("First LAGs port list:\n");
    for (uint32_t i = 0; i < lag_attr.value.objlist.count; ++i) {
      printf("0x%lX, ", lag_attr.value.objlist.list[i]);
    }
    printf("\n");

    lag_attr.value.objlist.count = 32;
    status = lag_api->get_lag_attribute(second_lag_id, 1, &lag_attr);
    if (status != SAI_STATUS_SUCCESS) {
      printf("Failed to get a LAG parameter, status=%d\n", status);
      return 1;
    }
    printf("Second LAGs port list:\n");
    for (uint32_t i = 0; i < lag_attr.value.objlist.count; ++i) {
      printf("0x%lX, ", lag_attr.value.objlist.list[i]);
    }
    printf("\n\n");
    free(lag_attr.value.objlist.list);


    printf("===== Get LAG member parameters\n");
    lag_member_attrs[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;
    lag_member_attrs[1].id = SAI_LAG_MEMBER_ATTR_PORT_ID;
    status = lag_api->get_lag_member_attribute(first_lag_first_member, 2, lag_member_attrs);
    if (status != SAI_STATUS_SUCCESS) {
      printf("Failed to get LAG member parameters, status=%d\n", status);
      return 1;
    }
    printf("LAG Member 0x%lX LAG ID: 0x%lX, PORT ID: 0x%lX\n", first_lag_first_member, lag_member_attrs[0].value.oid, lag_member_attrs[1].value.oid);

    status = lag_api->get_lag_member_attribute(second_lag_second_member, 2, lag_member_attrs);
    if (status != SAI_STATUS_SUCCESS) {
      printf("Failed to get LAG member parameters, status=%d\n", status);
      return 1;
    }
    printf("LAG Member 0x%lX LAG ID: 0x%lX, PORT ID: 0x%lX\n", first_lag_first_member, lag_member_attrs[0].value.oid, lag_member_attrs[1].value.oid);
    printf("\n");


    printf("===== Remove members and check LAG parameters\n");
    status = lag_api->remove_lag_member(first_lag_second_member);
    if (status != SAI_STATUS_SUCCESS) {
      printf("Failed to remove the LAG MEMBER 0x%lX, status=%d\n", first_lag_second_member, status);
      return 1;
    }

    lag_attr.id = SAI_LAG_ATTR_PORT_LIST;
    lag_attr.value.objlist.count = 32;
    lag_attr.value.objlist.list = calloc(32, sizeof(sai_object_id_t));
    status = lag_api->get_lag_attribute(first_lag_id, 1, &lag_attr);
    if (status != SAI_STATUS_SUCCESS) {
      printf("Failed to get a LAG parameter, status=%d\n", status);
      free(lag_attr.value.objlist.list);
      return 1;
    }
    printf("First LAGs port list:\n");
    for (uint32_t i = 0; i < lag_attr.value.objlist.count; ++i) {
      printf("0x%lX, ", lag_attr.value.objlist.list[i]);
    }
    printf("\n");

    status = lag_api->remove_lag_member(second_lag_first_member);
    if (status != SAI_STATUS_SUCCESS) {
      printf("Failed to remove the LAG MEMBER 0x%lX, status=%d\n", second_lag_first_member, status);
      free(lag_attr.value.objlist.list);
      return 1;
    }
    status = lag_api->get_lag_attribute(second_lag_id, 1, &lag_attr);
    if (status != SAI_STATUS_SUCCESS) {
      printf("Failed to get a LAG parameter, status=%d\n", status);
      free(lag_attr.value.objlist.list);
      return 1;
    }
    printf("First LAGs port list:\n");
    for (uint32_t i = 0; i < lag_attr.value.objlist.count; ++i) {
      printf("0x%lX, ", lag_attr.value.objlist.list[i]);
    }
    printf("\n\n");
    free(lag_attr.value.objlist.list);


    printf("===== Verify that status SAI_STATUS_OBJECT_IN_USE is returned when trying to remove a non-empty LAG\n");
    status = lag_api->remove_lag(second_lag_id);
    if (status != SAI_STATUS_OBJECT_IN_USE) {
      printf("Expected status %lX when removing the LAG  0x%lX, status=%d\n", SAI_STATUS_OBJECT_IN_USE, second_lag_id, status);
      return 1;
    }

    status = lag_api->remove_lag(first_lag_id);
    if (status != SAI_STATUS_OBJECT_IN_USE) {
      printf("Expected status %lX when removing the LAG  0x%lX, status=%d\n", SAI_STATUS_OBJECT_IN_USE, first_lag_id, status);
      return 1;
    }
    printf("\n");


    printf("===== Remove remaining LAG members and LAGs\n");
    status = lag_api->remove_lag_member(first_lag_first_member);
    if (status != SAI_STATUS_SUCCESS) {
      printf("Failed to remove the LAG MEMBER 0x%lX, status=%d\n", first_lag_first_member, status);
      return 1;
    }
    status = lag_api->remove_lag(first_lag_id);
    if (status != SAI_STATUS_SUCCESS) {
      printf("Expected status %lX when removing the LAG  0x%lX, status=%d\n", SAI_STATUS_OBJECT_IN_USE, first_lag_id, status);
      return 1;
    }

    status = lag_api->remove_lag_member(second_lag_second_member);
    if (status != SAI_STATUS_SUCCESS) {
      printf("Failed to remove the LAG MEMBER 0x%lX, status=%d\n", second_lag_second_member, status);
      return 1;
    }
    status = lag_api->remove_lag(second_lag_id);
    if (status != SAI_STATUS_SUCCESS) {
      printf("Expected status %lX when removing the LAG  0x%lX, status=%d\n", SAI_STATUS_OBJECT_IN_USE, second_lag_id, status);
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