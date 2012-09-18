<?php

function bibdk_reservation_form_step_5($form, &$form_state){
  $response = bibdk_reservation_form_step_5_do_order();
  dpm($response);
  $result = bibdk_reservation_form_step_5_check_response($response);
  
}

function bibdk_reservation_form_step_5_do_order(){
  drupal_load('module', 'bibdk_openorder');
  #$respons = bibdk_openorder_do_placeOrder($pid, $pickUpAgencyId, $userId);
  $respons = bibdk_openorder_do_placeOrder(NULL, NULL, NULL);
  return $respons;
}

function bibdk_reservation_form_step_5_check_response($response) {
  if(isset($response->placeOrderResponse->orderPlaced)) {
    $orderId = $response->placeOrderResponse->orderPlaced->orderId;
    $result = array('orderId' => $orderId);
  } else if(isset($response->placeOrderResponse->orderNotPlaced)){
    $error = $response->placeOrderResponse->orderNotPlaced->placeOrderError;
    dpm($error);
  }
  return $result;
}