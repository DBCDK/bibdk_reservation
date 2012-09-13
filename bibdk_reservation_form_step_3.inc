<?php

/*
 * Form for user input on Step 3
 */

function bibdk_reservation_form_step_3($form, &$form_state) {

  $fields = bibdk_reservation_get_agency_fields();
  foreach ($fields->getUserParameters() as $userParameter) {
    $type = $userParameter['type'];
    $form[$type] = array(
      '#type' => isset($userParameter['field_type']) ? $userParameter['field_type'] : 'textfield',
      '#title' => $userParameter['field_name'],
      '#description' => isset($userParameter['field_description']) ? $userParameter['field_description'] : NULL,
      '#required' => $userParameter['required'],
      '#default_value' => (isset($form_state['values'][$type])) ? $form_state['values'][$type] : "",
    );
  }
  return $form;
}

/*
 * Implements hook_form_validate
 * Validate user input
 */
function bibdk_reservation_form_step_3_validate($form, &$form_state) {
  if (!empty($form_state['values']['pincode']) && (!is_numeric($form_state['values']['pincode']) || strlen($form_state['values']['pincode']) != 4)) {
    form_set_error('pincode', t('Pincode needs to be a four diggit number'));
  }

  if (!empty($form_state['values']['cpr']) && !is_numeric($form_state['values']['cpr'])) {
    form_set_error('cpr', t('CPR-number needs to be a valid CPR-number'));
  }

  if (!empty($form_state['values']['userMail']) && !valid_email_address($form_state['values']['userMail'])) {
    form_set_error('cpr', t('email needs to be a valid email'));
  }
  if (!form_get_errors()) {
    bibdk_reservation_borchk_validate($form_state);
  }
}
/*
 * Implements hook_form_submit
 */

function bibdk_reservation_form_step_3_submit($form, &$form_state) {
  $userParameters = BibdkReservationOrderObject::GetObject()->getFields()->getUserParameters();

  foreach ($userParameters as $key => $userParameter) {
    $type = $userParameter['type'];
    $name = $userParameter['field_name'];
    $value = (isset($userParameter['field_type']) && $userParameter['field_type'] == 'password') ? preg_replace("/./", "*", $form_state['values'][$type]) : $form_state['values'][$type];
    if (!empty($value)) {
      $userData[$name] = $value;
    }
  }
  BibdkReservationOrderObject::GetObject()->setUserData($userData);
}

/**
 * Validate up against Borchk
 */
function bibdk_reservation_borchk_validate($form_state) {
  $response = bibdk_reservation_execute_borchk_service($form_state);
  switch ($response->borrowerCheckResponse->requestStatus) {
    case 'ok' :
      return;
      break;
    case 'borrower_not_found' :
      form_set_error("Library User", "Userid and/or pincode is wrong");
      break;
    case 'error_in_request' :
      form_set_error("Library User", "Unknown Error has happened");
      break;
  }
}

/**
 * Execute service request to borchk
 */
function bibdk_reservation_execute_borchk_service($form_state) {
  $libraryCode = BibdkReservationOrderObject::GetObject()->getBranchId();
  $userIdKey = BibdkReservationOrderObject::GetObject()->getFields()->getUserIdKey();
  $userId = isset($form_state['values'][$userIdKey]) ? $form_state['values'][$userIdKey] : NULL;
  $userPincode = isset($form_state['values']['pincode']) ? $form_state['values']['pincode'] : NULL;

  return bibdk_borchk_request($userId, $userPincode, $libraryCode);
}

/**
 * Get fields required by Agency
 * @return \bibdkReservationUserInput
 */
function bibdk_reservation_get_agency_fields() {

  $agencyId = BibdkReservationOrderObject::GetObject()->getBranchId();

  $response = bibdk_reservation_execute_agency_service($agencyId);

  $fields = new bibdkReservationAgencyFields($response);

  BibdkReservationOrderObject::GetObject()->setFields($fields);

  return $fields;
}

/**
 * Get result from OpenAgency ServiceRequest
 * 
 * @param string $agencyId
 * @return array
 */
function bibdk_reservation_execute_agency_service($agencyId) {

  $client = new ting_client_class();
  $service = 'userOrderParameters';

  $response = $client->do_agency_service(array('agencyId' => $agencyId, 'service' => $service));

  return parse_agency_service_response($response);
}

/**
 * Parse json response from ServiceRequest
 * @param json $response
 * @return array
 */
function parse_agency_service_response($response) {
  $result = array();
  if (isset($response->userParameter)) {
    foreach ($response->userParameter as $userParameter) {

      $result['userParameters'][] = array(
        'type' => $userParameter->userParameterType->{'$'},
        'required' => $userParameter->parameterRequired->{'$'},
      );
    }
  }
  if (isset($response->userIdTxt)) {
    foreach ($response->userIdTxt as $txt) {
      $result['userIdTxt'][$txt->{'@language'}->{'$'}] = $txt->{'$'};
    }
  }
  if (isset($response->orderParameters)) {
    foreach ($response->orderParameters as $key => $orderParameter) {
      $result['orderParameters'][$key] = array(
        'materialType' => $orderParameter->orderMaterialType->{'$'},
        'orderType' => $orderParameter->orderType->{'$'},
      );
      if (isset($orderParameter->itemParameter)) {
        $itemParameters = array();
        foreach ($orderParameter->itemParameter as $itemParameter) {
          $itemParameters[] = array(
            'type' => $itemParameter->itemParameterType->{'$'},
            'required' => $itemParameter->parameterRequired->{'$'},
          );
        }
        $result['orderParameters'][$key]['itemParameters'] = $itemParameters;
      }
    }
  }
  return $result;
}
