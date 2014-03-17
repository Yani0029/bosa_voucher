<?php
/**
 * page callback for menu hook
 *
 * @access public
 * @param 
 *  int $order_id, order id from commerce
 *  int $arg, profile_id and order created time
 * @return 
 *  _bosa_offer_mail_create_pdf callback
 **/
function bosa_voucher_pdf($order_id = NULL, $arg = NULL){
  global $user;
  

  $order = commerce_order_load($order_id);


  // Allow the customer to download the voucher from the checkout complete page
  $tmp = explode('/', $_SERVER['HTTP_REFERER']);
  $complete = array_pop($tmp);  
  $order_id_tmp = array_pop($tmp);  
  $checkout = array_pop($tmp);  

  if( $complete == 'complete' && is_numeric($order_id_tmp) && $checkout == 'checkout' ){
    $order = commerce_order_load($order_id_tmp);
    return bosa_voucher_create_pdf($order);
  }

  //entity_get_controller('commerce_order')->resetCache(array($order->order_id));

  if(is_object($order)){
    $key = $order->commerce_customer_billing[LANGUAGE_NONE][0]['profile_id'];
    $key .= $order->created;


    // Allow the admin or offer admin to download the voucher from the orderslist page
    if($key == $arg || (in_array('administrator', $user->roles) || in_array('Offer admin', $user->roles ) ))
    {
      return bosa_voucher_create_pdf($order);
    }
    else {
      return "Access denied";
    }
  }
  else {
    return "Not found";
 }
}

/**
 * create pdf containing voucher using dompdf
 *
 * @todo
 *  split this up!
 *
 * @access public
 * @param
 *  object $order, commerce order entity
 * @return
 *
 **/
