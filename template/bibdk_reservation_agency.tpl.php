<div class="bottom-space">
  <b><?php print $name; ?></b><br/>
  <?php if (isset($address)) : ?>
    <?php print $address; ?><br/>
    <?php print $postalCode; ?> <?php print $city; ?><br/>
  <?php endif; ?>
</div>