<?php

function bibdk_reservation_form_step_1($form, &$form_state) {
    $form['openagency_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#size' => 5,
    '#default_value' => isset($form_state['values']['openagency_name']) ? $form_state['values']['openagency_name'] : '',
  );
  $form['openagency_zip'] = array(
    '#type' => 'textfield',
    '#title' => t('Zip'),
    '#size' => 5,
    '#default_value' => isset($form_state['values']['openagency_zip']) ? $form_state['values']['openagency_zip'] : '',
  );
  $form['openagency_city'] = array(
    '#type' => 'textfield',
    '#title' => t('City'),
    '#size' => 5,
    '#default_value' => isset($form_state['values']['openagency_city']) ? $form_state['values']['openagency_city'] : '',
  );
  return $form;
}

function bibdk_reservation_form_step_1_validate($form, &$form_state){
  if(empty($form_state['values']['openagency_name']) && empty($form_state['values']['openagency_zip']) && empty($form_state['values']['openagency_city'])){
    form_set_error('bibdk_reservation_form_step_1', t('At least one kind of information should be provided'));
  }
}