<!-- TODO mmj possibly unused template after 1394 - remove!-->
<?php $items['title'] = '<span class="title">' . $items['title'] . '</span><span class="icon icon-right icon-blue-down"> </span>' ?>

<!-- TODO mmj remember to remove deprecated classes -->
<div class="col<?php print $column; ?>">
  <?php print theme_item_list($items); ?>
</div>
