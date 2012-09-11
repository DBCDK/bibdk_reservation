<?php

function bibdk_reservation_form_step_1($form) {
  drupal_set_message('step 1');
  $form['openagency_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#size' => 5,
  );
  $form['openagency_zip'] = array(
    '#type' => 'textfield',
    '#title' => t('Zip'),
    '#size' => 5,
  );
  $form['openagency_city'] = array(
    '#type' => 'textfield',
    '#title' => t('City'),
    '#size' => 5,
  );
/*
  $form['openagency_submit'] = array(
    '#type' => 'submit',
    '#value' => t('Search'),
    '#executes_submit_callback' => TRUE,
  );*/
  return $form;
}