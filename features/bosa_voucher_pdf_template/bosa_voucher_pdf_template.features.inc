<?php
/**
 * @file
 * bosa_voucher_pdf_template.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function bosa_voucher_pdf_template_ctools_plugin_api() {
  list($module, $api) = func_get_args();
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_node_info().
 */
function bosa_voucher_pdf_template_node_info() {
  $items = array(
    'bosa_voucher_pdf_template' => array(
      'name' => t('BOSA Voucher pdf template'),
      'base' => 'node_content',
      'description' => '',
      'has_title' => '1',
      'title_label' => t('Titel'),
      'help' => '',
    ),
  );
  return $items;
}
