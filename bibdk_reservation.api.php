<?php

/**
 * @file
 * Hooks provided by the bibdk_reservation module.
 */

/**
 * actions to perform when a reservation has been done successfully
 * @param type $pids
 * @param type $result
 */
function hook_bibdk_reservation_complete($pids, $result) {
  // example from bibdk_openuserstatus.module :
  // refresh $_SESSION variables when an order is completed  
  $library = BibdkReservationOrderObject::GetObject()->getBranchId();
  if (strpos($library, 'DK-') === 0) {
    // strip 'DK-' from librarynumber
    $library = substr($library, 3, strlen($library));
  }
  if (isset($_SESSION['userStatus'][$library])) {
    unset($_SESSION['userStatus'][$library]);
  }
}

/**
 * Definition of hook_order_any_btn_lists().
 * Makes it possible for other modules to add/manipulate the contents of the
 * 'Order any edition' button shown on each collection.
 *
 * In the basic example below another list is added to the button but any
 * content given in a render array could be added, as it will be rendered by the
 * drupal_render() method.
 *
 * @param array $lists Array with the content of the 'Order any edition'
 *                            button so far collected.
 *
 * @param array $collection Array with the collections that will be displayed.
 *                            Each collection will get its own 'Order any
 *                            edition' button.
 *
 * @return array $lists       Array with the lists including the one your
 *                            implementation of the hook is providing.
 *
 * @see includes/common.inc - drupal_render();
 */
function hook_order_any_btn_lists($lists, $collection) {
  $item1 = l('my-link-1', 'my/path/1');
  $item2 = l('my-link-2', 'my/path/2');

  $lists[] = array(
    '#type' => 'ol',
    '#theme' => 'item_list',
    '#title' => 'My List',
    '#items' => array($item1, $item2),
    '#prefix' => '<div>',
    '#suffix' => '</div>',
    '#attributes' => array(
      'class' => array(
        'my-class',
      ),
    ),
  );

  return $lists;
}
