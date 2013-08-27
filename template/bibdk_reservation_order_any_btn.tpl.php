<div class="actions">
  <div class="primary-actions">
    <div class="dropdown-wrapper">
      <a <?php print drupal_attributes($addi_attributes);?> class="btn btn-blue dropdown-toggle" href="#"><?php print t('Order any edition', array(), array('context'=>'bibdk_reservation')); ?> <span class="icon icon-right icon-white-down">&nbsp;</span></a>
      <div class="dropdown-menu order-any-btn-list visuallyhidden">
        <?php foreach ($lists as $list) : ?>
          <?php print $list; ?>
      <?php endforeach; ?>
      </div>
    </div>
  </div>
</div>