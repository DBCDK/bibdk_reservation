<?php

//include field info for button and infotext
module_load_include('inc', 'bibdk_reservation', 'bibdk_reservation.field');

/*
 * Implementation of hook_form
 */
function bibdk_reservation_button_form($form, &$form_state, $entity_ids, $type, $entity_type) {
  $form_id = 'bibdk_reservation_button_'.$entity_type;
  $form[$form_id]['entity_id']["#tree"] = true;
  
  foreach ($entity_ids as $entity_id) {
    $form[$form_id]['entity_id'][] = array(
      '#type' => 'hidden',
      "#value" => $entity_id,
    );
  }

  $form[$form_id]['type'] = array(
    '#type' => 'hidden',
    "#value" => $type,
  );
  $form[$form_id]['submit'] = array(
    '#type' => 'submit',
    '#value' => ($entity_type == "bibdkSubwork") ? t("Order any edition") : t("Order this @type", array("@type" => $type)),
    '#executes_submit_callback' => TRUE,
    '#manifestaions' => "test",
  );
  return $form;
}

/*
 * Returns a link for reservation flow
 */
function bibdk_reservation_link($entity_ids, $type, $entity_type) {
  $text = ($entity_type == "bibdkSubwork") ? t("Order any edition") : t("Order this @type", array("@type" => $type));
  $path = 'reservation';
  $query = array('ids' => implode(',', $entity_ids));
  $attributes = array('class' => array('bibdk-popup-link'), 'rel' => 'userhelp');
  if(bibdk_reservation_check_past_reservations()){
    $attributes['class'][] = 'orderedonce';
  }
  return theme('bibdk_reservation_button', array('link_text' => $text, 'path' => $path, 'query' => $query, 'link_attributes' => $attributes, 'entity_type' => $entity_type)); 
}

/*
 * Implementation of hook_form_submit
 */
function bibdk_reservation_button_form_submit($form, $form_state) {
  drupal_set_message(t("You have expressed a wish to reserve @entity_id of type @type", array (
    "@entity_id" => implode(", ", $form_state['values']['entity_id']), 
    "@type" => $form_state['values']['type'] 
  )));
  /*
   * TODO : Add entity_id to Orderobject in session
   */
}


/**
 * Checks session for already ordered materials
 */
function bibdk_reservation_check_past_reservations() {
  if (isset($_SESSION['orderedPids'])) {
    $pids = BibdkReservationOrderObject::GetObject()->getManifestationIds();
    foreach ($pids as $pid) {
      if (in_array($pid, $_SESSION['orderedPids'])) {
        return TRUE;
        break;
      }
    }
  } else {
    return FALSE;
  }
}