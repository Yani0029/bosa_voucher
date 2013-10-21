<?php
/**
 * @file
 * bosa_voucher_views.views_default.inc
 */

/**
 * Implements hook_views_default_views().
 */
function bosa_voucher_views_views_default_views() {
  $export = array();

  $view = new view();
  $view->name = 'bosa_voucher';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'commerce_product';
  $view->human_name = 'BOSA Voucher';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'BOSA Voucher';
  $handler->display->display_options['use_ajax'] = TRUE;
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['use_more_text'] = 'mere';
  $handler->display->display_options['access']['type'] = 'none';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['exposed_form']['options']['submit_button'] = 'Udfør';
  $handler->display->display_options['exposed_form']['options']['reset_button_label'] = 'Gendan';
  $handler->display->display_options['exposed_form']['options']['exposed_sorts_label'] = 'Sortér efter';
  $handler->display->display_options['exposed_form']['options']['sort_asc_label'] = 'Stigende';
  $handler->display->display_options['exposed_form']['options']['sort_desc_label'] = 'Faldende';
  $handler->display->display_options['pager']['type'] = 'mini';
  $handler->display->display_options['pager']['options']['items_per_page'] = '12';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['pager']['options']['id'] = '0';
  $handler->display->display_options['pager']['options']['quantity'] = '9';
  $handler->display->display_options['pager']['options']['expose']['items_per_page_label'] = 'Antal elementer';
  $handler->display->display_options['pager']['options']['expose']['items_per_page_options_all_label'] = '- Alle -';
  $handler->display->display_options['pager']['options']['expose']['offset_label'] = 'Forskydning';
  $handler->display->display_options['pager']['options']['tags']['first'] = '« første';
  $handler->display->display_options['pager']['options']['tags']['previous'] = '‹ forrige';
  $handler->display->display_options['pager']['options']['tags']['next'] = 'næste ›';
  $handler->display->display_options['pager']['options']['tags']['last'] = 'sidste »';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'product_id' => 'product_id',
    'title' => 'title',
    'sku' => 'sku',
    'field_offer_dato' => 'field_offer_dato',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'product_id' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'title' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'sku' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'field_offer_dato' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* Opførsel ved ingen resultater: Global: Tekstområde */
  $handler->display->display_options['empty']['area']['id'] = 'area';
  $handler->display->display_options['empty']['area']['table'] = 'views';
  $handler->display->display_options['empty']['area']['field'] = 'area';
  $handler->display->display_options['empty']['area']['empty'] = TRUE;
  $handler->display->display_options['empty']['area']['content'] = 'Ingen produkter fundet, prøv at ændre kriterier';
  $handler->display->display_options['empty']['area']['format'] = 'filtered_html';
  /* Felt: Commerce Product: Produkt-ID */
  $handler->display->display_options['fields']['product_id']['id'] = 'product_id';
  $handler->display->display_options['fields']['product_id']['table'] = 'commerce_product';
  $handler->display->display_options['fields']['product_id']['field'] = 'product_id';
  $handler->display->display_options['fields']['product_id']['label'] = '';
  $handler->display->display_options['fields']['product_id']['exclude'] = TRUE;
  $handler->display->display_options['fields']['product_id']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['product_id']['link_to_product'] = 0;
  /* Felt: Commerce Product: Titel */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'commerce_product';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['title']['alter']['path'] = 'bosa/voucher/product/[product_id]';
  $handler->display->display_options['fields']['title']['link_to_product'] = 0;
  /* Felt: Commerce Product: Varenummer */
  $handler->display->display_options['fields']['sku']['id'] = 'sku';
  $handler->display->display_options['fields']['sku']['table'] = 'commerce_product';
  $handler->display->display_options['fields']['sku']['field'] = 'sku';
  $handler->display->display_options['fields']['sku']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['sku']['alter']['path'] = 'bosa/voucher/product/[product_id]';
  $handler->display->display_options['fields']['sku']['link_to_product'] = 0;
  /* Felt: Commerce Product: Dato */
  $handler->display->display_options['fields']['field_offer_dato']['id'] = 'field_offer_dato';
  $handler->display->display_options['fields']['field_offer_dato']['table'] = 'field_data_field_offer_dato';
  $handler->display->display_options['fields']['field_offer_dato']['field'] = 'field_offer_dato';
  $handler->display->display_options['fields']['field_offer_dato']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['field_offer_dato']['alter']['path'] = 'bosa/voucher/product/[product_id]';
  $handler->display->display_options['fields']['field_offer_dato']['settings'] = array(
    'format_type' => 'short',
    'fromto' => 'both',
    'multiple_number' => '',
    'multiple_from' => '',
    'multiple_to' => '',
  );
  /* Filterkriterie: Commerce Product: Titel */
  $handler->display->display_options['filters']['title']['id'] = 'title';
  $handler->display->display_options['filters']['title']['table'] = 'commerce_product';
  $handler->display->display_options['filters']['title']['field'] = 'title';
  $handler->display->display_options['filters']['title']['operator'] = 'contains';
  $handler->display->display_options['filters']['title']['group'] = 1;
  $handler->display->display_options['filters']['title']['exposed'] = TRUE;
  $handler->display->display_options['filters']['title']['expose']['operator_id'] = 'title_op';
  $handler->display->display_options['filters']['title']['expose']['label'] = 'Titel';
  $handler->display->display_options['filters']['title']['expose']['operator'] = 'title_op';
  $handler->display->display_options['filters']['title']['expose']['identifier'] = 'title';
  $handler->display->display_options['filters']['title']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    5 => 0,
  );
  /* Filterkriterie: Commerce Product: Dato (field_offer_dato) */
  $handler->display->display_options['filters']['field_offer_dato_value']['id'] = 'field_offer_dato_value';
  $handler->display->display_options['filters']['field_offer_dato_value']['table'] = 'field_data_field_offer_dato';
  $handler->display->display_options['filters']['field_offer_dato_value']['field'] = 'field_offer_dato_value';
  $handler->display->display_options['filters']['field_offer_dato_value']['operator'] = '>=';
  $handler->display->display_options['filters']['field_offer_dato_value']['group'] = 1;
  $handler->display->display_options['filters']['field_offer_dato_value']['exposed'] = TRUE;
  $handler->display->display_options['filters']['field_offer_dato_value']['expose']['operator_id'] = 'field_offer_dato_value_op';
  $handler->display->display_options['filters']['field_offer_dato_value']['expose']['label'] = 'Fra:';
  $handler->display->display_options['filters']['field_offer_dato_value']['expose']['operator'] = 'field_offer_dato_value_op';
  $handler->display->display_options['filters']['field_offer_dato_value']['expose']['identifier'] = 'field_offer_dato_value';
  $handler->display->display_options['filters']['field_offer_dato_value']['expose']['remember'] = TRUE;
  $handler->display->display_options['filters']['field_offer_dato_value']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    5 => 0,
  );
  $handler->display->display_options['filters']['field_offer_dato_value']['form_type'] = 'date_popup';
  $handler->display->display_options['filters']['field_offer_dato_value']['default_date'] = 'now';
  /* Filterkriterie: Commerce Product: Dato (field_offer_dato) */
  $handler->display->display_options['filters']['field_offer_dato_value_1']['id'] = 'field_offer_dato_value_1';
  $handler->display->display_options['filters']['field_offer_dato_value_1']['table'] = 'field_data_field_offer_dato';
  $handler->display->display_options['filters']['field_offer_dato_value_1']['field'] = 'field_offer_dato_value';
  $handler->display->display_options['filters']['field_offer_dato_value_1']['operator'] = '<=';
  $handler->display->display_options['filters']['field_offer_dato_value_1']['group'] = 1;
  $handler->display->display_options['filters']['field_offer_dato_value_1']['exposed'] = TRUE;
  $handler->display->display_options['filters']['field_offer_dato_value_1']['expose']['operator_id'] = 'field_offer_dato_value_1_op';
  $handler->display->display_options['filters']['field_offer_dato_value_1']['expose']['label'] = 'Til:';
  $handler->display->display_options['filters']['field_offer_dato_value_1']['expose']['operator'] = 'field_offer_dato_value_1_op';
  $handler->display->display_options['filters']['field_offer_dato_value_1']['expose']['identifier'] = 'field_offer_dato_value_1';
  $handler->display->display_options['filters']['field_offer_dato_value_1']['expose']['remember'] = TRUE;
  $handler->display->display_options['filters']['field_offer_dato_value_1']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    5 => 0,
  );
  $handler->display->display_options['filters']['field_offer_dato_value_1']['form_type'] = 'date_popup';
  /* Filterkriterie: Global: PHP */
  $handler->display->display_options['filters']['php']['id'] = 'php';
  $handler->display->display_options['filters']['php']['table'] = 'views';
  $handler->display->display_options['filters']['php']['field'] = 'php';
  $handler->display->display_options['filters']['php']['use_php_setup'] = 0;
  $handler->display->display_options['filters']['php']['php_filter'] = 'if(function_exists(\'bosa_products_permissions_check\')){
  global $user;
  return !bosa_products_permissions_check($row->product_id, $user->uid);
}';

  /* Display: Block */
  $handler = $view->new_display('block', 'Block', 'block');
  $handler->display->display_options['defaults']['hide_admin_links'] = FALSE;
  $export['bosa_voucher'] = $view;

  return $export;
}
