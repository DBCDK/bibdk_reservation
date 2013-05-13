<?php

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/** Implements form_FORM_ID_alter (bibdk_reservation_create_wizard_form($form, &$form_state)
 * 
 */
function bibdk_reservation_form_bibdk_reservation_create_wizard_form_alter(&$form, &$form_state) {
  // check if this is copy service
  $sbkopi = BibdkReservationOrderObject::GetObject()->getSbKopi();
  if (!$sbkopi) {
    return;
  }

  // disable step 1 in step_guide
  $form['step_guide']['1']['#disabled'] = TRUE;
  // only validate copy service in step 2
  if ($form_state['step'] == 2) {
    array_unshift($form['#validate'], 'bibdk_reservation_sb_kopi_validate_wrapper');
  }
  // do not use required fields. We do our own validation
  $form['userdata']['userParameters']['userfields']['wrapper']['userId']['#required'] = FALSE;
  $form['userdata']['userParameters']['userfields']['wrapper']['pincode']['#required'] = FALSE;
}

function bibdk_reservation_sb_kopi_new_user_fields() {
  $new = array('bestil_kopi_cpr_number', 'bestil_kopi_firstname', 'bestil_kopi_lastname', 'bestil_kopi_e_mail');
  return $new;
}

function bibdk_reservation_sb_kopi_old_user_fields() {
  $old = array('pincode', 'userId');
  return $old;
}

function bibdk_reservation_sb_kopi_validate_wrapper($form, &$form_state) {
  // clear error messages
  //drupal_get_messages();  

  $errors = bibdk_reservation_sbkopi_validate($form, $form_state);
  if (is_array($errors)) {
    foreach ($errors as $field => $message)
      form_set_error($field, t($message, array(), array('context' => 'bibdk_reservation:error')));
  }
}

function bibdk_reservation_sbkopi_validate($form, &$form_state) {
  // $new represents a new user, $old represents an already registred user
  $new = bibdk_reservation_sb_kopi_new_user_fields();
  $old = bibdk_reservation_sb_kopi_old_user_fields();

  // check if new has been filled out
  foreach ($old as $key) {
    if (!(empty($form_state['values'][$key]))) {
      continue;
    }
    else {
      $old = FALSE;
      break;
    }
  }

// check if old has been filled out
  foreach ($new as $key) {
    if (!empty($form_state['values'][$key])) {
      continue;
    }
    else {
      $new = FALSE;
      break;
    }
  }


  if (!$old && !$new) {
    return array('field' => 'form', 'message' => 'sb_kopi_fill_out_the_form_please');
  }


  if (is_array($old) && is_array($new)) {
    // both forms has been filled out; use the one for already registred users ($old)
    foreach ($new as $key) {
      $form_state['values'][$key] = NULL;
    }
  }

  // now check that the form that has been filled out was set correct
  if ($old && !$new) {
    if (isset($form_state['values']['pincode']) && isset($form_state['values']['userId'])) {
      if (!empty($form_state['values']['pincode']) && (!is_numeric($form_state['values']['pincode']) || strlen($form_state['values']['pincode']) != 4)) {
        return array('pincode', 'Pincode needs to be a four digit number');
      }
      if (!empty($form_state['values']['userId']) && !is_numeric($form_state['values']['userId'])) {
        return array('userId', 'CPR-number needs to be a valid CPR-number');
      }
    }
  }


  if (!$old && $new) {
    if (isset($form_state['values']['bestil_kopi_cpr_number'])) {
      // cpr should be numeric
      if (!is_numeric($form_state['values']['bestil_kopi_cpr_number'])) {
        return array('bestil_kopi_cpr_numer', 'CPR-number needs to be a valid CPR-number');
      }
    }
  }

  // set the type of user
  if ($old) {
    BibdkReservationOrderObject::GetObject()->setSbKopiUser('OLD');
  }
  elseif ($new) {
    bibdkReservationOrderObject::GetObject()->setSbKopiUser('NEW');
  }
}

function bibdk_reservation_sanitize_for_order_copy(&$form, $form_state) {
  // unset elements that are not needed for sb_copy service
  _bibdk_reservation_sbkopi_unset_elements($form, $form_state);

  $element = array(
    '#type' => 'link',
    '#title' => t('already_member', array(), array('context' => 'bibdk_reservation')),
    '#href' => '#',
    '#attributes' => array('class' => array(drupal_html_class('switch_views'))),
    '#weight' => -100,
  );
  $form['userdata']['userParameters']['userfields']['wrapper']['already_member'] = $element;


  $element = array(
    '#type' => 'link',
    '#title' => t('forgot_sb_pasword', array(), array('context' => 'bibdk_reservation')),
    // @TODO this link is hardcoded .. 
    '#href' => 'https://www.statsbiblioteket.dk/casext/forgotPassword',
    '#attributes' => array('target' => '_blank'),
  );
  $form['userdata']['userParameters']['userfields']['wrapper']['forgot_password'] = $element;

  $element = array(
    '#title' => t('sb_selvhenter', array(), array('context' => 'bibdk_reservation')),
    '#type' => 'checkbox',
    '#weight' => 100,
  );
  $form['userdata']['userParameters']['userfields']['self_pickup'] = $element;

  // set remaining elements
  _bibdk_reservation_sbkopi_set_elements($form, $form_state);
  return $form;
}

