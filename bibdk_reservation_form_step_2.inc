<?php

/**
 * implements hook_form
 */
function bibdk_reservation_form_step_2($form, &$form_state) {
  //render manifestation
  $manifestation = BibdkReservationOrderObject::GetObject()->getManifestation();
  $form['manifestation'] = bibdk_reservation_render_custom_manifestation_view($manifestation);

  //render branch
  $branch = BibdkReservationOrderObject::GetObject()->getBranch();

  $form['branchdata'] = bibdk_reservation_render_custom_branch_view($branch, $form_state['step']); //bibdk_reservation_step_2_order_branch_fields(array(), $form_state);
  //$form['branchdata'] = bibdk_reservation_render_custom_branch_view($branch); //bibdk_reservation_step_2_order_branch_fields(array(), $form_state);
  $form['branchdata']['favouriteselector'] = bibdk_reservation_favourite_selector();
  $form['branchdata']['hidden_favourite'] = bibdk_reservation_hidden_favourite();

  $orderpolicy = bibdk_reservation_form_step_2_do_policycheck();

  if ($orderpolicy->getOrderPossible()) {
    $form['userdata'] = bibdk_reservation_step_2_user_form_fields(array(), $form_state);
    $form['manifestation_userdata'] = bibdk_reservation_step_2_order_form_fields(array(), $form_state);
    $form['manifestation']['date'] = bibdk_reservation_step_2_permanent_fields(array());
  }
  else {
    $form_state['blocked'] = TRUE;
    if ($error = $orderpolicy->getCheckOrderPolicyError()) {
      form_set_error('bibdk_reservation_form_step_2', t($error, array(), array('context' => 'bibdk_reservation:error')));
      return $form;
    }
    $reason = $orderpolicy->getOrderPossibleReason();
    $msgs = _bibdk_reservation_generate_policy_messages();
    if ($msgs) {
      $form['messages_wrapper'] = array(
        '#type' => 'fieldset',
      );
      $form['messages_wrapper']['messages'] = $msgs;
    }
    form_set_error('bibdk_reservation_form_step_2', t($reason, array(), array('context' => 'bibdk_reservation:error')));
  }
  return $form;
}

/** \brief Hidden field to determine whether favourite has changed
 *
 * @return array; form element hidden
 */
function bibdk_reservation_hidden_favourite() {
  return array(
    '#name' => 'favourite_selected',
    '#type' => 'hidden',
    '#default_value' => 0,
    '#attributes' => array('class' => array('hidden_selector')),
  );
}

/** \ brief Dropdown list to select another favourite library
 *
 * @return array; form element dropdown box
 */
function bibdk_reservation_favourite_selector() {
  $fav_options = bibdk_reservation_get_favourites_for_selector();
  // data check
  if (empty($fav_options)) {
    return;
  }

  $options[0] = t('Select another favourite library');
  $options += $fav_options;
  return array(
    '#type' => 'select',
    //'#title' =>
    '#options' => $options,
    '#default_value' => 0,
    '#attributes' => array('class' => array(drupal_html_class('reservation_favourite_selector'))),
  );
}

/** \brief Get options for favourite select list
 *
 * @return array; [agencyId]=>[branchName]
 */
function bibdk_reservation_get_favourites_for_selector() {
  global $language;
  $ret = array();
  $favourites = BibdkReservationOrderObject::GetObject()->getFavourites();
  foreach ($favourites as $favourite) {
    $branch = $favourite->getBranch();
    if (is_object($branch)) {
      $ret[$favourite->getAgencyId()] = $branch->getBranchShortName($language->language);
    }
  }
  return $ret;
}

