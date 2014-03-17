<?php

/**
 * bosa_voucher_ticket_page()
 *
 * ticket page callback, handles scanned qr codes
 *
 * @param
 *  id - ticket id
 *
 * @return
 *  mixed
 */
function bosa_voucher_ticket_page($id = NULL){
  // Check if user is logged in
  global $user;

  if(!$user->uid) {
    return drupal_render(drupal_get_form('user_login_block'));
  }

  $id_array = explode('-', $id);

  $line_item = commerce_line_item_load($id_array[1]);
  $product_id =  $line_item->commerce_product[LANGUAGE_NONE][0]['product_id'];

  // Verify line_item_id and product_id
  if($id_array[0] == variable_get('bosa_voucher_qr_prefix') &&
    $product_id == $id_array[2]){

    // Check for permissions if bosa_products module is installed 
    if(bosa_products_permissions_check($product_id, $user->uid)){

      if(bosa_voucher_save_ticket($id_array[1], $id_array[2], $user->uid)) {
        drupal_set_message(t('Ticket registered'));
      }
      else {
        drupal_set_message(t('Ticket already used'), 'warning');
      }

      drupal_goto('bosa/voucher/product/'.$id_array[2]);
    }
  }
  return t('Not valid ticket'); 
}


function bosa_voucher_product_page($id = NULL){
  // Check if user is logged in
  global $user;

  if(!$user->uid) {
    return drupal_render(drupal_get_form('user_login_block'));
  }

  // Redirect to all products page
  if($id == 'all') {
    return bosa_voucher_product_all_page();
  }

  return '<a href="/bosa/voucher/product/all"><h3>&lt;&lt; See for other product</h3></a>' . drupal_render(drupal_get_form('bosa_voucher_ticket_form')) . '<div id="bosa_voucher_load" style="display:none;">Opdaterer...</div><div id="bosa_voucher_attendees"></div>';
}


function bosa_voucher_product_page_ajax($id = NULL){
  print bosa_voucher_attendees($id);
}

function bosa_voucher_product_all_page(){
  return views_embed_view('bosa_voucher', 'block');


  //return drupal_render(drupal_get_form('bosa_voucher_product_form'));
}

function bosa_voucher_ticket_form($form, &$form_state) {
  $form['ticket_id'] = array(
    '#title' => t('Ticket id'),
    '#type' => 'textfield',
  );
  $form['submit'] = array(
    '#value' => t('Submit'),
    '#type' => 'submit',
  );
  return $form;
}

function bosa_voucher_ticket_form_submit($form, &$form_state){
  global $user;
  $id_array = explode('-', $form_state['values']['ticket_id']);

  if(isset($id_array[1]) && isset($id_array[2])){
    // @todo:
    // Validation that entered value is valid ticket id

    if(bosa_voucher_save_ticket($id_array[1], $id_array[2], $user->uid)) {
      drupal_set_message(t('Ticket registered'));
    }
    else {
      drupal_set_message(t('Ticket already used or invalid ticket id'), 'warning');
    }
  }
}

/**
 * From here on it gets a little wierd!
 *
 * Reused code form bosa_attendees module, must be cleaned up.
 */
function bosa_voucher_product_form($form, &$form_state) {
  global $user;

  // Fetch a list of product names for the dropdown
  $productsQuery = new EntityFieldQuery();
  $productsQuery->entityCondition('entity_type', 'commerce_product')
   ->propertyOrderBy('title', 'ASC');

  $products = $productsQuery->execute();

  $offerOptions = array('0' => 'Vælg');
  if( isset($products['commerce_product']))
  {
    foreach( $products['commerce_product'] as $product)
    {
      // Check for permissions if bosa_products module is installed 
      if(function_exists('bosa_products_permissions_check')){
        if(!bosa_products_permissions_check($product->product_id, $user->uid)){
          continue;
        }
      }
        $entity = entity_load('commerce_product', array($product->product_id));
        @$offerOptions = $offerOptions + array( $product->product_id => substr($entity[$product->product_id]->title, 0, 20) . '(...) - ' .$entity[$product->product_id]->sku . ' - ' . $entity[$product->product_id]->field_offer_dato[LANGUAGE_NONE][0]['value']);
    }
  }

  // build the forms
  // products
  $form = array();
  $form['products'] = array(
    '#type' => 'select',
    '#options' => $offerOptions,
    '#ajax' => array(
      'callback' => '_bosa_voucher_offer_callback',
      'wrapper' => 'replace_textfield_div',
    ),
    '#prefix' => '<div class="offer"><div class="form-label"><h3>Produkter:</h3></div>',
    '#suffix' => '</div>'
  );

  $form['offer_date'] = array(
    '#type' => 'textfield',
    '#ajax' => array(
      'event' => 'change',
      'callback' => '_bosa_attendees_offer_callback_date',
      'wrapper' => 'replace_textfield_div',
    ),
    '#prefix' => '<div class="date"><div class="form-label">Dato:</div>',
    '#suffix' => '</div><div id="replace_textfield_div"></div>',
  );

   return $form;
}

