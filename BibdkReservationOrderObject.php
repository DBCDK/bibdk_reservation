<?php

class BibdkReservationOrderObject {
  private $branchId;
  private $branch;
  private $manifestationIds;
  private $userData;
  private $fields;


  public function setManifestationIds(array $ids){
    $this->manifestationIds = $ids;
  }
  
  public function getManifestationIds(array $ids){
    return $this->manifestationIds;
  }
  
  public function setBranchId($id){
    $this->branchId = "DK-".$id;
  }
  
  public function getBranchId(){
    return $this->branchId;
  }
  
  public function setBranch($branch) {
    $this->branch = $branch;
    $this->setBranchId($branch->branchId);
  }
  
  public function getBranch() {
    return $this->branch;
  }
  
  public function setUserData($userData){
    $this->userData = $userData;
  }

  public function getUserData(){
    return $this->userData;
  }
   
  public function setFields($fields){
    $this->fields = $fields;
  }
  
  /**
   * @return bibdkReservationAgencyFields
   */
  public function getFields(){
    return $this->fields;
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