<?php

function bibdk_reservation_form_step_2($form, &$form_state) {
  $result = $form_state['step_information'][1]['stored_values']['result'];
  $form = _bibdk_reservation_form_step_2_generate_form($form, $result);
  return $form;
}

function _bibdk_reservation_form_step_2_generate_form($form, $result){
  foreach ($result['libraries'] as $branch) {
      $form[$branch->branchId]['button'] = array(
        '#value' => t('Select'),
        '#name' => $branch->branchId, 
        '#prefix' => theme('bibdk_reservation_agency', array('agency' => $branch)),
        '#suffix' => '<hr/></div>',
        '#type' => 'submit',
        '#submit' => array('bibdk_reservation_wizard_next_submit'),
        '#branch' => $branch,
      );
  }
  return $form;
}

function bibdk_reservation_form_step_2_submit($form, $form_state) {
  BibdkReservationOrderObject::GetObject()->setBranch($form_state['clicked_button']['#branch']);
}