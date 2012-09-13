<?php

function bibdk_reservation_form_step_4($form, &$form_state) {
  
  //TODO implement $work
  //$work = BibdkReservationOrderObject::GetObject()->getManifestationIds();
  $user = BibdkReservationOrderObject::GetObject()->getUserData();
  $branch = BibdkReservationOrderObject::GetObject()->getBranch();
  
  $form['resume'] = array(
    '#markup' => theme('bibdk_reservation_resume', array('user' => $user, 'branch' => $branch)),
  );
  return $form;
}