function bosa_voucher_create_markup($order){

  global $base_url;

  $profile_id = $order->commerce_customer_billing[LANGUAGE_NONE][0]['profile_id'];
  $profile = commerce_customer_profile_load($profile_id);

  $language_code = bosa_voucher_get_language_code($profile->commerce_customer_address[LANGUAGE_NONE][0]['country']);

  $node_ids = bosa_voucher_get_node_ids($order->commerce_line_items[LANGUAGE_NONE]);

  $template_node = node_load(variable_get('bosa_voucher_pdf_node_id'));
  if(!is_object($template_node)){
    return 'No template node defined, or node does not exist!';
  }

  $preface = $template_node->field_preface[$language_code][0]['value'];
  $contactinfo = $template_node->field_contactinfo[$language_code][0]['value'];
  $postscript = $template_node->field_postscript[$language_code][0]['value'];
  $logo = 'sites/default/files/' . $template_node->field_logo[LANGUAGE_NONE][0]['filename'];

  // Replace tokens
  $preface = str_replace('[order_number]', $order->order_number, $preface);
  $preface = str_replace('[order_date]', date('d/m-Y H:i:s', $order->changed), $preface);
  $preface = str_replace('[customer_name]', $profile->commerce_customer_address[LANGUAGE_NONE][0]['name_line'], $preface);
  $preface = str_replace('[customer_street]', $profile->commerce_customer_address[LANGUAGE_NONE][0]['thoroughfare'], $preface);
  $preface = str_replace('[customer_address]', $profile->commerce_customer_address[LANGUAGE_NONE][0]['postal_code'] . ' '  . $profile->commerce_customer_address[LANGUAGE_NONE][0]['locality'], $preface);
  $preface = str_replace('[customer_country]', $profile->commerce_customer_address[LANGUAGE_NONE][0]['country'], $preface);

  // Counting pages in order to insert proper pagebreaks
  $pages = 0;
  $currentpage = 0;

  $view = '<html>
    <head>
    <link rel="stylesheet" type="text/css" href="'.drupal_get_path('module', 'bosa_voucher').'/css/pdf.css">
        <meta http-equiv="content-type" content="text/html;charset=utf-8">
        </head>
        <body>';

  foreach ($order->commerce_line_items[LANGUAGE_NONE] as $value)
  {
    $line_item_entity = commerce_line_item_load($value['line_item_id']);

    if(!empty($line_item_entity->field_line_item_ref))
    {
      $product_reference[$line_item_entity->field_line_item_ref[LANGUAGE_NONE][0]['value']][] = array(
        'title' => $line_item_entity->line_item_label,
        'quantity' => $line_item_entity->quantity,
        'comment' => !empty($line_item_entity->field_line_item_comment) ? $line_item_entity->field_line_item_comment[LANGUAGE_NONE][0]['value'] : '',
      );
    }

    if($line_item_entity->type == 'product') {
      $pages++;
    }
  }

  // The actual page markup generation
  foreach ($order->commerce_line_items[LANGUAGE_NONE] as $value)
  {
    error_log(print_r($value, 1));
    $currentpage++;

    $node_id = bosa_voucher_get_node_ids(array($value));
    if (is_numeric($node_id[0])) 
    {
      $line_item_entity = commerce_line_item_load($value['line_item_id']);
      if($line_item_entity->type == 'product')
      {
        $commerce_product_entity = commerce_product_load($line_item_entity->commerce_product[LANGUAGE_NONE][0]['product_id']);
        // If the date field is empty we need to insert some custom text, this in an array that holds this text, with languagecodes.
        $empty_date_time = array(
          'en' => 'Se gyldighed nedenfor',
          'da' => 'Se gyldighed nedenfor',
          'de' => 'Se gyldighed nedenfor',
        );

        $date_time =  $empty_date_time[$language_code];

        if(!empty($commerce_product_entity->field_offer_dato)){
          $date_time = $commerce_product_entity->field_offer_dato[LANGUAGE_NONE][0]['value'];
        }


        if(empty($commerce_product_entity->field_2nd_price_select)){
          $price_info = array(
            'en' => 'Voksen/Barn',
            'da' => 'Voksen/Barn',
            'de' => 'Voksen/Barn',
          );
        }
        else {
          $price_info = array(
              'en' => 'Voksen',
              'da' => 'Voksen',
              'de' => 'Voksen',
          );
        }

        // Some functionality for the second price stuff
        if(!empty($line_item_entity->field_line_item_comment)){
          $price_info = array(
              'en' => 'Barn',
              'da' => 'Barn',
              'de' => 'Barn',
          );
        }

        $price = commerce_currency_format($line_item_entity->commerce_unit_price[LANGUAGE_NONE][0]['amount'], $line_item_entity->commerce_unit_price[LANGUAGE_NONE][0]['currency_code']);

        // Begin the ticket markup
        $view .= '<table><tr>
              <td class="left">';

        $view .= $preface;

        $view .= '</td><td class="right"><img src="' . $logo  . '">';

        $view .= $contactinfo;

        $view .= '</td></tr></table>';

        $view .=
          '<table class="product-info"><tr><td><span class="field title">'.$commerce_product_entity->title.'</span><br />'.
          '<span class="label">Dato og tidspunkt:</span><span class="field-info">'.$date_time.'</span><br />'.
          '<span class="label">Antal: </span><span class="field-info">'.(int)$line_item_entity->quantity.' x '.$price_info[$language_code].' ('.$price.') </span><br />'.
          '</td>';

        if(variable_get('bosa_voucher_qr_enable')){
          $ticket_id = bosa_voucher_ticket_id($line_item_entity);
          $view .= '<td><span style="text-align:center" class="qr"><img src="' . bosa_voucher_qrcode($ticket_id) . '" /><br />';
          $view .= $ticket_id . '</span></td>';
        }
        
        $view .= '</tr></table>';

        $view .= '<table class="middle"><tr><td class="description">';

        $view .= bosa_voucher_get_node_info($node_id[0], 'product_description', $language_code);

        $view .= '</td><td>';
        
        $view .= '<table class="info">';

        $view .= bosa_voucher_get_node_info($node_id[0], 'seller_info', $language_code);


        $view .= bosa_voucher_get_node_info($node_id[0], 'product_info', $language_code);
        $view .= '</table>';

        $view .= '</td></tr></table>';
      }

      // list extra products
      if(isset($product_reference[$line_item_entity->data['context']]) || !empty($product_reference[$line_item_entity->data['context']['product_ids'][0]]))
      {
        $view .= '<div class="extras">Tilkøbsprodukter</div>';
        foreach($product_reference[$line_item_entity->data['context']['product_ids'][0]] as $product_extras)
        {
          $view .= '<span class="label">Produkt: </span><span class="field-info">'.$product_extras['title'].'</span><br />';
          $view .= '<span class="label">Antal: </span><span class="field-info">'.(int)$product_extras['quantity'].'</span><br />';
          $view .= '<span class="label">Kommentar: </span><span class="field-info">'.$product_extras['comment'].'</span><br />';
        }
      }
    }
    if($line_item_entity->type == 'product')
    {
      $view .= '<br /><br />'.$postscript;
    }

    // if there are more products on the order we want a pretty page break
    if($pages > $currentpage)
    {
      $view .= '<div class="page-break"></div>';
    }
  }

  $view .= '</body></html>';

  return $view;
}

