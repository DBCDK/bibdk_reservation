<?php

function bibdk_reservation_form_step_2($form, &$form_state) {
  if(isset($form_state['step_information'][1]['stored_values'])){
    $name = $form_state['step_information'][1]['stored_values']['openagency_name'];
    $zip = $form_state['step_information'][1]['stored_values']['openagency_zip'];
    $city = $form_state['step_information'][1]['stored_values']['openagency_city'];
  }
  
  module_load_include('inc', 'ting_openformat', 'ting_openformat.search_client');
  $client = new ting_client_class();
  $resp = $client->do_agency_search(array('agencyName' => $name, 'postalCode' => $zip, 'city' => $city));
  $result = bibdk_vejviser_handle_response($resp);
  
  dpm($form_state);
  
  foreach ($result['libraries'] as $agency) {
    foreach ($agency->pickUpAgencies as $branch) {
      $form[$branch->branchId] = array(
        '#value' => t('Select'),
        '#prefix' => theme('bibdk_reservation_agency',array('agency'=>$branch)),
        '#suffix' => '<hr/></div>',
        '#type' => 'submit',
        '#submit' => array('bibdk_reservation_wizard_next_submit'),
      );
    }
  }
  return $form;
}