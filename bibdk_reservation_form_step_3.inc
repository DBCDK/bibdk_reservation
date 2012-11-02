<?php

function bibdk_reservation_form_step_3($form, &$form_state) {
  $manifestation_object = BibdkReservationOrderObject::GetObject()->getManifestation();
  $manifestation_view = bibdk_reservation_manifestation_view($manifestation_object);
  $user = BibdkReservationOrderObject::GetObject()->getUserData();
  $branch = BibdkReservationOrderObject::GetObject()->getBranch();
  $orderParamWithLabel = array();
  $orderParameters = BibdkReservationOrderObject::GetObject()->getOrderParameters();
  if (isset($orderParameters)){
    foreach ($orderParameters as $type => $orderParameter){
    $orderParamWithLabel[bibdk_reservation_get_agency_fields()->getOrderLabelFromType($type)] = $orderParameter;
  }
  }
  $form['resume'] = array(
    '#markup' => theme('bibdk_reservation_resume', array('user' => $user, 'orderParameters' => $orderParamWithLabel, 'branch' => $branch, 'manifestation' => $manifestation_view)),
  );
  return $form;
}