/*
 * Generate pdf and save file or return stream
 *
 * @param
 *  obj - commerce order
 *  string - optional
 *
 * @return
 *  mixed
 */
function bosa_voucher_create_pdf($order, $param = NULL){
  require_once libraries_get_path('dompdf') . '/dompdf_config.inc.php';

  $view = bosa_voucher_create_markup($order);

  //print $view;
  //die();

  $dompdf = new DOMPDF();

  $dompdf->load_html($view);
  $dompdf->set_paper("a4","portrait");
  $dompdf->render();

  if($param == 'file') {
    $file = file_save_data($dompdf->output(), 'public://Ordre_'.$order_id. '.pdf');

    return $file;
  }

  $dompdf->stream('Ordre_'.$order->order_id.'.pdf');
}

/*
 * Save pdf to filesystem
 *
 * @param
 *  int - order id
 *
 * @return
 *  string - filepath, realpath
 */
function bosa_voucher_save_pdf($order_id) {
  $order = commerce_order_load($order_id);
  $file = bosa_voucher_create_pdf($order, 'file');

  return drupal_realpath($file->uri);
}

/*
 * Generate ticket id
 *
 * @param
 *  int- lineitem id
 *
 * @return
 *  string - ticket id
 */
function bosa_voucher_ticket_id($line_item) {
  $prefix = variable_get('bosa_voucher_qr_prefix');

  $line_item_id = $line_item->line_item_id;

  $product_id = $line_item->commerce_product[LANGUAGE_NONE][0]['product_id'];

  return $prefix . '-' . $line_item_id . '-' . $product_id;
}

/**
 * Generate QR code
 *
 * @param
 *  int - ticket id
 *
 * @return
 *  string - filepath
 */
function bosa_voucher_qrcode($ticket_id) {
  require_once libraries_get_path('phpqrcode') . '/qrlib.php';
  global $base_url;

  $dir_path = drupal_realpath(file_default_scheme() . '://');

  $full_path = $dir_path . '/' . $ticket_id . '.png'; 

  QRcode::png($base_url . '/bosa/voucher/ticket/' . $ticket_id , $full_path, QR_ECLEVEL_L, 10, 1);

  $image = image_load($full_path);

  image_resize($image, 100, 100);

  image_save($image, $full_path);

  return $full_path;
}

/**
 * Simple helper function for getting language code from customer
 * country
 */
function bosa_voucher_get_language_code($country_code) {
  $languages = array(
    'DK' => 'da',
    'DE' => 'de',
    );

  if(isset($languages[$country_code])){
    return $languages[$country_code];
  }

  return 'en';
}

function bosa_voucher_get_node_ids($line_items) {
  $node_ids = array();

  foreach($line_items as $line_item)
  {
    // load lineitem to get line_item_label
    $line_item_entity = commerce_line_item_load($line_item['line_item_id']);

    // load product to get id
    $product_entity = commerce_product_load_by_sku($line_item_entity->line_item_label);

    // entity search for nodes with products attached
    $query = new EntityFieldQuery;

    $query->entityCondition('entity_type', 'node')
      ->fieldCondition('field_reference', 'product_id', $product_entity->product_id, '=');

    $result = $query->execute();
    if(!empty($result))
    {
      $result = reset($result['node']);
    // save node ids
    
      if (in_array($result->nid, $node_ids))
      {
        continue;
      }
    $node_ids[] = $result->nid;
    }
  }
  
  return $node_ids;
}

