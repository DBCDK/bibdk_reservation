<?php

function bibdk_reservation_form_step_1($form, &$form_state) {
  $form['step_info'] = array(
    '#type' => 'container', 
    '#attributes' => array('class' => array('form-wrapper', 'form-item')),
  );
  $form['step_info']['content'] = array(
    '#type' => 'markup',
    '#markup' => t('bibdk_reservation_step1_description'),
    
  );
  
  $form['agency_search_form_container'] = array(
    '#type' => 'container', 
    '#attributes' => array('class' => array('form-wrapper')),
  );
  
  $form['agency_search_form_container']['search_form'] = bibdk_reservation_agency_search_form(array(), $form_state);
  
  
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
     '#attributes' => array('class' => array('search-form-horizontal', 'header-action-reservation')),
     '#title' => t('Find Library'),
     
   );
   $form['agency_search_fieldset']['anyField'] = array(
    '#type' => 'textfield',
    //'#title' => t('bibdk_reservation_agency_search_description'),
    '#attributes' => array('placeholder' => t('Library name, zip or city'), 'class'=> array('')),
    '#default_value' => isset($form_state['values']['anyField']) ? $form_state['values']['anyField']: NULL,
  );
   $form['agency_search_fieldset']['search'] = array(
    '#type' => 'submit',
    '#prefix' => '<div class="form-wrapper form-actions">', 
    '#submit' => array('bibdk_reservation_wizard_goto_step_submit'),
    '#suffix' => '</div>',
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
      $form += bibdk_reservation_view_agency_branch($branch);
  }
  return $form;
}

function bibdk_reservation_form_step_1_submit($form, &$form_state) {
  BibdkReservationOrderObject::GetObject()->setBranch($form_state['clicked_button']['#branch']);
}

function bibdk_reservation_agency_branch_get_action_link($branch) {
  $form = array(
        '#value' => t('select_reservation_library'),
        '#name' => 'branch-'.$branch->branchId,
        '#type' => 'submit',
        '#submit' => array('bibdk_reservation_wizard_next_submit'),
        '#step' => 2,
        '#branch' => $branch,
      );
  return $form;
}

/**
 * the form to be displayed on profile tab
 * */
function bibdk_reservation_view_agency_branch($branch) {
  // check if given branch is set as order-agency
  $order_class = ( isset($branch->orderLibrary) && $branch->orderLibrary == 'TRUE') ? 'order-agency' : '';

  $form['#title'] = $branch->branchName;

  $moreinfo = theme('ting_agency_more_info', array('branch' => $branch,
    'openingHours' => theme('ting_agency_opening_hours', array('branch' => $branch,)),
    //'tools' => theme('ting_agency_tools', array('branch' => $branch)),
    'address' => theme('ting_agency_address', array('branch' => $branch)),
    'contact' => theme('ting_agency_contact', array('branch' => $branch)),
      )
  );
  $classes = array();
  $classes[] = 'favourite_' . $branch->branchId;
  $classes[] = ( isset($branch->orderLibrary) && $branch->orderLibrary == 'TRUE') ? 'order-agency' : NULL;
  $form[$branch->branchId]['action'] = bibdk_reservation_agency_branch_get_action_link($branch);
  $form[$branch->branchId]['library']['#markup'] =
      theme('ting_agency_library', array('branchName' => $branch->branchName, 'actions' => drupal_render($form[$branch->branchId]['action']), 'add_classes' => $classes, 'moreinfo' => $moreinfo, 'branchid' => $branch->branchId));

  // place branch on top if set as order-agency
  if (isset($branch->orderLibrary) && $branch->orderLibrary == 'TRUE') {
    $form[$branch->branchId]['#weight'] = -1;
  }
  return $form;
}