function _bibdk_reservation_generate_policy_messages() {
  $msgs = array();
  if (BibdkOpenorderPolicyResponse::GetObject()->getLookUpUrl()) {
    $url = drupal_parse_url('reservations/catalogue_url/0');
    $msgs['lookUpUrl'] = array(
      '#theme' => 'link',
      '#text' => t('link_to_local_library_lookup_url', array(), array('context' => 'bibdk_reservation')),
      '#path' => $url['path'],
      '#options' => array(
        'attributes' => array(),
        'html' => FALSE,
      ),
    );
  }
  if (BibdkOpenorderPolicyResponse::GetObject()->getAgencyCatalogueUrl()) {
    $msgs['lookUpUrl'] = array(
      '#theme' => 'link',
      '#text' => t('link_to_local_library_catalogue_url', array(), array('context' => 'bibdk_reservation')),
      '#path' => BibdkOpenorderPolicyResponse::GetObject()->getAgencyCatalogueUrl(),
      '#options' => array(
        'attributes' => array(),
        'html' => FALSE,
      ),
    );
  }
  return $msgs;
}

function bibdk_reservation_manifestation_form_fields($form, &$form_state) {
  $form = array(
    '#type' => 'fieldset',
    '#title' => t('Selected Work', array(), array('context' => 'bibdk_reservation')),
  );
  $form['description'] = array(
    '#markup' => t('description_for_selected_work', array(), array('context' => 'bibdk_reservation')),
  );
  $manifestation = BibdkReservationOrderObject::GetObject()->getManifestation();
  $form['view_wrapper'] = array(
    '#type' => 'container',
  );
  $form['view_wrapper']['view'] = array(
    '#markup' => bibdk_reservation_manifestation_view($manifestation),
  );
  return $form;
}

/**
 * Render user fields;
 */
function bibdk_reservation_step_2_user_form_fields($form, $form_state) {
  //$agency = BibdkReservationOrderObject::GetObject()->getAgency();
  $userData = null;
  //$favourites = BibdkReservationOrderObject::GetObject()->getFavourites();
  $branch = BibdkReservationOrderObject::GetObject()->getBranch();
  /* foreach ($favourites as $branchid => $favourite) {
    if ( $branchid == $branch->branchId ) {
    $userData = $favourite->getUserData();
    }
    } */
  $fields = bibdk_reservation_get_agency_fields();
  //$form['userParameters'] = ting_agency_userdata_form($form, $form_state, $fields, $userData);
  global $user;
  if (ding_user_is_provider_user($user))
    $form['userParameters'] = bibdk_favourite_user_form_fields($form, $form_state, $branch->branchId);
  else
    $form['userParameters'] = ting_agency_userdata_form($form, $form_state, $fields);
  //$fields = bibdk_reservation_get_agency_fields()->getUserParameters();
  /* if (!isset($fields)) {
    return;
    }

    $favourites = BibdkReservationOrderObject::GetObject()->getFavourites();

    $branch = BibdkReservationOrderObject::GetObject()->getBranch();

    $userData = array();
    foreach ($favourites as $branchid => $favourite) {
    if ( $branchid == $branch->branchId ) {
    $userData = $favourite->getUserData();
    }
    }

    $form['userParameters'] = array(
    '#type' => 'fieldset',
    '#title' => t('User Parameters', array(), array('context' => 'bibdk_reservation')),
    '#attributes' => array('class' => array(drupal_html_class('bibdk_reservation_title'))),
    );

    // checkbox to unmask password fields
    $form['userParameters']['checkbox'] = array(
    '#title' => t('Show_masked_input'),
    '#type' => 'checkbox',
    '#attributes' => array('class' => array(drupal_html_class('bibdk_unmask_password_field'))),
    );


    $form['userParameters']['description'] = array(
    '#markup' => t('Description for user parameters', array(), array('context' => 'bibdk_reservation')),
    '#prefix' => '<div class="bibdk_clear">',
    '#suffix' => '</div>',
    );

    foreach ($fields as $userParameter) {
    $type = $userParameter['type'];
    $value = isset($userData[$type]) ? $userData[$type] : NULL;
    $form['userParameters'][$type] = array(
    //'#type' => isset($userParameter['field_type']) ? $userParameter['field_type'] : 'textfield',
    '#attributes' => array('class' => array((isset($userParameter['field_type']) && $userParameter['field_type'] == 'password') ? drupal_html_class('bibdk_password_field') : '')),
    '#type' => 'textfield',
    '#title' => isset($userParameter['field_name']) ? $userParameter['field_name'] : "",
    '#description' => isset($userParameter['field_description']) ? $userParameter['field_description'] : NULL,
    '#required' => $userParameter['required'],
    '#default_value' => isset($value) ? $value : "",
    );
    }
   */
  return $form;
}

