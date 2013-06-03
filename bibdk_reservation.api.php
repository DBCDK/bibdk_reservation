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
function hook_bibdk_reservation_complete ($pids, $result) {
  // example from bibdk_openuserstatus.module :
  // refresh $_SESSION variables when an order is completed  
  $library = BibdkReservationOrderObject::GetObject()->getBranchId();
  if (strpos($library, 'DK-') === 0) {
    // strip 'DK-' from librarynumber
    $library = substr($library, 3, strlen($library));
  }
  if( isset($_SESSION['userStatus'][$library])){
    unset($_SESSION['userStatus'][$library]);
  }
}


?>
