    <div class="agency-container">
      <h4>Du bestiller:</h4>
        <?php print $variables['manifestation']; ?>
      <h3>Personlige oplysninger</h3>
      <?php foreach ($user as $key => $value) : ?>
        <?php print $key . ': ' .$value . '<br />'; ?>
      <?php endforeach; ?>
      <br />
      <?php if(isset($orderParameters)): ?>
      <h3><?php echo t('Order Parameters'); ?></h3>
      <?php foreach ($orderParameters as $key => $value) : ?>
        <?php print $key . ': ' .$value . '<br />'; ?>
      <?php endforeach; ?>
      <?php endif; ?>
      
      <h3>Bibliotek</h3>
       <?php $branch = $variables['branch']; ?>
        <p><?php print $branch->branchName; ?><br/>
        <?php if (isset($branch->postalAddress) && $branch->postalAddress) : ?>
          <?php print $branch->postalAddress; ?><br/>
            <?php print $branch->postalCode; ?> <?php print $branch->city; ?><br/>
        <?php endif; ?>
    </div>