/**
 * Render order fields;
 */
function bibdk_reservation_step_2_order_form_fields($form, $form_state) {
  $manifestation = BibdkReservationOrderObject::GetObject()->getManifestation();
  // Quickfix to make sure no Order Fields are present for articles.
  if ($manifestation->getSubType() == 'Tidsskriftsartikel' || $manifestation->getSubType() == 'Avisartikel') {
    return NULL;
  }
  $valid_fields = bibdk_reservation_get_agency_fields();
  if (empty($valid_fields)) {
    return $form;
  }

  $fields = bibdk_reservation_get_agency_fields()->getOrderParametersForType($manifestation->getOrsOrderMaterialType());
  if (!isset($fields)) {
    return $form;
  }

  $form['orderParameters'] = array(
    '#type' => 'fieldset',
    '#title' => t('Order Parameters', array(), array('context' => 'bibdk_reservation')),
  );
  $form['orderParameters']['description'] = array(
    '#type' => 'markup',
    '#markup' => t('Description for Order Parameters', array(), array('context' => 'bibdk_reservation')),
  );
  foreach ($fields as $itemParameter) {
    $type = $itemParameter['type'];
    $form['orderParameters'][$type] = array(
      '#type' => isset($itemParameter['field_type']) ? $itemParameter['field_type'] : 'textfield',
      '#title' => bibdk_reservation_get_agency_fields()->getOrderLabelFromType($type),
      '#required' => $itemParameter['required'],
      '#default_value' => (isset($form_state['values'][$type])) ? $form_state['values'][$type] : "",
    );
  }
  return $form;
}

/**
 * Render branch fields;
 */
function bibdk_reservation_step_2_order_branch_fields($form, $form_state) {

  $branch = BibdkReservationOrderObject::GetObject()->getBranch();

  $form['branchParameters'] = array(
    '#type' => 'fieldset',
    '#title' => t('Selected Branch', array(), array('context' => 'bibdk_reservation')),
  );

  $form['branchParameters']['description'] = array(
    '#type' => 'markup',
    '#attributes' => array('class' => array('form-wrapper', 'form-item')),
    '#markup' => t('Description for selected branch', array(), array('context' => 'bibdk_reservation')),
  );
  
  $moreinfo = theme('ting_agency_more_info', array('branch' => $branch,
    'openingHours' => theme('ting_agency_opening_hours', array('branch' => $branch,)),
    //'tools' => theme('ting_agency_tools', array('branch' => $branch)),
    'address' => theme('ting_agency_address', array('branch' => $branch)),
    'contact' => theme('ting_agency_contact', array('branch' => $branch)),
      )
  );
    
  //$classes[] = 'selected_branch';
  $classes = array();
  $classes[] = 'element-wrapper';
  $classes[] = 'favourite_' . $branch->branchId;
  $classes[] = (isset($branch->orderLibrary) && $branch->orderLibrary == 'TRUE') ? 'order-agency' : NULL;
  
  global $language;
  $form['branchParameters']['selectedBranch']['#markup'] =
      theme('ting_agency_library', array('branchName' => $branch->getBranchName($language->language), 'actions' => NULL, 'add_classes' => $classes, 'moreinfo' => $moreinfo, 'branchid' => $branch->branchId));

  return $form;
}

