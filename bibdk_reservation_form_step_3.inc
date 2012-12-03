<?php

function bibdk_reservation_form_step_3($form, &$form_state) {

  //Render user parameters
  $form['userParameters'] = array(
    '#type' => 'fieldset',
    '#title' => t('User Parameters', array(), array('context'=>'bibdk_reservation')),
  );
  $user_data_array = BibdkReservationOrderObject::GetObject()->getUserData();
  $form['userParameters']['table'] = bibdk_reservation_convert_array_to_table($user_data_array);

  //render manifestation
  $manifestation = BibdkReservationOrderObject::GetObject()->getManifestation();
  $form['manifestation'] = bibdk_reservation_render_custom_manifestation_view($manifestation);

  //render branch
  $branch = BibdkReservationOrderObject::GetObject()->getBranch();
  $form['branchdata'] = bibdk_reservation_render_custom_branch_view($branch); //bibdk_reservation_step_2_order_branch_fields(array(), $form_state);

  // Render order parameters
  $orderParamWithLabel = array();
  $orderParameters = BibdkReservationOrderObject::GetObject()->getOrderParameters();
  if (isset($orderParameters)) {
    foreach ($orderParameters as $type => $orderParameter) {
      $orderParamWithLabel[bibdk_reservation_get_agency_fields()->getOrderLabelFromType($type)] = $orderParameter;
    }
    $form['orderParameters'] = array(
      '#type' => 'fieldset',
      '#title' => t('Order Parameters', array(), array('context'=>'bibdk_reservation')),
    );

    //Render User parameters
    $form['userParameters']['table'] = bibdk_reservation_convert_array_to_table($orderParamWithLabel);
  }
  return $form;
}

/**
 * Implements hook_form_validate
 */
function bibdk_reservation_form_step_3_validate($form, &$form_state) {
  $orderId = BibdkReservationOrderObject::GetObject()->getOrderId();
  if (!isset($orderId)) {
    $result = bibdk_reservation_form_step_4_do_order();
    if (isset($result['error'])) {
      form_set_error('form', t($result['error'], array(), array('context' => 'bibdk_reservation:error')));
    }
    else {
      $orderId = $result['orderId'];
      BibdkReservationOrderObject::GetObject()->setOrderId($orderId);
    }
  }
}

function bibdk_reservation_render_custom_branch_view($branch) {
  $name = $branch->branchName;
  $address = $branch->postalAddress;
  $postalCode = $branch->postalCode;
  $city = $branch->city;

  $markup = theme('bibdk_reservation_agency', array(
    'name' => $name,
    'address' => $address,
    'postalCode' => $postalCode,
    'city' => $city,
      ));
  $form = array(
    '#type' => 'fieldset',
    '#title' => t('Branch', array(), array('context'=>'bibdk_reservation')),
  );

  $form['markup'] = array(
    '#markup' => $markup,
    '#attributes' => array('class' => array('form-item'))
  );

  return $form;
}

function bibdk_reservation_render_custom_manifestation_view(manifestation $manifestation) {
  $title = $manifestation->getTitle();
  $contribs = $manifestation->getContributors();
  $author = isset($contribs) && is_array($contribs) ? implode(', ', $contribs) : NULL;
  $type = $manifestation->getSubType();

  $markup = theme('bibdk_reservation_manifestation', array(
    'title' => $title,
    'author' => $author,
    'type' => $type,
      ));

  $form = array(
    '#type' => 'fieldset',
    '#title' => t('Manifestation', array(), array('context'=>'bibdk_reservation')),
  );

  $form['markup'] = array(
    '#markup' => $markup,
  );

  return $form;
}

function bibdk_reservation_convert_array_to_table($array) {
  foreach ($array as $key => $value) {
    $rows[]['data'] = array(
      array('data' => $key, 'class' => array(''), 'header' => TRUE),
      array('data' => $value, 'class' => array('')),
    );
  }
  $table = array(
    '#theme' => 'table',
    '#rows' => $rows,
    '#empty' => t('No userdata', array(), array('context'=>'bibdk_reservation')),
    '#attributes' => array('class' => 'table-data'),
  );

  return $table;
}
