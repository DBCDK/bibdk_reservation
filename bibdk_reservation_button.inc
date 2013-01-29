<?php

//include field info for button and infotext
module_load_include('inc', 'bibdk_reservation', 'bibdk_reservation.field');

/*
 * Returns a link for reservation flow
 */

function bibdk_reservation_link($entity_ids, $subtype, $entity_type) {
  $text = ($entity_type == "bibdkSubwork") ? t($subtype, array(), array('context'=>'bibdk_reservation'))
                                           : t("Order this @type", array("@type" => $subtype), array('context'=>'bibdk_reservation'));
  $path = 'reservation';
  $query = array('ids' => implode(',', $entity_ids));

  // This should be moved to the template
  $attributes['rel'] = 'reservation';
  $attributes['class'][] = 'bibdk-popup-link';
  $attributes['class'][] = 'btn';
  $attributes['class'][] = (isset($entity_type) && $entity_type == 'bibdkManifestation') ? 'btn-grey' : 'btn-blue';
  if (bibdk_reservation_check_past_reservations($entity_ids) && !(strtolower($subtype) == 'avis' || strtolower($subtype) == 'tidsskrift')) {
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
function bibdk_reservation_check_past_reservations($entity_ids) {
  if (isset($_SESSION['orderedPids'])) {
    foreach ($entity_ids as $pid) {
      if (in_array($pid, $_SESSION['orderedPids'])) {
        return TRUE;
        break;
      }
    }
  }
  return FALSE;
}

/*
 * Returns a link for 'reservation'Order any edition' button
 */

function bibdk_reservation_order_any_edition_link($collection, $uid) {
  $uid = strtolower(preg_replace('/[^a-zA-Z0-9]+/', '', $uid));
  $types = array();
  foreach ($collection as $type => $ids) {
    $types[] = _bibdk_reservation_create_type_button($type, $ids, $uid);
  }
  $button = theme('bibdk_reservation_order_any_btn', array('types' => $types));

  return $button;
}

function _bibdk_reservation_create_type_button($type, $ids, $uid) {
  $text = $type;
  $url_arr = drupal_parse_url('reservation/ajax/' . $uid . '/');
  $path = $url_arr['path'];
  $query = implode(',', $ids);
  $class = strtolower(preg_replace('/[^a-zA-Z0-9]+/', '', $ids[0]));

  // This should be moved to the template
  $attributes['rel'] = 'reservation';
  $attributes['class'][] = 'bibdk-popup-link';
  $attributes['class'][] = 'bibdk-popup-link-' . $class;
  $attributes['class'][] = 'use-ajax';
  if (bibdk_reservation_check_past_reservations($ids) && !(strtolower($type) == 'avis' || strtolower($type) == 'tidsskrift')) {
    $attributes['class'][] = 'orderedOnceWork';
  }
  $link = l($text, $path . $query, array('attributes' => $attributes));
  return $link;
}

function bibdk_reservation_ajax_command($reservable, $class, $path) {
  return array(
    'command' => 'check_reservability',
    'reservable' => $reservable,
    'selector' => $class,
    'path' => $path,
  );
}

/**
 * ajax callback for order any edition check
 * If material is not not borrowable a message will appear otherwise the reservation window will pop up
 */
function bibdk_reservation_ajax_check_reservability($uid, $pids) {
  $ids = explode(',', $pids);
  $id = $ids[0];
  $infotext = array();

  $bibdkWorkEntity = ting_openformat_get_manifestations($ids);
  $manifestations = $bibdkWorkEntity[$id]->getManifestations();
  $isReservable = FALSE;
  foreach ($manifestations as $manifestation) {
    $infocode = $manifestation->getInfotext();
    if ($manifestation->isReservable()) {
      $isReservable = TRUE;
      if ( $infocode == 'picture7' || $infocode == 'picture11' || $infocode == 'picture15' || $infocode == 'usergroupC' || $infocode == 'usergroupE' || $infocode == 'noLendingLibraries' || $infocode == 'saleonly' ) {
        $infotext[] = _bibdk_reservation_render_infotext( $infocode );
      }
    } else {
      $infotext[] = _bibdk_reservation_render_infotext( $infocode );
    }
  }
  $infotext = array_unique($infotext);

  $class = strtolower(preg_replace('/[^a-zA-Z0-9]+/', '', $id));
  $commands = array();
  $selector = '.msg-' . $uid;
  $type = 'notice';
  if (!$isReservable) {
    $path = '';
    $text = t('not_possible_to_borrow_material', array(), array('context'=>'bibdk_reservation'));
    if ( sizeof($infotext) > 0 ) {
      $text = implode( t(' OR ', array(), array('context'=>'bibdk_reservation')), $infotext);
    }
    $commands = _bibdk_reservation_ajax_infotext($text, $type, $selector);
  }
  else {
    global $base_root;
    global $base_path;
    $root_url = drupal_parse_url($base_root . $base_path . 'reservation/');
    $path = $root_url['path'] . '?ids=' . $pids;
    if ( sizeof($infotext) > 0 ) {
      $text = implode( t(' OR ', array(), array('context'=>'bibdk_reservation')), $infotext);
      $commands = _bibdk_reservation_ajax_infotext($text, $type, $selector);
    }
  }

  $commands[] = bibdk_reservation_ajax_command($isReservable, $class, $path);

  $page = array('#type' => 'ajax', '#commands' => $commands);
  ajax_deliver($page);
}

function _bibdk_reservation_ajax_infotext($text, $type, $selector) {
    drupal_get_messages();
    drupal_set_message($text, $type);
    $commands[] = ajax_command_invoke($selector, 'hide');
    $commands[] = ajax_command_html($selector, theme('status_messages'));
    $commands[] = ajax_command_invoke($selector, 'fadeIn', array('fast'));
    return $commands;
}