function bibdk_reservation_step_2_permanent_fields($form) {
  $timestamp = strtotime(date('Y-m-d') . '+ 3 month');
  $date = date('Y-m-d', $timestamp);
  $form['needBeforeDate'] = array(
    '#type' => 'date_select',
    '#title' => t('Is of no interest after', array(), array('context' => 'bibdk_reservation')),
    '#required' => TRUE,
    '#date_label_position' => 'within',
    '#date_format' => 'd-m-Y',
    '#default_value' => $date,
    '#date_year_range' => '0:+3',
  );
  return $form;
}

function bibdk_reservation_form_step_2_do_policycheck() {
  $pickUpAgencyId = BibdkReservationOrderObject::GetObject()->getBranchId();
  $pid = BibdkReservationOrderObject::GetObject()->getManifestationIds();
  return bibdk_openorder_do_checkOrderPolicy($pickUpAgencyId, $pid);
}

/*
 * Implements hook_form_validate
 * Validate user input
 */

function bibdk_reservation_form_step_2_validate($form, &$form_state) {
  if (!empty($form_state['values']['pincode']) && (!is_numeric($form_state['values']['pincode']) || strlen($form_state['values']['pincode']) != 4)) {
    form_set_error('pincode', t('Pincode needs to be a four digit number', array(), array('context' => 'bibdk_reservation:error')));
  }

  if (!empty($form_state['values']['cpr']) && !is_numeric($form_state['values']['cpr'])) {
    form_set_error('cpr', t('CPR-number needs to be a valid CPR-number', array(), array('context' => 'bibdk_reservation:error')));
  }

  if (!empty($form_state['values']['userMail']) && !valid_email_address($form_state['values']['userMail'])) {
    form_set_error('userEmail', t('Email needs to be a valid email', array(), array('context' => 'bibdk_reservation:error')));
  }

  if (!form_get_errors()) {
    bibdk_reservation_borchk_validate($form_state);
  }
}

/*
 * Implements hook_form_submit
 */

function bibdk_reservation_form_step_2_submit($form, &$form_state) {
  $userParameters = BibdkReservationOrderObject::GetObject()->getFields()->getUserParameters();

  // Parse userParameters
  foreach ($userParameters as $key => $userParameter) {
    $type = $userParameter['type'];
    $name = $userParameter['field_name'];
    $value = (isset($userParameter['field_type']) && $userParameter['field_type'] == 'password') ? preg_replace("/./", "*", $form_state['values'][$type]) : $form_state['values'][$type];
    if (!empty($value)) {
      $userData[$name] = $value;
      if (in_array($type, array('cpr', 'userId', 'cardno', 'customId'))) {
        $userOrderData['userId'] = isset($userOrderData['userId']) ? $userOrderData['userId'] : $form_state['values'][$type];
      }
      else {
        $userOrderData[$type] = $form_state['values'][$type];
      }
    }
  }

  $manifestation = BibdkReservationOrderObject::GetObject()->getManifestation();

  // Parse itemParameters
  $fields = bibdk_reservation_get_agency_fields()->getOrderParametersForType($manifestation->getOrsOrderMaterialType());
  if ($fields) {
    foreach ($fields as $itemParameter) {
      $type = $itemParameter['type'];
      if (isset($form_state['values'][$type])) {
        $orderParameters[$type] = $form_state['values'][$type];
      }
    }
  }
  if (isset($userOrderData['pincode'])) {
    unset($userOrderData['pincode']);
  }

  // Parse static fields
  $input_date = $form_state['values']['needBeforeDate'];
  $date = date("Y-m-d\TH:i:s", strtotime($input_date));
  $userData['Need before Date'] = implode('/', array_reverse(explode('-', $input_date)));

  // Save all user input
  BibdkReservationOrderObject::GetObject()->setUserData($userData);
  BibdkReservationOrderObject::GetObject()->setUserOrderData($userOrderData);
  BibdkReservationOrderObject::GetObject()->setNeedBeforeDate($date);
  if (isset($orderParameters)) {
    BibdkReservationOrderObject::GetObject()->setOrderParameters($orderParameters);
  }
}



/**
 * Validate up against Borchk
 */