function _bosa_voucher_offer_callback($form, $form_state) {
  return '<div id="replace_textfield_div"><div class="print-page"><a href="'.$link.'">Udskriv</a></div>' . bosa_voucher_attendees($form_state['values']['products']).  '</div>';
}

function bosa_voucher_attendees($product_id) {
  $extras_html = '';
  $product = commerce_product_load($product_id);
  $extraLineItems = _bosa_voucher_extras_by_product_id($product_id);

  $lineItemQuery = new EntityFieldQuery();
  $lineItemQuery->entityCondition('entity_type', 'commerce_line_item')
   ->propertyCondition('line_item_label', $product->sku, '='); 

  $lineItems = $lineItemQuery->execute();


  if(!empty($lineItems)) {
    foreach ($lineItems['commerce_line_item'] as $lineItem) 
    {
      $attendees = _bosa_voucher_from_line_item_id($lineItem->line_item_id);
      if(!empty($attendees))
      {
        $end = array('order_id' => '', 'comment' => '');

        if(isset($allAttendees))
        {
          $end = end($allAttendees);
        }

        if($end['order_id'] == $attendees['order_id'] && $end['comment'] == $attendees['comment'])
        {

          $allAttendees[sizeof($allAttendees)-1]['quantity']++;
        }
        else
        {
          $allAttendees[] = $attendees;
        }
      }
    }
  }

  foreach($extraLineItems as $product_sku => $line_item_ids){
    $extraAttendees = array();

    foreach($line_item_ids as $line_item_id){
      $attendees = _bosa_voucher_from_line_item_id($line_item_id);
      $extraAttendees[] = $attendees;

    }

    $extras_html .= '<table><tr><td><h3>' . t('Extra product:') .' '. $attendees['product_title'] . '</h3></td></tr><tr><td></td><td>'.t('Price:').'  '.$attendees['product_price'].'</td></tr></table>';
    $extras_html .= _bosa_voucher_attendees_html($extraAttendees);
  }

  $nodeQuery = new EntityFieldQuery();

  $nodeQuery->entityCondition('entity_type', 'node')
      ->fieldCondition('field_reference', 'product_id', $product->product_id, '=');

  $nodeResult = $nodeQuery->execute();

  $title = '';
  if(is_array($nodeResult['node'])){
    $node = node_load(key($nodeResult['node']));
    $title = $node->title;
  }

  $price = '';
  $second_price = '';

  $price = commerce_currency_format($product->commerce_price[LANGUAGE_NONE][0]['amount'], $product->commerce_price[LANGUAGE_NONE][0]['currency_code']);
  if(!empty($product->field_2nd_price)){
    $second_price = commerce_currency_format($product->field_2nd_price[LANGUAGE_NONE][0]['amount'], $product->field_2nd_price[LANGUAGE_NONE][0]['currency_code']);
  }
  $dateandtime = '';
  if( isset($product->field_offer_dato)){
    $dateandtime = strtotime($product->field_offer_dato[LANGUAGE_NONE][0]['value']); 
    $dateandtime = date('d-m-Y G:i:s', $dateandtime);
  }
  $header = '<table><tr><td class="header"><h3>'.$title.'</h3><p>' .$product->title .' '. $dateandtime .'</p></td></tr><tr><td>'.t('Prices:').'</td><td>'.t('Reg.:').'</td><td>'.$price.'</td></tr><tr><td></td><td>'.t('2nd:').'</td><td>'.$second_price.'</td></tr></table>';

  return $header . _bosa_voucher_attendees_html($allAttendees) . $extras_html;
}

/*
 * Function for fetching name, quanitity an comments on products with specified line item id
 * @param
 *  lineitem id for product
 * @return
 */
function _bosa_voucher_from_line_item_id($line_item_id)
{
  $lineItemsEntity= entity_load('commerce_line_item', array($line_item_id));
  $lineItemEntity = reset($lineItemsEntity);

  $product = commerce_product_load_by_sku($lineItemEntity->line_item_label);

  $orderEntity = entity_load('commerce_order', array($lineItemEntity->order_id));

  $orderEntity = reset($orderEntity);

  $comment = isset($lineItemEntity->field_line_item_comment[LANGUAGE_NONE][0]['value'])  ? $lineItemEntity->field_line_item_comment[LANGUAGE_NONE][0]['value'] : '-';

  if(isset($orderEntity->status))
  {
    if ($orderEntity->status == 'completed')
    {
      $wrapper = entity_metadata_wrapper('commerce_order', $orderEntity);
      $address = $wrapper->commerce_customer_billing->commerce_customer_address->value();
      $phone = $wrapper->commerce_customer_billing->field_mobile_phone->value();
      $ticket = bosa_voucher_check_ticket($lineItemEntity->line_item_id, $product->product_id);
      $name = '-';
      $register = '<a href="/bosa/voucher/ticket/'. variable_get('bosa_voucher_qr_prefix') .'-'. $lineItemEntity->line_item_id . '-'. $lineItemEntity->commerce_product[LANGUAGE_NONE][0]['product_id'] .'"><i class="register-ticket"></i></a>';

      if(!empty($ticket)){
        $register = date('H:i:s d-m-y', $ticket[0]);
        $user = user_load($ticket[1]);
        $name = $user->name;
      }

      // Module bosa_attendees, implements this function.
      if(function_exists('bosa_attendees_get_attendees_markup')){
        $attendee_names = bosa_attendees_get_attendees_markup($lineItemEntity, $address['name_line']);
      }
      else {
        $attendee_names = $address['name_line'];
      }

      $attendees = array(
        'name' => $attendee_names,
        'phone' => $phone,
        'quantity' => (int)$lineItemEntity->quantity,
        'order_id' => $orderEntity->order_id,
        'comment' => $comment,
        'line_item_id' => $lineItemEntity->line_item_id,
        'product_sku' => $lineItemEntity->line_item_label,
        'product_title' => $product->title,
        'price' => commerce_currency_format($lineItemEntity->commerce_total[LANGUAGE_NONE][0]['amount'], $lineItemEntity->commerce_total[LANGUAGE_NONE][0]['currency_code']),
        'product_price' => commerce_currency_format($product->commerce_price[LANGUAGE_NONE][0]['amount'], $product->commerce_price[LANGUAGE_NONE][0]['currency_code']),
        'ticket_reg' => $register,
        'ticket_reg_name' => $name,
      );
    }
  }
  if(isset($attendees)) 
  {
    return $attendees;
  }
}

