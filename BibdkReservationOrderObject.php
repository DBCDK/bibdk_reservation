<?php

class BibdkReservationOrderObject {

  private $branchId;
  private $branch;
  private $manifestationIds;
  private $manifestation;
  private $userData;
  private $userOrderData;
  private $fields;
  private $needBeforeDate;


  public function setManifestationIds($ids){
    $this->manifestationIds = $ids;
  }
  
  public function getManifestationIds(){
    return $this->manifestationIds;
  }
  
  public function setManifestation($manifestation){
    $this->manifestation = $manifestation;
  }
  
  public function getManifestation(){
    return $this->manifestation;
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
  
  public function setUserOrderData($userOrderData){
    $this->userOrderData = $userOrderData;
  }
  
  public function getUserOrderData() {
    return $this->userOrderData;
  }
   
  public function setNeedBeforeDate($needBeforeDate){
    $this->needBeforeDate = $needBeforeDate;
  }

  public function getNeedBeforDate(){
    return $this->needBeforeDate;
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