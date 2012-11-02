<div class="receipt-container">
  <h4><?php print t('Your order was successfully completed:'); ?></h4>
  <p><?php print t('Your order ID: ') . $orderId; ?></p>
  <h3><?php print t('Personal information'); ?></h3>
  <?php foreach ($user as $key => $value) : ?>
    <?php print $key . ': ' . $value . '<br />'; ?>
<?php endforeach; ?>
  <br />
  <h4><?php print t('You are ordering: '); ?></h4>
  <p><?php print $manifestation ?></p>
  <h3><?php print t('Library'); ?></h3>
  <p><?php print $branch->branchName; ?><br/>
    <?php if (isset($branch->postalAddress) && $branch->postalAddress) : ?>
      <?php print $branch->postalAddress; ?><br/>
      <?php print $branch->postalCode; ?> <?php print $branch->city; ?><br/>
    <?php endif; ?>
    <?php if (isset($branch->openingHours) && $branch->openingHours) : ?>
    <p><?php print t('Opening hours'); ?><br/>
      <?php foreach ($branch->openingHours as $hours) : ?>
        <?php print $hours->{'$'}; ?>
        <br/>
    <?php endforeach; ?>
    </p>
<?php endif; ?>
</div>