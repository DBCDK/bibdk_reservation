<?php

class BibdkReservationOrderObject {
  public $agencyId;
  public $manifestationIds;
  
  public function setManifestationIds(array $ids){
    $this->manifestationIds = $ids;
  }
  
  public function setAgencyId($id){
    $this->agencyId = $id;
  }
  
  public function getAgencyId(){
    return $this->agencyId;
  }
  
  public function save(){
    $_SESSION['orderobject'] = $this;
  }
}