<div>
  <?php if (isset($IllOrderReceiptText)) : ?>
  <?php print $IllOrderReceiptText; print "\n"; ?>
  <?php endif; ?>
  
  <?php print $headertext; print "\n"; ?>
  
  <?php print $orderidtext; ?>
  <?php print $orderid; ?>
  
  <?php print "\n\n"; print $userparameter; print "\n\n"; ?>
  
  <?php foreach ($userinfo as $list) : ?>
      <?php print $list; print "\n"; ?>
  <?php endforeach; ?>
   
  <?php print "\n\n"; print $manifestation; ?>
  
  <?php print "\n"; ?>
  <?php foreach ($title as $list) : ?>
      <?php print $list; print "\n"; ?>
  <?php endforeach; ?>
  
  <?php print "\n"; print $branch; print "\n"; ?>
  <?php print "\n"; print $branchtext; ?>
  <?php print "\n"; print $addresstext; ?>
  <?php print "\n"; print $postalCodetext; print ' '; print $citytext; ?>
  
  <?php if (isset($orderinfo)) : ?>
    <?php print "\n\n"; print $orderparameters;print "\n\n"; ?>
    <?php foreach ($orderinfo as $data) : ?>
      <?php print $data; print "\n"; ?>
    <?php endforeach; ?>
  <?php endif; ?>
  
</div>
