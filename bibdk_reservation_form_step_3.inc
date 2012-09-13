<?php

/*
 * Form for user input on Step 3
 */
function bibdk_reservation_form_step_3($form, &$form_state) {
  $userData = bibdk_reservation_execute_agency_service("DK-810010");
  foreach($userData->getUserParameters() as $userParameter){
    $type = $userParameter['type'];
    $required = $userParameter['required'];
    $form[$type] = array(
      '#type' => isset($userParameter['field_type']) ? $userParameter['field_type'] : 'textfield',
      '#title' => $userParameter['field_name'],
      '#description' => isset($userParameter['field_description']) ? $userParameter['field_description'] : NULL,
      '#required' => $required,
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
  if (!form_get_errors()){
    bibdk_reservation_borchk($form_state);
  }
}
function bibdk_reservation_form_step_3_submit($form, &$form_state){ 
  foreach($form_state['values'] as $key => $value){
    if($form[$key]['#type'] != 'password'){
      $userData[$key] = $value;
    }
  }
  BibdkReservationOrderObject::GetObject()->setUserData($userData);
}


function bibdk_reservation_borchk($form_state){
  $response = bibdk_reservation_execute_borchk_service($form_state);
  switch($response->borrowerCheckResponse->requestStatus){
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


function bibdk_reservation_execute_borchk_service($form_state){
  $libraryCode = BibdkReservationOrderObject::GetObject()->getAgencyId();
  $userId = $form_state['values']['userId'];
  $userPincode = $form_state['values']['pincode'];
  $responce = bibdk_borchk_request($userId, $userPincode, $libraryCode);
  return $responce;
}

/**
 * Get result from OpenAgency ServiceRequest
 * 
 * @param string $agencyId
 * @return \bibdkReservationUserInput
 */
function bibdk_reservation_execute_agency_service($agencyId) {

  module_load_include('inc', 'ting_openformat', 'ting_openformat.search_client');
  $client = new ting_client_class();

  $service = 'userOrderParameters';
  
  $response = $client->do_agency_service(array('agencyId' => $agencyId, 'service' => $service));
  $response = _parse_agency_service_response($response);
  
  return new bibdkReservationUserInput($response);
}

/**
 * Parse json response from ServiceRequest
 * @param json $response
 * @return array
 */
function _parse_agency_service_response($response) {
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

/**
 * Handle settings for Agency specific form fields
 * 
 * This function is prepared to manifestation specific userinput
 */
class bibdkReservationUserInput {

  private $userParameters, $userIdTxt, $orderParameters;

  public function __construct($response) {
    foreach($response as $key => $value){
      $this->$key = $value;
    }
    foreach($this->userParameters as $key => $element)
      $this->userParameters[$key] += $this->_getSettingsFromType ($element['type']);
  }
  
  public function getUserParameters(){
    return $this->userParameters;
  }
  
private function _getSettingsFromType($type){
  $settings = array();
  switch ($type) {
    case 'cpr':
      $settings = array(
        'field_name' => t('CPR-number'),
        'field_type' => 'password',
      );
      break;
    case 'userId':
      $settings = array(
        'field_name' => t('User ID'),
        'field_description' => isset($this->userIdTxt) ? implode(", ", $this->userIdTxt) : NULL,
      );
      break;
    case 'barcode':
      $settings = array(
        'field_name' => t('Barcode'),
        'field_type' => 'password',
      );
      break;
    case 'cardno':
      $settings = array(
        'field_name' => t('Card number'),
        'field_type' => 'password',
      );
      break;
    case 'pincode':
      $settings = array(
        'field_name' => t('Pincode'),
        'field_type' => 'password',
      );
      break;
    case 'customId':
      $settings = array(
        'field_name' => t('Custom ID'),
        'field_type' => 'password',
      );
      break;
    case 'userDateOfBirth':
      $settings = array(
        'field_name' => t('Date of birth'),
      );
      break;
    case 'userName':
      $settings = array(
        'field_name' => t('Name'),
      );
      break;
    case 'userAddress':
      $settings = array(
        'field_name' => t('Adress, postal code, town/city'),
      );
      break;
    case 'userMail':
      $settings = array(
        'field_name' => t('E-mail'),
      );
      break;
    case 'userTelephone':
      $settings = array(
        'field_name' => t('Phone number'),
      );
      break;

    default:
      break;
  }
  
  return $settings;
  
}
}