<?php
  $attributes['id'][] = 'any_edtion_order_' . $class;
  $attributes['data-rel'] = 'reservation';
  $attributes['class'][] = 'bibdk-popup-order-work';
  $attributes['class'][] = 'bibdk-popup-link-' . $class;
  $attributes['class'][] = 'use-ajax';
  if ( $orderedOnce ) {
    $attributes['class'][] = 'orderedOnceWork';
  }
?>

<div class="bibdk-reservation-item">
  <?php
    $link = l($text, $path . $query, array('attributes' => $attributes));
  ?>
</div>
