<?php

/**
 * Class Manifestation_mockup
 */
class Manifestation_mockup {

  public $bibdk_order;
  public function getTitleSpecific() {
    return $this->_getTitle();
  }

  public function getTitle() {
    return $this->_getTitle();
  }

  /**
   * @return bool|string
   */
  private function _getTitle() {
    switch ($this->bibdk_order) {
      case 'titleSpecfic':
        return 'TITLESPECIFIC_RETURN';
        break;
      case 'title':
        return 'TITLE_RETURN';
        break;
      case 'no_title':
        return '';
        break;
      default:
        return FALSE;
        break;
    }
  }
}
