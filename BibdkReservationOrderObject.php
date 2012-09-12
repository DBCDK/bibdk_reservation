<?php

class BibdkReservationOrderObject {
  private $agencyId;
  
  public function setAgencyId($id){
    $this->agencyId = $id;
  }
  
  public function getAgencyId(){
    return $this->agencyId;
  }
}