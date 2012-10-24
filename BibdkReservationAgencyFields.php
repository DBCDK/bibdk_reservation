<?php

/**
 * Handle settings for Agency specific form fields
 * 
 * This function is prepared to manifestation specific userinput
 */
class BibdkReservationAgencyFields {

  public $userParameters, $userIdTxt, $orderParameters, $agencyParameters, $response;

  public function __construct($response) {
    foreach ($response as $key => $value) {
      $this->$key = $value;
    }
    $this->response = $response;

    foreach ($this->userParameters as $key => $element)
      $this->userParameters[$key] += $this->_getSettingsFromType($element['type']);
  }

  public function getUserParameters() {
    return $this->userParameters;
  }

  public function getOrderParameters() {
    return $this->orderParameters;
  }

  public function getOrderParametersForType($type, $orderType = 'ill') {
    if (!isset($this->orderParameters))
      return;
    foreach ($this->orderParameters as $orderParameter) {
      if ($orderParameter['materialType'] == $type && $orderParameter['orderType'] == $orderType) {
        return isset($orderParameter['itemParameters']) ? $orderParameter['itemParameters'] : NULL;
      }
    }
  }

  public function getUserIdKey() {
    foreach ($this->userParameters as $key => $element) {
      if (in_array($element['type'], array('cpr', 'userId', 'cardno', 'customId'))) {
        return $element['type'];
      }
    }
  }

  public function isBorrowerCheckRequired() {
    return $this->agencyParameters['borrowerCheckParameters']['bibliotek.dk'];
  }

  public function acceptOrderFromUnknownUser() {
    return $this->agencyParameters['acceptOrderFromUnknownUser'];
  }

  public function acceptOrderAgencyOffline() {
    return $this->agencyParameters['acceptOrderAgencyOffline'];
  }

  public function getOrderLabelFromType($type) {
    switch ($type) {
      case 'authorOfComponent' :
        return t('authorOfComponent');
        break;
      case 'issue' :
        return t('issue');
        break;
      case 'pagination' :
        return t('pagination');
        break;
      case 'publicationDateOfComponent' :
        return t('publicationDateOfComponent');
        break;
      case 'userReferenceSource' :
        return t('userReferenceSource');
        break;
      case 'titleOfComponent' :
        return t('titleOfComponent');
        break;
      case 'volume' :
        return t('volume');
        break;
      default:
        break;
    }
  }

  private function _getSettingsFromType($type) {
    $settings = array();
    switch ($type) {
      case 'cpr':
        $settings = array(
          'field_name' => t('CPR-number'),
          'field_type' => 'password',
          'field_description' => isset($this->userIdTxt) ? implode(", ", $this->userIdTxt) : NULL,
        );
        break;
      case 'userId':
        $settings = array(
          'field_name' => t('User ID'),
          'field_description' => isset($this->userIdTxt) ? implode(", ", $this->userIdTxt) : NULL,
        );
        break;

      case 'cardno':
        $settings = array(
          'field_name' => t('Card number'),
          'field_type' => 'password',
          'field_description' => isset($this->userIdTxt) ? implode(", ", $this->userIdTxt) : NULL,
        );
        break;
      case 'customId':
        $settings = array(
          'field_name' => t('Custom ID'),
          'field_type' => 'password',
          'field_description' => isset($this->userIdTxt) ? implode(", ", $this->userIdTxt) : NULL,
        );
        break;
      case 'barcode':
        $settings = array(
          'field_name' => t('Barcode'),
          'field_type' => 'password',
          'field_description' => isset($this->userIdTxt) ? implode(", ", $this->userIdTxt) : NULL,
        );
        break;
      case 'pincode':
        $settings = array(
          'field_name' => t('Pincode'),
          'field_type' => 'password',
        );
        break;
      case 'customId':
        $settings = array(
          'field_name' => t('Custom ID'),
          'field_type' => 'password',
          'field_description' => isset($this->userIdTxt) ? implode(", ", $this->userIdTxt) : NULL,
        );
        break;
      case 'userDateOfBirth':
        $settings = array(
          'field_name' => t('Date of birth'),
        );
        break;
      case 'userName':
        $settings = array(
          'field_name' => t('Name'),
        );
        break;
      case 'userAddress':
        $settings = array(
          'field_name' => t('Adress, postal code, town/city'),
        );
        break;
      case 'userMail':
        $settings = array(
          'field_name' => t('E-mail'),
        );
        break;
      case 'userTelephone':
        $settings = array(
          'field_name' => t('Phone number'),
        );
        break;

      default:
        break;
    }

    return $settings;
  }

}