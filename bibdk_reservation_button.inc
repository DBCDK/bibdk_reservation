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
    $button_id = 'any_edition_' . $manifestations[0];
  }
  else {
    $translated_sub = t($subtype, array(), array('context' => 'bibdk_reservation'));
    $text = ($entity_type == "bibdkSubwork") ? $translated_sub : t("Order this @type", array("@type" => $translated_sub), array('context' => 'bibdk_reservation'));
    $button_id = 'this_edition_' . current($manifestations);
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
    $items['items'][] = _bibdk_reservation_create_type_button($type, $ids, $uid);
  }

  $items['title'] = t('reservation_order_any', array(), array('context' => 'bibdk_reservation'));
  $items['type'] = 'ol';
  $items['attributes'] = array(
    'class' => 'order-list',
  );

  $lists[] = array(
    '#theme' => 'bibdk_reservation_order_any_btn_list',
    '#column' => '1',
    '#items' => $items,
  );

  $lists = module_invoke_all('order_any_btn_lists', $lists, $collection);

  $rendered = array();
  foreach ($lists as $list) {
    $rendered[] = drupal_render($list);
  }

  // set an id for the button
  $attributes = array('id' => 'any_edition_but_' . $uid);
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
  $attributes['id'][] = 'any_edtion_order_' . $class;
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
 * AJAX callback for order any edition check
 * If material is not not reservable a message will appear otherwise the
 * reservation window will pop up.
 *
 * @param String $uid
 * @param array $pids
 */
function bibdk_reservation_ajax_check_reservability($uid, $pids) {
  global $language;
  $language_prefix = (!empty($language->prefix)) ? $language->prefix . '/' : '';
  $ids = explode(',', $pids);
  $id = $ids[0];

  $bibdkWorkEntity = ting_openformat_get_manifestations($ids);
  $manifestations = $bibdkWorkEntity[$id]->getManifestations();

  $isReservable = _ting_openformat_material_is_reservable($manifestations);

  $infotext = _ting_openformat_get_infotext($manifestations);
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
    global $base_root, $base_path;
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
 * Extracts the infotext and renders the appropriate text.
 * If the material is available online, the link will be extracted and prepared
 * for display
 *
 * @param array $manifestations
 * @internal ManifestationMaster $manifestation
 * @return array
 */
function _ting_openformat_get_infotext($manifestations) {
  //TODO mmj needs testing
  $infotext = array();
  foreach ($manifestations as $manifestation) {
    $infocode = $manifestation->getInfotext();
    fb('Infocode: ' . $infocode); //TODO mmj handle links --> watch storry in ScrumDo - 'link' // 'plusLink'
    if ($manifestation->isReservable()) {
      if ($infocode == 'picture7' || $infocode == 'picture11' || $infocode == 'picture15' || $infocode == 'usergroupC' || $infocode == 'usergroupE' || $infocode == 'noLendingLibraries' || $infocode == 'saleonly') {
        $infotext[] = _bibdk_reservation_render_infotext($infocode);
      }
    }// TODO mmj this seems to be subject for a clean up, as the use of _render_infotext() seems ambiguous.
    else if ($infocode == 'link' || $infocode == 'plusLink' || $infocode == 'linkE17') {
      $infotext[] = _ting_openformat_render_direct_link($manifestation);
    }
    else {
      $infotext[] = _bibdk_reservation_render_infotext($infocode);
    }
  }
  return $infotext;
}

/**
 * Checks if any of the manifestations is reservable.
 *
 * @param array $manifestations
 * @return bool
 */
function _ting_openformat_material_is_reservable($manifestations) {
  //TODO mmj needs unittesting
  foreach ($manifestations as $manifestation) {
    if ($manifestation->isReservable()) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
 * @param ManifestationMaster $manifestation
 * @return string
 */
function _ting_openformat_render_direct_link($manifestation) {
  $links = _bibdk_reservation_get_access_links($manifestation);

  $message = t("drupal_text_link !links", array('!links' => $links), array('context' => 'bibdk_reservation'), array('attributes' => array('target' => '_blank')));
  return $message;
}

/**
 * Retrieves any accessUrl's from the given manifestation a string of links
 * seprated by <br />
 *
 * @param ManifestationMaster $manifestation
 * @return string
 */
function _bibdk_reservation_get_access_links($manifestation){
  //TODO mmj needs testing
  $info = $manifestation->getAccessInformation();
  $links = "<br/>";
  $last = end($info['accessUrl']);

  foreach ($info['accessUrl'] as $link) {
    $links .= l($link, $link, array('attributes'=>array('target'=>'_blank')));
    if($link != $last){
      $links .= "<br/>";
    }
  }
  return $links;
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
  $commands[] = ajax_command_invoke($selector, 'fadeIn', array(100));
  return $commands;
}
