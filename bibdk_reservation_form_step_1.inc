<?php

function bibdk_reservation_form_step_1($form, &$form_state) {
  bibdk_reservation_form_step_1_check_session();
   $form['anyField'] = array(
    '#type' => 'textfield',
    '#attributes' => array('placeholder' => t('Library name, zip or city')),
    '#default_value' => isset($form_state['values']['anyField']) ? $form_state['values']['anyField'][0] : NULL,
  );
  return $form;
}

function bibdk_reservation_form_step_1_validate($form, &$form_state) {
  $query = check_plain($form_state['values']['anyField']);
  if (empty($query)) {
    form_set_error('bibdk_reservation_form_step_1', t('You haven\'t entered any search criteria.'));
  }
  else {
    $result = bibdk_reservation_form_step_1_execute_search($query);
    $form_state['values']['result'] = $result;
  }
}

function bibdk_reservation_form_step_1_execute_search($query) {
  return bibdk_vejviser_execute_agency_search($query);
  
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
  }
  else if (isset($response['libraries'])) {
    $result = $response;
    return $result;
  }
}

/**
 * Checks session for already ordered materials
 */
function bibdk_reservation_form_step_1_check_session() {
  if (isset($_SESSION['orderedPids'])) {
    $pids = BibdkReservationOrderObject::GetObject()->getManifestationIds();
    foreach ($pids as $pid) {
      if (in_array($pid, $_SESSION['orderedPids'])) {
        drupal_set_message(t('You have already ordered this material once in this session'), 'warning');
        break;
      }
    }
  }
}