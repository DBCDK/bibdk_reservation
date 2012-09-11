<?php

function bibdk_reservation_execute_agency_service() {
  
    module_load_include('inc', 'ting_openformat', 'ting_openformat.search_client');
    $client = new ting_client_class();
    
    $service = 'userOrderParameters';
    $agencyId = 'DK-810010';
    
    $resp = $client->do_agency_service(array('agencyId' => $agencyId, 'service' => $service));

    print_r($resp);
    return "";
}