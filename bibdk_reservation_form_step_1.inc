<?php

function bibdk_reservation_form_step_1($form, &$form_state) {
  $form['step_info'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('form-wrapper', 'form-item')),
  );
  $form['step_info']['content'] = array(
    '#type' => 'markup',
    '#markup' => t('bibdk_reservation_step1_description', array(), array('context' => 'bibdk_reservation')),
  );

  $form['agency_search_form_container'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('form-wrapper')),
  );

  $form['agency_search_form_container']['search_form'] = bibdk_reservation_agency_search_form(array(), $form_state);

  if (isset($form_state['values']['anyField'])) {
    $form['agency_search_result'] = _bibdk_reservation_form_step_1_agency_result(array(), $form_state);
  }
  elseif ($favourites = BibdkReservationOrderObject::GetObject()->getFavourites()) {
    foreach ($favourites as $favourite) {
      $branches = $favourite->getBranch();
    }
    $form['agency_search_result'] = bibdk_reservation_parse_agency_search_result($branches);
  }

  return $form;
}

function bibdk_reservation_form_step_1_validate($form, &$form_state) {
  // if clicked button is a branch selector; don't validate on search field - it comes from a search result
  if (strpos($form_state['clicked_button']['#name'], 'branch-') === 0) {
    return;
  }

  $query = check_plain($form_state['values']['anyField']);
  if (empty($query)) {
    form_set_error('anyField', t("You haven't entered any search criteria.", array(), array('context' => 'bibdk_reservation:error')));
  }
}

function bibdk_reservation_agency_search_form($form, &$form_state) {
  $form['agency_search_fieldset'] = array(
    '#type' => 'fieldset',
    '#attributes' => array(
      'class' => array(
        'search-form-horizontal',
        'header-action-reservation'
      )
    ),
    '#title' => t('Find Library', array(), array('context' => 'bibdk_reservation')),
  );
  $form['agency_search_fieldset']['anyField'] = array(
    '#type' => 'textfield',
    //'#title' => t('bibdk_reservation_agency_search_description'),
    '#attributes' => array(
      'placeholder' => t('Library name, zip or city', array(), array('context' => 'bibdk_reservation')),
      'class' => array('')
    ),
    '#default_value' => isset($form_state['values']['anyField']) ? $form_state['values']['anyField'] : NULL,
  );
  $form['agency_search_fieldset']['search'] = array(
    '#type' => 'submit',
    '#prefix' => '<div class="form-wrapper form-actions">',
    '#submit' => array('bibdk_reservation_wizard_goto_step_submit'),
    '#suffix' => '</div>',
    '#value' => t('Search', array(), array('context' => 'bibdk_reservation')),
    '#step' => 1
  );

  return $form;
}

function _bibdk_reservation_form_step_1_agency_result($form, &$form_state) {
  $query = check_plain($form_state['values']['anyField']);
  if (!isset($query)) {
    return;
  }

  $result = bibdk_vejviser_execute_agency_search($query);
  $form += bibdk_reservation_parse_agency_search_result($result);

  return $form;
}

/**
 * Parse agency search result into form elements
 *
 * @param $result array of TingClientAgencyBranch objects
 * @return array formarray
 */
function bibdk_reservation_parse_agency_search_result($result) {
  $form = array();

  if (!isset($result)) {
    return array();
  }

  // @TODO .. this is not an array ask soeren why
  if (is_array($result)) {
    foreach ($result as $branch) {
      $form += bibdk_reservation_view_agency_branch($branch);
    }
  }
  else {
    $form += bibdk_reservation_view_agency_branch($result);
  }

  return $form;
}

function bibdk_reservation_form_step_1_submit($form, &$form_state) {
  BibdkReservationOrderObject::GetObject()->setBranch($form_state['clicked_button']['#branch']);

  return $form_state;
}

function bibdk_reservation_agency_branch_get_action_link($branch) {
  $form = array(
    '#value' => t('select_reservation_library', array(), array('context' => 'bibdk_reservation')),
    '#name' => 'branch-' . $branch->branchId,
    '#type' => 'submit',
    '#submit' => array('bibdk_reservation_wizard_next_submit'),
    '#step' => 2,
    '#branch' => $branch,
  );

  return $form;
}

/**
 * Render the view of an agency branch and wrap it in an form element
 *
 * @param TingClientAgencyBranch $branch
 * @return mixed
 */
function bibdk_reservation_view_agency_branch(TingClientAgencyBranch $branch) {
  global $language;

  $form['#title'] = $branch->getBranchName($language->language);

  $moreinfo = theme('ting_agency_more_info', array(
      'branch' => $branch,
      'openingHours' => theme('ting_agency_opening_hours', array('branch' => $branch,)),
      'tools' => theme('ting_agency_tools', array('branch' => $branch)),
      'address' => theme('ting_agency_address', array('branch' => $branch)),
      'pickupallowed' => theme('ting_agency_pickup_allowed', array('branch' => $branch)),
      'librarydkcontact' => theme('ting_agency_librarydk_contact', array('branch' => $branch)),
      'contact' => theme('ting_agency_contact', array('branch' => $branch)),
    ));

  $classes = array();
  $classes[] = 'favourite_' . $branch->branchId;
  $classes[] = (isset($branch->orderLibrary) && $branch->orderLibrary == 'TRUE') ? 'order-agency' : NULL;

  // Handle TemporarilyClosedReason
  if ($branch->getTemporarilyClosedReason($language->language) != 'ting_agency_no_temporarilyClosedReason') {
    $messages = drupal_get_messages();
    drupal_set_message($branch->getTemporarilyClosedReason($language->language), 'warning');
    $form[$branch->branchId]['librarymessage']['#markup'] = theme('status_messages');
    foreach ($messages as $key => $message) {
      drupal_set_message($message[0], $key);
    }
  }

  if ($branch->pickupAllowed) {
    //set button for reservation library
    $form[$branch->branchId]['action'] = bibdk_reservation_agency_branch_get_action_link($branch);
  }
  $form[$branch->branchId]['library']['#markup'] = theme('ting_agency_library', array(
    'branchName' => $branch->getBranchName($language->language),
    'actions' => drupal_render($form[$branch->branchId]['action']),
    'add_classes' => $classes,
    'moreinfo' => $moreinfo,
    'branchid' => $branch->branchId
  ));

  // place branch on top if set as order-agency
  if (isset($branch->orderLibrary) && $branch->orderLibrary == 'TRUE') {
    $form[$branch->branchId]['#weight'] = -1;
  }

  return $form;
}
