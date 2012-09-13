<?php

function bibdk_reservation_form_step_4($form, &$form_state) {
  dpm($form_state);
  $form['resume'] = array(
    '#value' => t('Select'),
    '#name' => $branch->branchId,
    '#prefix' => theme('bibdk_reservation_agency', array('agency' => $branch)),
    '#suffix' => '<hr/></div>',
    '#type' => 'submit',
    '#submit' => array('bibdk_reservation_wizard_next_submit', 'bibdk_reservation_form_step_2_update_orderinfo'),
  );
  return $form;
}