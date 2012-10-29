<?php

function bibdk_reservation_form_step_5($form, &$form_state) {
  $result = bibdk_reservation_form_step_5_do_order();
  if (isset($result['error'])) {
    drupal_set_message($result['error'], 'error');
  }
  else {
    $id = $result['orderId'];
    return bibdk_reservation_form_step_5_form($form, $id);
  }
}

function bibdk_reservation_form_step_5_form($form, $id) {
  $manifestation_object = BibdkReservationOrderObject::GetObject()->getManifestation();
  $manifestation_view = bibdk_reservation_manifestation_view($manifestation_object);
  $user = BibdkReservationOrderObject::GetObject()->getUserData();
  $branch = BibdkReservationOrderObject::GetObject()->getBranch();
  $orderParamWithLabel = null;
  $orderParameters = BibdkReservationOrderObject::GetObject()->getOrderParameters();
  if (isset($orderParameters)){
  foreach ($orderParameters as $type => $orderParameter){
    $orderParamWithLabel[bibdk_reservation_get_agency_fields()->getOrderLabelFromType($type)] = $orderParameter;
  }
  }
  $needBeforeDate = t('Is not needed after: ') . BibdkReservationOrderObject::GetObject()->getNeedBeforDate();
  $orderId = $id;
  
  $form['resume'] = array(
    '#markup' => theme('bibdk_reservation_receipt', array('user' => $user, 'orderParameters' => $orderParamWithLabel, 'branch' => $branch, 'manifestation' => $manifestation_view, 'orderId' => $orderId, 'needBeforeDate' => $needBeforeDate)),
  );
  $form['print'] = array(
    '#type' => 'button',
    '#value' => t('Print'),
  );
  return $form;
}

function bibdk_reservation_form_step_5_do_order() {
  $params = array();
  $userdata = BibdkReservationOrderObject::GetObject()->getUserOrderData();
  $orderdata = BibdkReservationOrderObject::GetObject()->getOrderParameters();
  $params += (isset($userdata)) ? $userdata : array();
  $params += (isset($orderdata)) ? $orderdata : array();
  $params += array(
    'pid' => BibdkReservationOrderObject::GetObject()->getManifestationIds(),
    'pickUpAgencyId' => BibdkReservationOrderObject::GetObject()->getBranchId(), 
    'needBeforeDate' => BibdkReservationOrderObject::GetObject()->getNeedBeforDate(),
  );
  bibdk_reservation_form_step_5_register_pid();
  $response = bibdk_openorder_do_placeOrder($params);
  if (isset($response->placeOrderResponse->orderNotPlaced)) {
    $error = $response->placeOrderResponse->orderNotPlaced->placeOrderError->{'$'};
    $pid = BibdkReservationOrderObject::GetObject()->getManifestationIds();
    $pickUpAgencyId = BibdkReservationOrderObject::GetObject()->getBranchId();
    watchdog('bibdk_reservation_form_step_5', 'openOrder responded with an error: ' . $error . '. pid: #pid agencyId: #agency', array('#pid' => $pid[0], '#agency' => $pickUpAgencyId), WATCHDOG_ERROR);
    $result = array('error' => t('An error happened and the order could not be completed.'));
  }
  else {
    $result = bibdk_reservation_form_step_5_handle_response($response);
  }
  return $result;
}

function bibdk_reservation_form_step_5_handle_response($response) {
  if (isset($response->placeOrderResponse->orderPlaced)) {
    $orderId = $response->placeOrderResponse->orderPlaced->orderId->{'$'};
    $result = array('orderId' => $orderId);
    $type = BibdkReservationOrderObject::GetObject()->getManifestation()->getType();
    if(strtolower($type) != 'avis' && strtolower($type) != 'tidsskrift'){
      bibdk_reservation_form_step_5_register_pid();
    }
  }
  return $result;
}

function bibdk_reservation_form_step_5_register_pid(){
  $pid = BibdkReservationOrderObject::GetObject()->getManifestationIds();
  if(array_key_exists('orderedPids', $_SESSION)){
    $_SESSION['orderedPids'] = array_merge($_SESSION['orderedPids'], $pid);
  } else {
    $_SESSION['orderedPids'] = $pid;
  }
}