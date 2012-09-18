<?php

function bibdk_reservation_form_step_5($form, &$form_state) {
  $result = bibdk_reservation_form_step_5_do_order();
  if (isset($result['error'])) {
    drupal_set_message($result['error'], 'error');
  }
  else {
    $id = $result['orderId'];
    drupal_set_message(t('Success! ') . $id, 'status');
  }
}

function bibdk_reservation_form_step_5_do_order() {
  drupal_load('module', 'bibdk_openorder');
  
  $pid = BibdkReservationOrderObject::GetObject()->getManifestationIds();
  $pickUpAgencyId = BibdkReservationOrderObject::GetObject()->getBranchId();
  $userData = BibdkReservationOrderObject::GetObject()->getUserOrderData();
  $userId = $userData[BibdkReservationOrderObject::GetObject()->getFields()->getUserIdKey()];

  $response = bibdk_openorder_do_placeOrder($pid, $pickUpAgencyId, $userId);
  dpm($response);
  if (isset($response->placeOrderResponse->orderNotPlaced)) {
    $error = $response->placeOrderResponse->orderNotPlaced->placeOrderError->{'$'};
    watchdog('bibdk_reservation_form_step_5', 'openOrder responded with an error: ' . $error . '. pid: #pid agencyId: #agency', array('#pid' => $pid, '#agency' => $pickUpAgencyId), WATCHDOG_ERROR);
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
  }
  return $result;
}