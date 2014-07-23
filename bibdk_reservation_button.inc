<?php
/**
 * @file
 * Functions for reservation buttons
 */

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
  $button = theme('bibdk_reservation_order_any_btn', array(
    'lists' => $rendered,
    'addi_attributes' => $attributes
  ));

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
  
  if (!_bibdk_reservation_is_mattype_order_ok($text)) {
    // make not for order text
    $note = array(
      '#prefix' => '<span class="not-for-order">',
      '#markup' => $text . t(', not for order', array(), array('context' => 'bibdk_reservation')),
      '#suffix' => '</span>',
    ); 
    return drupal_render($note);
  }
  
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

  $link = theme('bibdk_reservation_order_any_btn_list_item', array(
    'text' => $text,
    'path' => $path,
    'query' => $query,
    'class' => $class,
    'orderedOnce' => FALSE,
  ));

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

  $bibdk_work_entity = ting_openformat_get_manifestations($ids);
  $manifestations = $bibdk_work_entity[$id]->getManifestations();

  $is_reservable = _ting_openformat_material_is_reservable($manifestations);

  $infotext = _bibdk_reservation_get_infotext($manifestations);

  $class = strtolower(preg_replace('/[^a-zA-Z0-9]+/', '', $id));
  $selector = '.msg-' . $uid;
  $type = 'status';

  if (!$is_reservable) {
    $path = '';
    if (sizeof($infotext) > 0) {
      drupal_get_messages(); // flush queue
      $commands = _bibdk_reservation_render_commands($infotext, $selector);
    }
    else {
      $text = t('not_possible_to_borrow_material', array(), array('context' => 'bibdk_reservation'));
      $commands = _bibdk_reservation_ajax_infotext($text, $type, $selector);
    }
  }
  else {
    global $base_root, $base_path;
    $root_url = drupal_parse_url($base_root . $base_path . $language_prefix . 'reservation/');
    $path = $root_url['path'] . '?ids=' . $pids;
    drupal_get_messages(); // flush queue
    $commands = _bibdk_reservation_render_commands($infotext, $selector);
  }

  $commands[] = bibdk_reservation_ajax_command($is_reservable, $class, $path);
  $page = array('#type' => 'ajax', '#commands' => $commands);
  ajax_deliver($page);
}

/**
 * Parses the $infotext array and combines strings ordered by message type.
 * Returns an array of ajax commands for inserting the messages.
 *
 * @param array $infotext
 * @param string $selector
 * @return array
 */
function _bibdk_reservation_render_commands($infotext, $selector) {
  $commands = array();
  foreach ($infotext as $type => $text) {
    if (sizeof($text) > 0) {
      $text = implode("<br />", $text);
    }
    drupal_set_message($text, $type);
  }

  if (!empty($_SESSION['messages'])) {
    $commands[] = ajax_command_invoke($selector, 'hide');
    $commands[] = ajax_command_html($selector, theme('status_messages'));
    $commands[] = ajax_command_invoke($selector, 'fadeIn', array(100));
  }

  return $commands;
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
function _bibdk_reservation_get_infotext($manifestations) {
  $infotext = array();
  $messages = array();

  /* @var $manifestation Manifestation */
  foreach ($manifestations as $manifestation) {
    $infocode = $manifestation->getInfotext();
    $messageCategory = $manifestation->getMessageCategory();
    $messages[$messageCategory][] = $infocode;
  }

  $specialnotes = array(
    'link' => 'status',
    'plusLink' => 'status',
    'linkE17' => 'warning',
    'restrictedAccess' => 'warning',
    'noAccess' => 'error',
  );

  ksort($messages);
  $type = _bibdk_reservation_get_message_type(key($messages));
  $toDisplay = reset($messages);
  $toDisplay = array_unique($toDisplay);

  foreach ($toDisplay as $infocode) {
    //Handling online materials
    if (array_key_exists($infocode, $specialnotes)) {
      $type = $specialnotes[$infocode];
      if ($infocode == 'restrictedAccess' && in_array('literature', $manifestation->getType()) && in_array('online', $manifestation->getType()) && $manifestation->isLink()) {
        //this is a non-borrowable ebook
        $infotext['warning'][] = bibdk_reservation_translate('this_is_not_a_free_ebook');
      } else {
        $infotext[$type][] = _ting_openformat_render_direct_link($infocode, $manifestation);
      }
    } else {
      $infotext[$type][] = _bibdk_reservation_render_infotext($infocode);
    }
  }

  return $infotext;
}

/**
 * Returns the appropriate type of message based on the given index param
 * @param int $index
 * @return string
 */
function _bibdk_reservation_get_message_type($index) {
  switch ($index) {
    case 1:
      return 'status';
      break;
    case 2:
      return 'warning';
      break;
    case 3:
    case 4:
      return 'error';
      break;
    default:
      return 'status';
      break;
  }
}

/**
 * Checks if any of the manifestations is reservable.
 *
 * @param array $manifestations
 * @return bool
 */
function _ting_openformat_material_is_reservable($manifestations) {
  /* @var $manifestation Manifestation */
  foreach ($manifestations as $manifestation) {
    if ($manifestation->isReservable()) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
 * @param string $infocode
 * @param Manifestation $manifestation
 * @return string
 */
function _ting_openformat_render_direct_link($infocode, $manifestation) {
  $links = _bibdk_reservation_get_access_links($manifestation);
  $message = bibdk_reservation_translate("$infocode !links", array('!links' => $links));

  return $message;
}

/**
 * Retrieves any accessUrl's from the given manifestation a string of links
 * seprated by <br />
 *
 * @param Manifestation $manifestation
 * @return string
 */
function _bibdk_reservation_get_access_links($manifestation) {
  $info = $manifestation->getAccessInformation();

  return _ting_openformat_parse_element($info, '<br/>');
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

/**
 * Ckecks settings from admin Configuration -> Openformat -> Materiale types
 * @return boolean, TRUE ok for order - else FALSE 
 */
function _bibdk_reservation_is_mattype_order_ok($text) {
  $mattypes = variable_get('bibdk_mattype_settings', array());
  foreach ($mattypes as $key => $mattype) {
    if ($mattype['name'] == $text) { return FALSE; }
  }
  return TRUE;
}
