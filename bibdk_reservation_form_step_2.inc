<?php

function bibdk_reservation_form_step_2($form, &$form_state) {
  if (isset($form_state['step_information'][1]['stored_values'])) {
    $name = $form_state['step_information'][1]['stored_values']['openagency_name'];
    $zip = $form_state['step_information'][1]['stored_values']['openagency_zip'];
    $city = $form_state['step_information'][1]['stored_values']['openagency_city'];
  }

  module_load_include('inc', 'ting_openformat', 'ting_openformat.search_client');
  $client = new ting_client_class();
  $resp = $client->do_agency_search(array('agencyName' => $name, 'postalCode' => $zip, 'city' => $city));
  $result = bibdk_vejviser_handle_response($resp);

  foreach ($result['libraries'] as $agency) {
    foreach ($agency->pickUpAgencies as $branch) {
      #dpm($branch);
      $form[$branch->branchId]['button'] = array(
        '#value' => t('Select'),
        '#name' => $branch->branchId, 
        '#prefix' => theme('bibdk_reservation_agency', array('agency' => $branch)),
        '#suffix' => '<hr/></div>',
        '#type' => 'submit',
        '#submit' => array('bibdk_reservation_wizard_next_submit', 'bibdk_reservation_form_step_2_update_orderinfo'),
        '#branch' => $branch,
      );
    }
  }

  return $form;
}

function bibdk_reservation_form_step_2_update_orderinfo($form, $form_state) {
  dpm($form_state);
  if (!isset($_SESSION['orderobject'])) {
    $_SESSION['orderobject'] = new BibdkReservationOrderObject();
  }
  $orderObj = $_SESSION['orderobject'];
  $orderObj->setAgencyId($form_state['clicked_button']['#name']);
}