function _bosa_voucher_extras_by_product_id($productId) {
  $lineItemQuery = new EntityFieldQuery();
  $lineItemQuery->entityCondition('entity_type', 'commerce_line_item')
    ->fieldCondition('field_line_item_ref', 'value', $productId);

  $lineItems = $lineItemQuery->execute(); 
  $extras = array();

  if(isset($lineItems['commerce_line_item'])){
    foreach($lineItems['commerce_line_item'] as $key => $value){

      $lineItemsEntity= entity_load('commerce_line_item', array($key));
      $lineItemsEntity = reset($lineItemsEntity);

      $extras[$lineItemsEntity->line_item_label][] = $lineItemsEntity->line_item_id; 
    }
  }
  return $extras;
}

function _bosa_voucher_attendees_html($allAttendees) {
  $rows = array();
  $offerAttendees = '';
  $number_of_attendees = 0;
  $prevorderid = 0;
  $header =  array(
      array('data' => t('Order number:'),  'class' => 'order_id'),
      array('data' => t('Name:'),          'class' => 'name'),
      array('data' => t('Phone number:'),  'class' => 'phone'),
      array('data' => t('Quantity:'),      'class' => 'quantity'),
      array('data' => t('Comment:'),       'class' => 'comment'),
      array('data' => t('Price:'),         'class' => 'price'),
      array('data' => t('Register:'),      'class' => 'ticket_reg'),
      array('data' => t('By:')            ,'class' => 'ticket_reg_name'),
    );

  if(!empty($allAttendees)){
    foreach($allAttendees as $attendee)
    {

      $order_id = $attendee['order_id'];
      if($prevorderid == $attendee['order_id']){
        $order_id = '';
        $attendee['phone'] = '';
      }

      $prevorderid = $attendee['order_id'];
      $rows[] = array('data' => array(
          array('data' => $order_id,                   'class' => 'order_id'),
          array('data' => $attendee['name'],           'class' => 'name'),
          array('data' => $attendee['phone'],          'class' => 'phone'),
          array('data' => $attendee['quantity'],       'class' => 'quantity'),
          array('data' => $attendee['comment'],        'class' => 'comment'),
          array('data' => $attendee['price'],          'class' => 'price'),
          array('data' => $attendee['ticket_reg'],     'class' => 'ticket_reg'),
          array('data' => $attendee['ticket_reg_name'],'class' => 'ticket_reg_name'),
        ),
        'class' => array('data-row info'),
        'data-id' => array($order_id),
      );

      $rows[] = array('data' => array(
          array('data' => t('Phone:'). '<br />' . $attendee['phone'].'1231231asdf1',          'class' => 'phone'),
          array('data' => t('Comment:'). '<br />' . $attendee['comment'],        'class' => 'comment', 'colspan' => 2),
          array('data' => t('Price:'). '<br />' . $attendee['price'],          'class' => 'price', 'colspan' => 2),
        ),
        'class' => array('data-row extra-info hide order_id-'.$order_id),
      );

      $number_of_attendees += $attendee['quantity'];
    }
  }
  else 
  {
    $rows[] = array(t('No attendees'), '', '', '', '', '');
  }
  $rows[] = array('data' => array(
      t('Total:'),
      array('data' =>'', 'class' => 'footer-extra'),
      '',
      $number_of_attendees, 
      '',
      array('data' =>'', 'class' => 'footer-extra'),
      array('data' =>'', 'class' => 'footer-extra'),
      '',
    ),
    'class' => array('attendees-footer'),
  );

  $offerAttendees .= theme('table', array('header' => $header, 'rows' =>  $rows, 'attributes' => array('class' => array('attendees'))));

  return $offerAttendees;
}


