<div class="receipt-container">
  <h4>Ordre:</h4>
  <p><?php t('Your ordernumber: ');
print $variables['orderId']; ?></p>
  <h3>Personlige oplysninger</h3>
  <?php $user = $variables['user']; ?>
  <?php foreach ($user as $key => $value) : ?>
    <?php print $key . ': ' . $value . '<br />'; ?>
<?php endforeach; ?>
  <br />
  <h4>Du bestiller:</h4>
  <p><?php print $variables['manifestation']; ?></p>
  <p><?php print $variables['needBeforeDate']; ?></p>
  <h3>Bibliotek</h3>
    <?php $branch = $variables['branch']; ?>
  <p><?php print $branch->branchName; ?><br/>
    <?php if (isset($branch->postalAddress) && $branch->postalAddress) : ?>
      <?php print $branch->postalAddress; ?><br/>
      <?php print $branch->postalCode; ?> <?php print $branch->city; ?><br/>
    <?php endif; ?>
    <?php if (isset($branch->openingHours) && $branch->openingHours) : ?>
    <p>Åbningstider:<br/>
      <?php foreach ($branch->openingHours as $hours) : ?>
        <?php print $hours->{'$'}; ?>
        <br/>
    <?php endforeach; ?>
    </p>
<?php endif; ?>
</div>