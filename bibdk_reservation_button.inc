<?php

//include field info for button and infotext
module_load_include('inc', 'bibdk_reservation', 'bibdk_reservation.field');


/**
 * Returns a link for reservation flow.
 * If more than one pid is present in $entity_ids a button will be
 *
 * @param $entity_ids
 * @param $subtype
 * @param $entity_type
 * @return bool|string
 */
function bibdk_reservation_link($entity_ids, $subtype, $entity_type) {
  $manifestations = explode(',', implode(',', $entity_ids));

  if (count($manifestations) >= 2) {
    $text = t("order_any_edition", array(), array('context' => 'bibdk_reservation'));
    $button_id = 'any_edition_'.$manifestations[0];
  }
  else {
    $translated_sub = t($subtype, array(), array('context' => 'bibdk_reservation'));
    $text = ($entity_type == "bibdkSubwork") ? $translated_sub : t("Order this @type", array("@type" => $translated_sub), array('context' => 'bibdk_reservation'));
    $button_id = 'this_edition_'.current($manifestations);
  }
  $path = 'reservation';
  $query = array('ids' => implode(',', $entity_ids));

  // This should be moved to the template
  $attributes['id'] = $button_id;
  $attributes['data-rel'] = 'reservation';
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

/**
 * Returns a link for 'reservation'Order any edition' button
 *
 * @param $collection
 * @param $uid
 * @return string
 */
function bibdk_reservation_order_any_edition_link($collection, $uid) {
  $uid = strtolower(preg_replace('/[^a-zA-Z0-9]+/', '', $uid));
  $items = array();
  foreach ($collection as $type => $ids) {
    $items[] = _bibdk_reservation_create_type_button($type, $ids, $uid);
  }

  $lists[] = array(
    '#type' => 'ol',
    '#theme' => 'item_list',
    '#title' => t('Order ↓', array(), array('context' => 'bibdk_reservation')),
    '#items' => $items,
    '#prefix' => '<div class="col1">',
    '#suffix' => '</div>',
    '#attributes' => array(
      'class' => 'order-list',
    ),
  );

  $lists = module_invoke_all('order_any_btn_lists', $lists, $collection);

  $rendered = array();
  foreach ($lists as $list) {
    $rendered[] = drupal_render($list);
  }
  
  // set an id for the button
  $attributes = array('id'=>'any_edition_but_'.$uid);
  // also render additonal attributes
  $button = theme('bibdk_reservation_order_any_btn', array('lists' => $rendered, 'addi_attributes' => $attributes));

  return $button;
}

/**
 * Returns button with material type
 *
 * @param $type
 * @param $ids
 * @param $uid
 * @return string
 */
function _bibdk_reservation_create_type_button($type, $ids, $uid) {
  $text = t($type, array(), array('context' => 'material_type'));
  $url_arr = drupal_parse_url('reservation/ajax/' . $uid . '/');
  $path = $url_arr['path'];
  $query = implode(',', $ids);
  $class = strtolower(preg_replace('/[^a-zA-Z0-9]+/', '', $ids[0]));

  // This should be moved to the template
  $attributes['data-rel'] = 'reservation';
  $attributes['class'][] = 'bibdk-popup-order-work';
  $attributes['class'][] = 'bibdk-popup-link-' . $class;
  $attributes['class'][] = 'use-ajax';
  $attributes['id'][] = 'any_edtion_order_' .$class;
  if (bibdk_reservation_check_past_reservations($ids) && !(strtolower($type) == 'avis' || strtolower($type) == 'tidsskrift')) {
    $attributes['class'][] = 'orderedOnceWork';
  }
  $link = l($text, $path . $query, array('attributes' => $attributes));
  return $link;
}

/**
 * @param $reservable
 * @param $class
 * @param $path
 * @return array
 */
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
  global $language;
  $language_prefix = (!empty($language->prefix)) ? $language->prefix . '/' : '';
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
      if ($infocode == 'picture7' || $infocode == 'picture11' || $infocode == 'picture15' || $infocode == 'usergroupC' || $infocode == 'usergroupE' || $infocode == 'noLendingLibraries' || $infocode == 'saleonly') {
        $infotext[] = _bibdk_reservation_render_infotext($infocode);
      }
    }
    else {
      $infotext[] = _bibdk_reservation_render_infotext($infocode);
    }
  }
  $infotext = array_unique($infotext);

  $class = strtolower(preg_replace('/[^a-zA-Z0-9]+/', '', $id));
  $commands = array();
  $selector = '.msg-' . $uid;
  $type = 'notice';
  if (!$isReservable) {
    $path = '';
    $text = t('not_possible_to_borrow_material', array(), array('context' => 'bibdk_reservation'));
    if (sizeof($infotext) > 0) {
      $text = implode(t(' OR ', array(), array('context' => 'bibdk_reservation')), $infotext);
    }
    $commands = _bibdk_reservation_ajax_infotext($text, $type, $selector);
  }
  else {
    global $base_root;
    global $base_path;
    $root_url = drupal_parse_url($base_root . $base_path . $language_prefix . 'reservation/');
    $path = $root_url['path'] . '?ids=' . $pids;
    if (sizeof($infotext) > 0) {
      $text = implode(t(' OR ', array(), array('context' => 'bibdk_reservation')), $infotext);
      $commands = _bibdk_reservation_ajax_infotext($text, $type, $selector);
    }
  }

  $commands[] = bibdk_reservation_ajax_command($isReservable, $class, $path);

  $page = array('#type' => 'ajax', '#commands' => $commands);
  ajax_deliver($page);
}

/**
 * @param $text
 * @param $type
 * @param $selector
 * @return array
 */
function _bibdk_reservation_ajax_infotext($text, $type, $selector) {
  drupal_get_messages();
  drupal_set_message($text, $type);
  $commands[] = ajax_command_invoke($selector, 'hide');
  $commands[] = ajax_command_html($selector, theme('status_messages'));
  $commands[] = ajax_command_invoke($selector, 'fadeIn', array('fast'));
  return $commands;
}
