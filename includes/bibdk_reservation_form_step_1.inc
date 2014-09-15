<?php

/**
 * @param array $form
 * @param array $form_state
 * @return mixed
 */
function bibdk_reservation_form_step_1($form, &$form_state) {

  // If user is not logged in - set message
  if (!user_is_logged_in()) {
    _bibdk_reservation_set_login_message();
  }

  // Get regular reservation step
  $form['step_info'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('form-wrapper', 'form-item')),
  );
  $form['step_info']['header'] = array(
    '#theme' => 'html_tag',
    '#tag' => 'h2',
    '#value' => t('Order a local library', array(), array('context' => 'bibdk_reservation')),
  );

  $form['step_info']['content'] = array(
    '#type' => 'markup',
    '#markup' => t('bibdk_reservation_step1_description', array(), array('context' => 'bibdk_reservation')),
  );

  $form['step_info']['agency_search_form_container'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('form-wrapper')),
  );

  $form['step_info']['agency_search_form_container']['search_form'] = bibdk_reservation_agency_search_form(array(), $form_state);

  if (isset($form_state['values']['anyField'])) {
    $form['step_info']['agency_search_result'] = _bibdk_reservation_form_step_1_agency_result(array(), $form_state);
  }

  return $form;
}

/**
 * @param array $form
 * @param array $form_state
 */
function bibdk_reservation_form_step_1_validate($form, &$form_state) {
  /**  if clicked button is a branch selector; don't validate on search field - it comes from a search result */
  if (strpos($form_state['clicked_button']['#name'], 'branch-') === 0) {
    return;
  }

  $query = check_plain($form_state['values']['anyField']);
  if (empty($query)) {
    form_set_error('anyField', t("You haven't entered any search criteria.", array(), array('context' => 'bibdk_reservation:error')));
  }
}

/**
 * @param array $form
 * @param array $form_state
 * @return mixed
 */
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

/**
 * @param array $form
 * @param array $form_state
 * @return array
 */
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
 * @param array|bool|TingClientAgencyBranch $result array of TingClientAgencyBranch objects
 * @return array formarray
 */
function bibdk_reservation_parse_agency_search_result($result) {

  if (!isset($result)) {
    return array();
  }
  if (!is_array($result)){
    $result = array($result);
  }

  $form = array();
  foreach ($result as $branch) {
    $settings = array(
      '#actions' =>  bibdk_reservation_agency_branch_get_action_link($branch),
    );
    $form +=ting_agency_view_agency_branch($branch, $settings);
  }


  return $form;
}

/**
 * @param $form
 * @param $form_state
 * @return mixed
 */
function bibdk_reservation_form_step_1_submit($form, &$form_state) {
  BibdkReservationOrderObject::GetObject()->setBranch($form_state['clicked_button']['#branch']);
  return $form_state;
}

/**
 * @param TingClientAgencyBranch $branch
 * @return array
 */
function bibdk_reservation_agency_branch_get_action_link($branch) {
  $form = array();

  if($branch->pickupAllowed){
    $form = array(
      '#value' => t('select_reservation_library', array(), array('context' => 'bibdk_reservation')),
      '#name' => 'branch-' . $branch->branchId,
      '#type' => 'submit',
      '#submit' => array('bibdk_reservation_wizard_next_submit'),
      '#step' => 2,
      '#branch' => $branch,
    );
  }
  return $form;
}