function bibdk_reservation_borchk_validate($form_state) {
  if ($form_state['clicked_button']['#name'] == 'next') {
    // @TODO check that object is set
    $agencySettings = BibdkReservationOrderObject::GetObject()->getFields();

    $messages = bibdk_reservation_borchk_execute($form_state, $agencySettings);
    if (!isset($messages)) {
      return;
    }
    if (isset($messages['status'])) {
      drupal_set_message(t($messages['status'], array(), array('context' => 'bibdk_reservation:error')), 'warning');
    }
    if (isset($messages['error'])) {
      form_set_error('borchk', t($messages['error'], array(), array('context' => 'bibdk_reservation:error')));
    }
  }
}

/**
 * Execute service request to borchk
 */
function bibdk_reservation_borchk_execute($form_state, $agencySettings) {
  $acceptOrderFromUnknownUser = $agencySettings->acceptOrderFromUnknownUser();
  $acceptOrderAgencyOffline = $agencySettings->acceptOrderAgencyOffline();
  $isBorrowerCheckRequired = $agencySettings->isBorrowerCheckRequired();
  $messages = array(
    'error' => NULL,
    'status' => NULL
  );
  if (!$isBorrowerCheckRequired) {
    if (!$acceptOrderFromUnknownUser) {
      // just a notice - it just mean that the library reserves the right to trash the reservation.
      $messages['status'] = t('Library does not accept reservations form unknown users', array(), array('context' => 'bibdk_reservation:error'));
    }
  }
  else {
    $libraryCode = BibdkReservationOrderObject::GetObject()->getBranchId();
    $userIdKey = BibdkReservationOrderObject::GetObject()->getFields()->getUserIdKey();
    $userId = isset($form_state['values'][$userIdKey]) ? $form_state['values'][$userIdKey] : NULL;
    $userPincode = isset($form_state['values']['pincode']) ? $form_state['values']['pincode'] : NULL;
    $response = bibdk_borchk_request($userId, $userPincode, $libraryCode);

    $messages = bibdk_reservation_borchk_parse_response($response->borrowerCheckResponse->requestStatus->{'$'}, $acceptOrderFromUnknownUser, $acceptOrderAgencyOffline);
  }
  return $messages;
}

/**
 * Parse response from borchk and return messages
 *
 * @param string $response
 * @param string $acceptOrderFromUnknownUser
 * @param string $acceptOrderAgencyOffline
 * @return array
 */
function bibdk_reservation_borchk_parse_response($response, $acceptOrderFromUnknownUser, $acceptOrderAgencyOffline) {
  $message = array(
    'error' => NULL,
    'status' => NULL
  );
  switch ($response) {
    case 'ok' :
      return;
      break;
    case 'borrower_not_found' :
      if ($acceptOrderFromUnknownUser) {
        $message['status'] = t('Borrower not found, but Library accepts reservations from unknown users', array(), array('context' => 'bibdk_reservation'));
      }
      else {
        $message['error'] = t('User not found', array(), array('context' => 'bibdk_reservation'));
      }
      break;
    case 'error_in_request' :
    case 'service unavailable' :
    case 'borrowercheck_not_allowed' :
    default :
      //watchdog(WATCHDOG_ERROR, 'Unknown response from openOrder', array('response' => $response));
      if ($acceptOrderAgencyOffline) {
        $message['status'] = t('We could not check your status', array(), array('context' => 'bibdk_reservation'));
      }
      else {
        $message['error'] = t('Service Unavailable. We can not make reservation right know', array(), array('context' => 'bibdk_reservation'));
      }
      break;
  }
  return $message;
}

/**
 * Get fields required by Agency
 *
 * @return
 */
function bibdk_reservation_get_agency_fields() {
  $agency = BibdkReservationOrderObject::GetObject()->getAgency();
  if (!is_object($agency)) {
    return NULL;
  }
  $fields = $agency->getAgencyFields();
  if (!empty($fields)) {
    BibdkReservationOrderObject::GetObject()->setFields($fields);
  }

  return $fields;
}
