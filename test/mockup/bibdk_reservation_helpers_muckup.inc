<?php
/**
 * @file
 * Muckups of Helper functions for unittests
 */

/**
 * Helper function to get a branch name from an Agency ID
 * @param $agencyId
 * @return null
 */
function bibdk_reservation_get_branch_name_from_id($agencyId){
  return $agencyId . '_name';
}

/**
 * Helper function to translate strings in bibdk_reservation module
 * @param $string
 * @param array $args
 * @param array $options
 * @return null|string
 */
function bibdk_reservation_translate($string, $args = array(), $options = array('context' => 'bibdk_reservation')) {
  return format_string($string, $args);
}
