<?php
/**
 * Form callback voucher mail settings
 */
function bosa_voucher_mail_settings() {
  $form['bosa_voucher_mail_enable'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable BOSA order confirmation mail'),
      '#description' => t(''),
      '#default_value' => variable_get('bosa_voucher_mail_enable'),
  );

  return system_settings_form($form);
}

/**
 * Form callback voucher mail settings
 */
function bosa_voucher_voucher_settings() {
  $form['bosa_voucher_voucher_enable'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable BOSA voucher generation'),
      '#description' => t(''),
      '#default_value' => variable_get('bosa_voucher_voucher_enable'),
  );

  $form['bosa_voucher_qr_enable'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable QR code generation'),
      '#description' => t(''),
      '#default_value' => variable_get('bosa_voucher_qr_enable'),
  );
 
  $form['bosa_voucher_qr_prefix'] = array(
      '#type' => 'textfield',
      '#title' => t('QR code prefix'),
      '#description' => t('The first characters shown on QR code, ex. "TET" -> "TET-xxxx-xx"'),
      '#default_value' => variable_get('bosa_voucher_qr_prefix'),
  );

  $query = new EntityFieldQuery;

  $query->entityCondition('entity_type', 'node')
          ->entityCondition('bundle', 'bosa_voucher_pdf_template');

  $result = $query->execute();
  $result = array_shift($result);

  foreach ($result as $nid => $value) {
    $node = node_load($nid);
    $options[0] = t('None');
    $options[$nid] = $node->title;
  }

  $form['bosa_voucher_pdf_node_id'] = array(
    '#title' => t('Voucher mail node'),
    '#options' => $options,
    '#default_value' => variable_get('bosa_offer_mail_node_id'),
    '#description' => t('Select the node that contains the text you want to use. You can create a new node here: ') . ' ' . l(t('create'), 'node/add/bosa-voucher-pdf-template') .'</br>' . t('Or you can edit the one you have specified here:') . ' ' . l(t('edit'), 'node/'.variable_get('bosa_voucher_pdf_node_id').'/edit'),
    '#type' => 'select',
  );

  return system_settings_form($form);
}
