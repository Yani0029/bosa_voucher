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

  return drupal_render(drupal_get_form('bosa_voucher_ticket_form')) . bosa_voucher_attendees($id);
}


function bosa_voucher_product_all_page(){
  return drupal_render(drupal_get_form('bosa_voucher_product_form'));
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

  // @todo:
  // Validation that entered value is valid ticket id
  
  if(bosa_voucher_save_ticket($id_array[1], $id_array[2], $user->uid)) {
    drupal_set_message(t('Ticket registered'));
  }
  else {
    drupal_set_message(t('Ticket already used'), 'warning');
  }
}

/**
 * From here on it gets a little wierd!
 */
function bosa_voucher_product_form($form, &$form_state) {
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
        $offerOptions = $offerOptions + array( $product->product_id => substr($entity[$product->product_id]->title, 0, 20) . '(...) - ' .$entity[$product->product_id]->sku . ' - ' . $entity[$product->product_id]->field_offer_dato[LANGUAGE_NONE][0]['value']);
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
 
  $price = commerce_currency_format($product->commerce_price[LANGUAGE_NONE][0]['amount'], $product->commerce_price[LANGUAGE_NONE][0]['currency_code']);
  $second_price = commerce_currency_format($product->field_2nd_price[LANGUAGE_NONE][0]['amount'], $product->field_2nd_price[LANGUAGE_NONE][0]['currency_code']);
  
  $dateandtime = strtotime($product->field_offer_dato[LANGUAGE_NONE][0]['value']); 
  $dateandtime = date('d-m-Y G:i:s', $dateandtime);
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
    if ($orderEntity->status !== 'canceled' || $orderEntity->status !== 'cart')
    {
      $wrapper = entity_metadata_wrapper('commerce_order', $orderEntity);
      $address = $wrapper->commerce_customer_billing->commerce_customer_address->value();
      $phone = $wrapper->commerce_customer_billing->field_mobile_phone->value();
      $ticket = bosa_voucher_check_ticket($lineItemEntity->line_item_id, $product->product_id);
      $name = '-';
      $date = '-';

      if(!empty($ticket)){
        $date = date('H:i:s d-m-Y', $ticket[0]);
        $user = user_load($ticket[1]);
        $name = $user->name;
      }
      $attendees = array(
        'name' => $address['name_line'],
        'phone' => $phone,
        'quantity' => (int)$lineItemEntity->quantity,
        'order_id' => $orderEntity->order_id,
        'comment' => $comment,
        'line_item_id' => $lineItemEntity->line_item_id,
        'product_sku' => $lineItemEntity->line_item_label,
        'product_title' => $product->title,
        'price' => commerce_currency_format($lineItemEntity->commerce_total[LANGUAGE_NONE][0]['amount'], $lineItemEntity->commerce_total[LANGUAGE_NONE][0]['currence_code']),
        'product_price' => commerce_currency_format($product->commerce_price[LANGUAGE_NONE][0]['amount'], $lineItemEntity->commerce_price[LANGUAGE_NONE][0]['currence_code']),
        'ticket_reg' => $date,
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

  if(is_array($lineItems['commerce_line_item'])){
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

  $header = array(
    t('Order number:'), 
    t('Name:'), 
    t('Phone number:'), 
    t('Quantity:'), 
    t('Comment:'), 
    t('Price:'), 
    t('Ticket registered:'), 
    t('By:')
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
      $rows[] = array(
        $order_id,
        $attendee['name'], 
        $attendee['phone'], 
        $attendee['quantity'], 
        $attendee['comment'], 
        $attendee['price'], 
        $attendee['ticket_reg'], 
        $attendee['ticket_reg_name']
      );

      $number_of_attendees += $attendee['quantity'];
    }
  }
  else 
  {
    $rows[] = array(t('No attendees'), '', '', '', '', '');
  }
  $rows[] = array(t('Total:'), '', '', $number_of_attendees, '', '', ''); 

  $offerAttendees .= theme('table', array('header' => $header, 'rows' =>  $rows));

  return $offerAttendees;
}


