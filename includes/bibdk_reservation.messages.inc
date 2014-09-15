<?php

function bibdk_reservation_get_messages($bibdk_work_entity) {
  $infotexts = _bibdk_reservation_get_infotext($bibdk_work_entity->getManifestations());
  if (!$bibdk_work_entity->isReservable() && empty($infotexts)) {
    $infotexts['status'][] = t('not_possible_to_borrow_material', array(), array('context' => 'bibdk_reservation'));
  }
  _bibdk_reservation_render_messages($infotexts);
}

function bibdk_reservation_get_actions($ids, $bibdk_work_entity) {
  $links = array();
  bibdk_reservation_get_sbkopi_link($ids);
  $manifestation = reset($bibdk_work_entity->getManifestations());
  if ($link = bibdk_reservation_article_infomedia_link($manifestation)) {
    $links[] = $link;
  }
  if ($link = bibdk_reservation_get_sbkopi_link($ids)) {
    $links[] = $link;
  }
  if(count($links)) {
    $form = array(
      '#type' => 'container',
      '#attributes' => array(
        'class' => array(
          'reservation-online-links'
        ),
      )
    );
    $form['header'] = array(
      '#theme' => 'html_tag',
      '#tag' => 'h2',
      '#value' => t('Alternative access', array(), array('context' => 'bibdk_reservation')),
    );

    $form['description'] = array(
      '#theme' => 'html_tag',
      '#tag' => 'p',
      '#value' => t('description_text', array(), array('context' => 'bibdk_reservation')),
      '#attributes' => array(
        'class' => array(
          'bibdk-reservation-links-description'
        ),
      ),
    );

    $form['links'] = $links;

    return $form;

  }
  return $links;
}


function bibdk_reservation_article_infomedia_link(Manifestation $manifestation) {
  $link = array();
  if ($relation_link = $manifestation->getInfomediaLink()) {
    $link[] = array(
      '#theme' => 'ting_infomedia_button',
      '#ting_infomedia_button' => _ting_infomedia_get_link($manifestation->ding_entity_id, $manifestation->getInfomediaLink()),
    );
  }
  return $link;
}

function bibdk_reservation_get_sbkopi_link($ids) {
  $result = bibdk_sbkopi_ajax_openorder_request($ids);
  if ($result !== 'error') {
    return $result;
  }
  return null;
}
