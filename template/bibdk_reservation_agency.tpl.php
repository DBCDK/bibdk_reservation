
<div class="bottom-space branch-data">
  <b><?php print $name; ?></b><br/>
  <?php if (isset($shortname)) : ?>
    <?php print $shortname; ?><br/>
  <?php endif; ?>
  <?php if (isset($address)) : ?>
    <?php print $address; ?><br/>
    <?php print $postalCode; ?> <?php print $city; ?><br/>
  <?php endif; ?>
</div>
