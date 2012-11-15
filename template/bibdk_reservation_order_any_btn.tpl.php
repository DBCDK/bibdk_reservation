<div class="actions">
  <div class="primary-actions">
    <div class="dropdown-wrapper">
      <a class="btn btn-blue dropdown-toggle" href="#"><?php print t('Order any edition'); ?> <span class="icon icon-right icon-white-down">&nbsp;</span></a>
      <ul class="dropdown-menu visuallyhidden">
        <?php foreach ($types as $type => $action) : ?>
          <li><?php print $action; ?></li>
      <?php endforeach; ?>
      </ul>
    </div>
  </div>
</div>