
<div class="order-policy">
  <?php $orderpolicy = $variables['orderpolicy']; ?>
  <?php if(isset($orderpolicy['orderpossible'])): ?>
  <p><?php print $orderpolicy['orderpossible']; ?></p>
  <?php endif; ?>
  
  <?php if(isset($orderpolicy['reason'])): ?>
  <p><?php print $orderpolicy['reason']; ?></p>
  <?php endif; ?>
  
  <?php if(isset($orderpolicy['lookUpUrl'])): ?>
  <p><?php print $orderpolicy['lookUpUrl']; ?></p>
  <?php endif; ?>
  
  <?php if(isset($orderpolicy['agencyCatalogueUrl'])): ?>
  <p><?php print $orderpolicy['agencyCatalogueUrl']; ?></p>
  <?php endif; ?>
  
</div>