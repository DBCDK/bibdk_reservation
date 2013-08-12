<?php
function bibdk_reservation_form_step_4($form, &$form_state) {

  $orderId = BibdkReservationOrderObject::GetObject()->getOrderId();
  return bibdk_reservation_form_step_4_form($form,$form_state, $orderId);
}

function bibdk_reservation_form_step_4_form($form, &$form_state, $orderId) {

  // render OrderID
  $form['orderId'] = array(
    '#type' => 'fieldset',
    '#title' => t('Your order is complete', array(), array('context'=>'bibdk_reservation')),
    '#attributes' => array('class' => array('bottom-space')),
  );

  $form['orderId']['container']['markup'] = bibdk_reservation_convert_array_to_table(array(
      t('Your order ID', array(), array('context'=>'bibdk_reservation')) => $orderId,
    ));

  // Render the same fields as on step 3
  $form = bibdk_reservation_form_step_3($form, $form_state);

  return $form;
}

function bibdk_reservation_form_step_4_do_order() {
  global $language;
  $lang = $language->language;
  $params = array();
  $pids = BibdkReservationOrderObject::GetObject()->getManifestationIds();
  $userdata = BibdkReservationOrderObject::GetObject()->getUserOrderData();
  $orderdata = BibdkReservationOrderObject::GetObject()->getOrderParameters();

  $articleDirect = BibdkReservationOrderObject::GetObject()->getArticleDirect();
  if (!empty($articleDirect)) {
    $params += array('articleDirect'=>'pickup');
  }
  
  $params += (isset($userdata)) ? $userdata : array();
  $params += (isset($orderdata)) ? $orderdata : array();
  $params += array(
    'pid' => $pids,
    'pickUpAgencyId' => BibdkReservationOrderObject::GetObject()->getBranchId(),
    'needBeforeDate' => BibdkReservationOrderObject::GetObject()->getNeedBeforDate(),
  );
  
  bibdk_reservation_form_step_4_register_pid();
  $response = bibdk_openorder_do_placeOrder($params);
  if (isset($response->placeOrderResponse->orderNotPlaced)) {
    $error = $response->placeOrderResponse->orderNotPlaced->placeOrderError->{'$'};
    $pid = BibdkReservationOrderObject::GetObject()->getManifestationIds();
    $pickUpAgencyId = BibdkReservationOrderObject::GetObject()->getBranchId();
    watchdog('bibdk_reservation_form_step_4', 'openOrder responded with an error: ' . $error . '. pid: #pid agencyId: #agency', array('#pid' => $pid[0], '#agency' => $pickUpAgencyId), WATCHDOG_ERROR);
    $result = array('error' => t('An error happened and the order could not be completed.', array(), array('context'=>'bibdk_reservation')));
  }
  else {
    drupal_set_message(check_plain(BibdkReservationOrderObject::GetObject()->getBranch()->getIllOrderReceiptText($lang)), 'status');
    $result = bibdk_reservation_form_step_4_handle_response($response);
    module_invoke_all('bibdk_reservation_complete', $pids, $result);
  }
  return $result;
}

function bibdk_reservation_form_step_4_handle_response($response) {
  if (isset($response->placeOrderResponse->orderPlaced)) {
    $orderId = $response->placeOrderResponse->orderPlaced->orderId->{'$'};
    $result = array('orderId' => $orderId);
    $type = BibdkReservationOrderObject::GetObject()->getManifestation()->getType();
    if(strtolower($type) != 'avis' && strtolower($type) != 'tidsskrift'){
      bibdk_reservation_form_step_4_register_pid();
    }
  }
  return $result;
}

function bibdk_reservation_form_step_4_register_pid(){
  $pid = BibdkReservationOrderObject::GetObject()->getManifestationIds();
  if(array_key_exists('orderedPids', $_SESSION)){
    $_SESSION['orderedPids'] = array_merge($_SESSION['orderedPids'], $pid);
  } else {
    $_SESSION['orderedPids'] = $pid;
  }
}
