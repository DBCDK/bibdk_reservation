<?php

//include field info for button and infotext
module_load_include('inc', 'bibdk_reservation', 'bibdk_reservation.field');

/*
 * Returns a link for reservation flow
 */
function bibdk_reservation_link($entity_ids, $subtype, $entity_type) {
  $text = ($entity_type == "bibdkSubwork") ? t($subtype) : t("Order this @type", array("@type" => $subtype));
  $path = 'reservation';
  $query = array('ids' => implode(',', $entity_ids));

  // This should be moved to the template
  $attributes['rel'] = 'userhelp';
  $attributes['class'][] = 'bibdk-popup-link';
  $attributes['class'][] = 'btn';
  $attributes['class'][] = (isset($entity_type) && $entity_type == 'bibdkManifestation') ? 'btn-grey' : 'btn-blue';
  if(bibdk_reservation_check_past_reservations() && strtolower($subtype) != 'avis' && strtolower($subtype) != 'tidsskrift'){
    $attributes['class'][] = 'orderedonce';
  }

  $link = array(
    '#theme' => 'link',
    '#text' => $text,
    '#path' => $path,
    '#options' => array(
      'query' => $query,
      'attributes' => $attributes,
      'html' => FALSE,
    ),
  );
  return drupal_render($link);
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