function _bibdk_reservation_sbkopi_unset_elements(&$form, $form_state) {
  // do not show needbefore date 
  // unset($form['manifestation']['date']);
  // do not show favourite selector
  unset($form['branchdata']['favouriteselector']);
  // do not show 'change library' button
  unset($form['branchdata']['branchParameters']);
  // do not show 'save as favourite' button
  unset($form['userdata']['userParameters']['userfields']['wrapper']['submit']);
}

function _bibdk_reservation_sbkopi_set_elements(&$form, $form_state) {
  // add form elements for NOT members (visually_hidden)
  // cpr, firstname, lastname, e-mail
  $bestil_kopi_elements = array('bestil_kopi_cpr_number', 'bestil_kopi_firstname', 'bestil_kopi_lastname', 'bestil_kopi_e_mail');
  // set toggle class
  $elements['kopi_bestil'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array(drupal_html_class('switch_me'), 'visuallyhidden'),
    ),
  );

  $elements['kopi_bestil']['not_member'] = array(
    '#type' => 'link',
    '#title' => t('signup_member', array(), array('context' => 'bibdk_reservation')),
    '#href' => '#',
    '#attributes' => array('class' => array(drupal_html_class('switch_views'))),
    '#weight' => -100,
  );

  foreach ($bestil_kopi_elements as $element_name) {
    $elements['kopi_bestil'][$element_name] = array(
      '#type' => 'textfield',
      '#title' => t($element_name, array(), array('context' => 'bibdk_reservation')),
      // '#required' => TRUE,
      '#default_value' => (isset($form_state['values'][$element_name])) ? $form_state['values'][$element_name] : "",
    );
    // make cpr password type
    if ($element_name == 'bestil_kopi_cpr_number') {
      $elements['kopi_bestil'][$element_name]['#attributes'] = array('class' => array(drupal_html_class('bibdk_password_field')));
    }
  }
  // add bestil_kopi fields to form
  $form['userdata']['userParameters']['userfields'] += $elements;
}

function bibdk_reservation_sbkopi_parse_response($xml) {
  $dom = new DomDocument();
  if (!$dom->loadXML($xml)) {
    watchdog('bibdk_reservation', t('could not load response: %xml', array('%xml' => var_export($xml, TRUE))), array(), WATCHDOG_ERROR);
    return FALSE;
  }
  $xpath = new DomXPATH($dom);
  $xpath->registerNamespace('CU', 'http://statsbiblioteket.dk/xws/createuser_v1');
  
  /** possible status **/
 /*  
  <ServiceStatus>UserPending</ServiceStatus>
  <Diagnostic><CPRStatus>MustBe18OrAbove</CPRStatus></Diagnostic>
  <Diagnostic><EMailStatus>InvalidSyntax</EMailStatus></Diagnostic>
  
  <ServiceStatus>UserNotAccepted</ServiceStatus><Diagnostic><NameStatus>InvalidName</NameStatus></Diagnostic>
  
  <ServiceStatus>UserExists</ServiceStatus>
  <ServiceStatus>UserAccepted</ServiceStatus> 
  */
  
  // ServiceStatus
  $query = '//CU:ServiceStatus';  
  $nodelist = $xpath->query($query);
  // UserPending
  // UserNotAccepted
  // UserExists
  
  
  $status = $nodelist->item(0)->nodeValue; 
  switch($status){
    case 'UserPending':      
    case 'UserNotAccepted':     
    case 'UserExists':
      $reply['status'] = 'error';
      $reply['field'] = 'form';
      $reply['message'] = $status;
      break;
    case 'UserAccepted':
      $reply['status'] = 'success';
      $reply['message'] = $status;
      break;
    default:
      $reply = array();
      break;
  }
  // Diagnostics
  // only set diagnostics if an error occured
  if ($reply['status'] == 'error') {
    $query = '//CU:Diagnostic';
    $nodelist = $xpath->query($query);
    if ($nodelist->length >= 1) {
      $reply['field'] = $nodelist->item(0)->nodeName;
      // add diagnostics to message
      $reply['message'] .= $nodelist->item(0)->firstChild->nodeValue;
    }
  }
  
  return $reply;
}

