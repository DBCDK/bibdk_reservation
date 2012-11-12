<?php

function bibdk_reservation_form_step_1($form, &$form_state) {

  $form['step_description'] = array(
    '#type' => 'markup',
    '#prefix' => '<div class="form-wrapper"><div class="form-item">',
    '#suffix' => '</div></div>',
    '#markup' => t('bibdk_reservation_step1_description'),
  );
  $form['agency_search'] = bibdk_reservation_agency_search_form(array(), $form_state);
  if (isset($form_state['values']['anyField']))
    $form['agency_search_result'] = _bibdk_reservation_form_step_1_agency_result(array(), $form_state);

  return $form;
}

function bibdk_reservation_form_step_1_validate($form, &$form_state) {
  $query = check_plain($form_state['values']['anyField']);
  if (empty($query)) {
    form_set_error('anyField', t('You haven\'t entered any search criteria.'));
  }
}

function bibdk_reservation_agency_search_form($form, &$form_state){

   $form['agency_search_fieldset'] = array(
     '#type' => 'fieldset',
     '#title' => t('Find Library'),
   );
   $form['agency_search_fieldset']['anyField'] = array(
    '#type' => 'textfield',
    '#title' => t('bibdk_reservation_agency_search_description'),
    '#attributes' => array('placeholder' => t('Library name, zip or city')),
    '#default_value' => isset($form_state['values']['anyField']) ? $form_state['values']['anyField']: NULL,
  );
   $form['agency_search_fieldset']['search'] = array(
    '#type' => 'submit',
    '#submit' => array('bibdk_reservation_wizard_goto_step_submit'),
    '#value' => t('Search'),
    '#step' => 1
  );

   return $form;
}

function _bibdk_reservation_form_step_1_agency_result($form, &$form_state){
  $query = check_plain($form_state['values']['anyField']);
  if (!isset($query))
    return;

  $result = bibdk_vejviser_execute_agency_search($query);
  if (!isset($result))
    return;

  foreach ($result as $branch) {
      $form[$branch->branchId] = array(
      '#type' => 'container',
      '#attributes' => array(
        'class' => array('form-section'),
      ),
      );
      $form[$branch->branchId]['agency_result'] = array(
        '#markup' => theme('bibdk_reservation_agency', array('agency' => $branch)),
      );
      $form[$branch->branchId]['button'] = array(
        '#value' => t('Select'),
        '#name' => $branch->branchId,
        '#type' => 'submit',
        '#submit' => array('bibdk_reservation_wizard_next_submit'),
        '#branch' => $branch,
      );
  }
  return $form;
}

function bibdk_reservation_form_step_1_submit($form, $form_state) {
  BibdkReservationOrderObject::GetObject()->setBranch($form_state['clicked_button']['#branch']);
}