function bosa_voucher_get_node_info($nid, $arg, $language_code){
  $node = node_load($nid);

  // Field info and labels
  $fields = array();

  $fields['product_info'] = array(
    'field_forplejning' => array(
      'en' => 'Food/Drinks',
      'da' => 'Forplejning',
      'de' => 'Verpflegung',
    ),
    'field_turledere' => array(
      'en' => 'Guides',
      'da' => 'Turledere',
      'de' => 'Reiseleiter',
    ),
    'field_varighed' => array(
      'en' => 'Duration',
      'da' => 'Varighed',
      'de' => 'Dauer',
    ),
    'field_alder' => array(
      'en' => 'Age',
      'da' => 'Alder',
      'de' => 'Alter',
    ),
    'field_diplom' => array(
      'en' => 'Diploma',
      'da' => 'Diplom',
      'de' => 'Diplom',
    ),
    'field_m_dested' => array(
      'en' => 'Meeting place',
      'da' => 'Mødested',
      'de' => 'Treffpunkt',
    ),
    'field_husk' => array(
      'en' => 'Remember',
      'da' => 'Husk',
      'de' => 'Bitte daran denken',
    ),
    'field_sikkerhed' => array(
      'en' => 'Safety',
      'da' => 'Sikkerhed',
      'de' => 'Sicherheit',
    ),
     'field_validity' => array(
      'en' => 'Validity',
      'da' => 'Gyldighed',
      'de' => 'Gültigkeit',
    ),
 );

  $fields['seller_info'] = array(
    'field_seller_name' => array(
      'en' => '<tr><td class="seller" colspan="2">[value]</td></tr>',
      'da' => '<tr><td class="seller" colspan="2">[value]</td></tr>',
      'de' => '<tr><td class="seller" colspan="2">[value]</td></tr>',
    ),
    'locations/street' => array(
      'en' => 'Street',
      'da' => 'Vejnavn',
      'de' => 'Adresse',
    ),
    'locations/city' => array(
      'en' => 'City',
      'da' => 'By',
      'de' => 'Ort',
    ),
    'locations/postal_code' => array(
      'en' => 'Postalcode',
      'da' => 'Postnummer',
      'de' => 'Postleitzahl',
    ),
    'field_phone' => array(
      'en' => 'Phone',
      'da' => 'Telefon',
      'de' => 'Telefon',
    ),
    'field_offer_url' => array(
      'en' => 'Homepage',
      'da' => 'Hjemmeside',
      'de' => 'Homepage',
    ),
    'field_offer_email' => array(
      'en' => 'E-mail',
      'da' => 'E-mail',
      'de' => 'E-mail',
    ),
  );

  $fields['product_description'] = array(
    'title_field' => array(
      'en' => '<h2>[value]</h2>',
      'da' => '<h2>[value]</h2>',
      'de' => '<h2>[value]</h2>',
    ),
    'field_subtitle' => array(
      'en' => '<h4>[value]</h4>',
      'da' => '<h4>[value]</h4>',
      'de' => '<h4>[value]</h4>',
    ),
    'field_offer_you_get' => array(
      'en' => '[value]',
      'da' => '[value]',
      'de' => '[value]',
    ),
  );

  $desc_cutoff = array(
    'en' => '<br />You can read the entire description here: ',
    'da' => '<br />Du kan læse hele beskrivelsen her: ',
    'de' => '<br />Sie können die gesamte Beschreibung hier lesen: ',
  );

  $info = '';

  // Fetch field info for requested fields
  foreach($fields[$arg] as $field => $labels) {
    // Standard drupal fields
    if(strstr($field, 'field')){
      if(!empty($node->$field)){

        // Store field contents temporarily
        $tmp = $node->$field;

        // Does the node contain translated content,
        // in specified language
        if(isset($tmp[$language_code])) {
          $value = $tmp[$language_code][0]['value'];
        }
        // If not just fetch the first element
        else {
          $tmp = reset($tmp);
          $value = $tmp[0]['value'];
        }

        unset($tmp);
      }
    }
    // Non standard fields
    else if(strstr($field, '/')) {
      $field_exp = explode('/', $field);

      $tmp = $node->$field_exp[0];

      if(!empty($tmp[0][$field_exp[1]])){
        $value = $tmp[0][$field_exp[1]];
      }
    }

    // Only set label if label i specified
    $label = !empty($labels[$language_code]) ? $labels[$language_code] . ': ' : '';

    if(isset($value)) {
          
      $value = strip_tags($value, '<p><em>');

      if($field == 'field_offer_you_get') {
       // $value = strip_tags($value);
        $nodeurl = url('node/'. $node->nid, array('language' => $language_code, 'absolute' => true));
        
        // Intended for the body field
        $value = (strlen($value) > 1600) ? substr($value, 0, 1600) .'... '.$desc_cutoff[$language_code].$nodeurl : $value;
      }
      // We replace the token [value] if set in label, and set the result as info for that field
      // This allows us to wrap the value in markup, as we do with the title and subtitle
      if(strstr($label, '[')){
        $info .= str_replace('[value]', $value, str_replace(':', '', $label));
      } else {
        $info .= '<tr><td class="label">' . $label . '</td><td>' . $value . '</td></tr>';
      
      }
    }
    $label = NULL;
    $value = NULL;
  }

  return $info;
}
