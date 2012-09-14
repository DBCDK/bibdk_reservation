<?php

function bibdk_reservation_form_step_1($form, &$form_state) {
  $form['openagency_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#size' => 5,
    '#default_value' => isset($form_state['values']['openagency_name']) ? $form_state['values']['openagency_name'] : '',
  );
  $form['openagency_zip'] = array(
    '#type' => 'textfield',
    '#title' => t('Zip'),
    '#size' => 5,
    '#default_value' => isset($form_state['values']['openagency_zip']) ? $form_state['values']['openagency_zip'] : '',
  );
  $form['openagency_city'] = array(
    '#type' => 'textfield',
    '#title' => t('City'),
    '#size' => 5,
    '#default_value' => isset($form_state['values']['openagency_city']) ? $form_state['values']['openagency_city'] : '',
  );
  return $form;
}

function bibdk_reservation_form_step_1_validate($form, &$form_state) {
  if (empty($form_state['values']['openagency_name']) && empty($form_state['values']['openagency_zip']) && empty($form_state['values']['openagency_city'])) {
    form_set_error('bibdk_reservation_form_step_1', t('At least one kind of information should be provided'));
  }

  if (!empty($form_state['values']['openagency_name']) || !empty($form_state['values']['openagency_zip']) || !empty($form_state['values']['openagency_city'])) {
    $params = array('agencyName' => $form_state['values']['openagency_name'], 'postalCode' => $form_state['values']['openagency_zip'], 'city' => $form_state['values']['openagency_city']);
    $result = bibdk_reservation_form_step_1_execute_search($params);
    $form_state['values']['result'] = $result;
  }
}

function bibdk_reservation_form_step_1_execute_search($params) {
  module_load_include('inc', 'ting_openformat', 'ting_openformat.search_client');
  $client = new ting_client_class();
  $resp = $client->do_agency_search($params);
  $result = bibdk_reservation_form_step_1_handle_response($resp);
  return $result;
}

/**
 * Handling the response.
 * @param array $response
 * @return array
 */
function bibdk_reservation_form_step_1_handle_response($response) {
  if (isset($response['error'])) {
    switch ($response['error']) {
      case 'no_agencies_found':
        form_set_error('bibdk_reservation_form_step_1', t('No libraries found. Please provide more information'));
        break;
      default:
        form_set_error('bibdk_reservation_form_step_1', 'Error: ' . $response['error']);
        break;
    }
  } else if (isset($response['libraries'])) {
    $result = $response;
    return $result;
  }
}