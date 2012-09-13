<?php

class BibdkReservationOrderObject {
  private $agencyId;
  private $manifestationIds;
  private $userData;
  
  public function setManifestationIds(array $ids){
    $this->manifestationIds = $ids;
  }
  
  public function getManifestationIds(array $ids){
    return $this->manifestationIds;
  }
  
  public function setAgencyId($id){
    $this->agencyId = "DK-".$id;
  }
  
  public function getAgencyId(){
    return $this->agencyId;
  }
  
  public function setUserData($userData){
    $this->userData = $userData;
  }

  public function getUserData(){
    return $this->userData;
  }
   
  /**
   * @return BibdkReservationOrderObject
   */
  public static function GetObject(){
    return $_SESSION['orderobject'];
  }
  
  /**
   * @return BibdkReservationOrderObject
   */
  public static function SetObject(){
    $_SESSION['orderobject'] = new BibdkReservationOrderObject();
    return $_SESSION['orderobject'];
  }
}