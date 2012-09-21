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
  
  $needBeforeDate = t('Is not needed after: ') . BibdkReservationOrderObject::GetObject()->getNeedBeforDate();
  $orderId = $id;
  
  $form['resume'] = array(
    '#markup' => theme('bibdk_reservation_receipt', array('user' => $user, 'branch' => $branch, 'manifestation' => $manifestation_view, 'orderId' => $orderId, 'needBeforeDate' => $needBeforeDate)),
  );
  $form['print'] = array(
    '#type' => 'button',
    '#value' => t('Print'),
    '#markup' => '<a href="#" />b</a>',
  );
  return $form;
}

function bibdk_reservation_form_step_5_do_order() {
  drupal_load('module', 'bibdk_openorder');

  $pid = BibdkReservationOrderObject::GetObject()->getManifestationIds();
  $pickUpAgencyId = BibdkReservationOrderObject::GetObject()->getBranchId();
  $userData = BibdkReservationOrderObject::GetObject()->getUserOrderData();
  $userId = $userData['userId'];

  $response = bibdk_openorder_do_placeOrder($pid, $pickUpAgencyId, $userId);

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