<div class="actions">
  <div class="primary-actions">
    <div class="dropdown-wrapper">
      <a <?php print drupal_attributes($addi_attributes);?> class="btn btn-blue dropdown-toggle" href="#"><?php print t('Order any edition', array(), array('context'=>'bibdk_reservation')); ?> <span class="icon icon-right icon-white-down">&nbsp;</span></a>
      <div class="dropdown-menu order-any-btn-list visuallyhidden">
        <?php print $table; ?>
        <div class="reservation-btn-infotext <?php print $hide; ?>"><?php print $infotext; ?></div>
      </div>
    </div>
  </div>
</div>