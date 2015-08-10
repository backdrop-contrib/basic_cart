<?php

/**
 * @file
 * Basic cart shopping cart implementation functions.
 */

/**
 * Callback function for cart listing.
 */
function basic_cart_cart() {
  $cart = basic_cart_get_cart();
  
  if (empty($cart)) {
    return t('Your cart is empty.');
  }
  return backdrop_get_form('basic_cart_cart_form');
}

/**
 * Shopping cart form.
 */
function basic_cart_cart_form() {
  // Getting the shopping cart.
  $cart = basic_cart_get_cart();
  // And now the form.
  $form['cartcontents'] = array(
    // Make the returned array come back in tree form.
    '#tree' => TRUE,
    '#prefix' => '<div class="basic-cart-cart basic-cart-grid">',
    '#suffix' => '</div>',
  );
  // Cart elements.
foreach ($cart as $cid => $value) {
if (is_numeric($cid))
{
$node = node_load($cart[$value]);
    $form['cartcontents'][$cid] = array(
      '#type' => 'textfield',
      '#size' => 1,
      '#default_value' => $_SESSION['basic_cart']['cart'][$cid . 'quantity'],
      '#theme' => 'basic_cart_render_cart_element',
    );
  }
  }
  // Total price.
  $form['total_price'] = array(
    '#markup' => t('Total price'),
    '#prefix' => '<div class="basic-cart-cart basic-cart-grid">',
    '#suffix' => '</div>',
    '#theme' => 'basic_cart_cart_total_price',
  );
  // Buttons.
  $form['buttons'] = array(
    // Make the returned array come back in tree form.
    '#tree' => TRUE,
    '#prefix' => '<div class="row"><div class="basic-cart-call-to-action cell">',
    '#suffix' => '</div></div>',
  );
  // Update button.
  $form['buttons']['update'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
  );
  // Checkout button.
  $form['buttons']['checkout'] = array(
    '#type' => 'submit',
    '#value' => t('Checkout'),
  );

  return $form;
}

/**
 * Shopping cart form.
 */
function basic_cart_cart_form_submit($form_id, $form_state) {
  foreach ($form_state['values']['cartcontents'] as $nid => $value) {
    $quantity = (int) $value;
    if ($quantity > 0) {
      $_SESSION['basic_cart']['cart'][$nid . 'quantity'] = $quantity;
    }
    // If the quantity is zero, we just remove the node from the cart.
    elseif ($quantity == 0) {
      unset($_SESSION['basic_cart']['cart'][$nid]);
      unset($_SESSION['basic_cart']['cart'][$nid . 'quantity']);
    }
  }

  if ($form_state['values']['op'] == t('Checkout')) {
    backdrop_goto('checkout');
  }
  else {
    backdrop_set_message(t('Shopping cart updated.'));
  }
}

/**
 * Callback function for cart/add/.
 *
 * @param int $nid
 *   We are using the node id to store the node in the shopping cart
 */
function basic_cart_add_to_cart($nid = NULL) {
  $nid = (int) $nid;
  if ($nid > 0) {
    // If a node is added more times, just update the quantity.
    $cart = basic_cart_get_cart();
    if (!empty($cart) && in_array($nid, array_keys($cart))) {
      // Clicked 2 times on add to cart button. Increment quantity.
    $x = $_SESSION['basic_cart']['cart'][$nid . 'quantity'];
    $x++;
    $_SESSION['basic_cart']['cart'][$nid . 'quantity'] = $x;
    }
    else {
      // Storing in session.
      $_SESSION['basic_cart']['cart'][$nid] = $nid;
      $_SESSION['basic_cart']['cart'][$nid . 'quantity'] = 1;
    }
  }
  backdrop_set_message(t('Shopping cart updated.'));
  
  $redirect = config_get('basic_cart.settings', 'basic_cart_redirect_user_after_add_to_cart');
  if (empty($redirect)) {
    backdrop_goto('cart');
  }
  elseif ($redirect == '<none>') {
    $referer = $_SERVER["HTTP_REFERER"];
    backdrop_goto($referer);
  }
  else {
    backdrop_goto($redirect);
  }
}

/**
 * Callback function for cart/remove/.
 *
 * @param int $nid
 *   We are using the node id to remove the node in the shopping cart
 */
function basic_cart_remove_from_cart($nid = NULL) {
  $nid = (int) $nid;
  if ($nid > 0) {
    unset($_SESSION['basic_cart']['cart'][$nid]);
    unset($_SESSION['basic_cart']['cart'][$nid . 'quantity']);
  }
  backdrop_set_message(t('Shopping cart updated.'));
  backdrop_goto('cart');
}

/**
 * Function for shopping cart retrieval.
 *
 * @param int $nid
 *   We are using the node id to store the node in the shopping cart
 *
 * @return mixed
 *   Returning the shopping cart contents.
 *   An empty array if there is nothing in the cart
 */
function basic_cart_get_cart($nid = NULL) {
  if (isset($nid)) {
    return $_SESSION['basic_cart']['cart'][$nid];
  }
  if (isset($_SESSION['basic_cart']['cart'])) {
    return $_SESSION['basic_cart']['cart'];
  }
  // Empty cart.
  return array();
}

/**
 * Returns the final price for the shopping cart.
 *
 * @return mixed $total_price
 *   The total price for the shopping cart. 
 */
function basic_cart_get_total_price() {
  // Building the return array.
  $return = array(
    'price' => 0,
    'vat' => 0,
    'total' => 0,
  );
  $cart = basic_cart_get_cart();
  if (empty($cart)) {
    return (object) $return;
  }
  
  $total_price = 0;
  foreach ($cart as $cid => $value) {
if (is_numeric($cid))
{
$node = node_load($cart[$value]);
    if (isset($_SESSION['basic_cart']['cart'][$cid . 'quantity']) && isset($node->price["und"][0]["value"])) {
      $total_price += $_SESSION['basic_cart']['cart'][$cid . 'quantity'] * basic_cart_price_format($node->price["und"][0]["value"]);
    }
  }
      }
  $return['price'] = $total_price;
  
  // Checking whether to apply the VAT or not.
  $vat_is_enabled = (int) config_get('basic_cart.settings', 'basic_cart_vat_state');
  if (!empty ($vat_is_enabled) && $vat_is_enabled) {
    $vat_value = (float) config_get('basic_cart.settings', 'basic_cart_vat_value');
    $vat_value = ($total_price * $vat_value) / 100;
    $total_price += $vat_value;
    // Adding VAT and total price to the return array.
    $return['vat'] = $vat_value;
  }
  
  $return['total'] = $total_price;
  return (object) $return;
}

/**
 * Shopping cart reset.
 */
function basic_cart_empty_cart() {
  unset($_SESSION['basic_cart']['cart']);
}

/**
 * Formats the input $price in the desired format.
 *
 * @param float $price
 *   The price in the raw format.
 * @return $price
 *   The price in the custom format.
 */
function basic_cart_price_format($price) {
  $format = config_get('basic_cart.settings', 'basic_cart_price_format');
  $currency = check_plain(config_get('basic_cart.settings', 'basic_cart_currency'));
  $price = (float) $price;
  switch ($format) {
    case 0:
      $price = number_format($price, 2, ',', ' ') . ' ' . $currency;
      break;
    
    case 1:
      $price = number_format($price, 2, '.', ' ') . ' ' . $currency;
      break;
    
    case 2:
      $price = number_format($price, 2, '.', ',') . ' ' . $currency;
      break;
    
    case 3:
      $price = number_format($price, 2, ',', '.') . ' ' . $currency;
      break;
    
    case 4:
      $price = $currency . ' ' . number_format($price, 2, ',', ' ');
      break;
    
    case 5:
      $price = $currency . ' ' . number_format($price, 2, '.', ' ');
      break;
    
    case 6:
      $price = $currency . ' ' . number_format($price, 2, '.', ',');
      break;
    
    case 7:
      $price = $currency . ' ' . number_format($price, 2, ',', '.');
      break;
    
    default:
      $price = number_format($price, 2, ',', ' ') . ' ' . $currency;
      break;
  }
return $price;  
}

/**
 * Returns the available price formats.
 *
 * @return $formats
 *   A list with the available price formats.
 */
function _basic_cart_price_format() {
  $currency = config_get('basic_cart.settings', 'basic_cart_currency');
  return array(
    0 => t('1 234,00 @currency', array('@currency' => $currency)),
    1 => t('1 234.00 @currency', array('@currency' => $currency)),
    2 => t('1,234.00 @currency', array('@currency' => $currency)),
    3 => t('1.234,00 @currency', array('@currency' => $currency)),
    
    4 => t('@currency 1 234,00', array('@currency' => $currency)),
    5 => t('@currency 1 234.00', array('@currency' => $currency)),
    6 => t('@currency 1,234.00', array('@currency' => $currency)),
    7 => t('@currency 1.234,00', array('@currency' => $currency)),
  );
}

/**
 * Checkout.
 */

/**
 * Checkout form implementation.
 */
function basic_cart_checkout() {
  $shopping_cart = basic_cart_get_cart();
  // Price.
  $price = basic_cart_get_total_price();
  $total = $price->total;
  
  $options = array(
    'cart' => $shopping_cart,
    'price' => $total,
  );
  
  // Checking the VAT.
  $vat_is_enabled = (int) config_get('basic_cart.settings', 'basic_cart_vat_state');
  if (!empty ($vat_is_enabled) && $vat_is_enabled) {
    $options['vat'] = basic_cart_price_format($price->vat);
  }
  
  // The flat cart (just the listing part).
  $cart = theme('basic_cart_cart_flat', $options);
  
  // If the cart is empty, we don't want to show the checkout form.
  if (empty($shopping_cart)) {
    return $cart;
  }
  
  $form = backdrop_get_form('basic_cart_checkout_form');
  $form = backdrop_render($form);
  return $cart . $form;
}


/**
 * Ajax validation on email and password.
 *
 * @param array $form
 *   Elements to build the form.
 *
 * @param array $form_state
 *   Values passed during form submission.
 *
 * @return string
 */
function _basic_cart_form_email_ajax_validate($form, &$form_state)
{
    // Validate as a true email format.
    $email  = check_plain(filter_xss($form_state['values']['basic_cart_checkout_email']));
    $query  = 'SELECT name FROM {users} where mail = :mail';
    $result = db_query($query, array(
        ':mail' => $email
    ))->fetchField();

    if (!empty($result)) {
        $error = '<div id="email-error" style="color:#ff0000;">User is already registered.</div>';
        return $error;
    }

    // Validate as a true email format.`
    elseif (filter_var($form_state['values']['basic_cart_checkout_email'], FILTER_VALIDATE_EMAIL) == FALSE) {
        $error = '<div id="email-error" style="color:#ff0000;">The email is not valid.</div>';
        return $error;
    } else {
        $noerror = '<div id="email-error"> </div>';
        return $noerror;
    }
}



/**
 * Checkout form.
 */
function basic_cart_checkout_form() {
  
  // Check for $form_state values returned on form rebuild and when submitting the back button.
    global $user;
    if ($user->uid == 0) {
        $email                    = isset($form_state['values']['basic_cart_checkout_email']) ? $form_state['values']['basic_cart_checkout_email'] : '';
        $form['basic_cart_checkout_email'] = array(
            '#type' => 'textfield',
            '#title' => t('Email'),
            '#required' => TRUE,
            '#default_value' => $email,

            // Perform ajax validation.
            '#ajax' => array(
                'callback' => '_basic_cart_form_email_ajax_validate',
                'wrapper' => 'email-error',
                'effect' => 'slide'
            ),
            '#suffix' => '<div id="email-error" style="color:#ff0000;"> </div>'
        );
    } else {
        $email                    = $user->mail;
        $form['basic_cart_checkout_email'] = array(
            '#type' => 'textfield',
            '#title' => t('Email'),
            '#required' => TRUE,
            '#default_value' => $email,

            // Perform ajax validation.
            '#ajax' => array(
                'callback' => '_basic_cart_form_email_ajax_validate',
                'wrapper' => 'email-error',
                'effect' => 'slide'
            ),
            '#suffix' => '<div id="email-error" style="color:#ff0000;"> </div>'
        );
    }

    $testvariable = config_get('basic_cart.settings', 'basic_cart_name');
    if ($testvariable > 0) {
        $yourname                    = isset($form_state['values']['basic_cart_checkout_name']) ? $form_state['values']['basic_cart_checkout_name'] : '';
        $form['basic_cart_checkout_name'] = array(
            '#type' => 'textfield',
            '#title' => t('Your Name'),
            '#required' => TRUE,
            '#size' => 10,
            '#maxlength' => 40,
            '#default_value' => $yourname
        );
    }

    $testvariable = config_get('basic_cart.settings', 'basic_cart_phone');
    if ($testvariable > 0) {
        $phone                    = isset($form_state['values']['basic_cart_checkout_phone']) ? $form_state['values']['basic_cart_checkout_phone'] : '';
        $form['basic_cart_checkout_phone'] = array(
            '#type' => 'textfield',
            '#title' => t('Phone'),
            '#required' => TRUE,
            '#size' => 12,
            '#maxlength' => 20,
            '#default_value' => $phone
        );
    }

    $testvariable = config_get('basic_cart.settings', 'basic_cart_address');
    if ($testvariable > 0) {
        $address                    = isset($form_state['values']['basic_cart_checkout_address']) ? $form_state['values']['basic_cart_checkout_address'] : '';
        $form['basic_cart_checkout_address'] = array(
            '#type' => 'textfield',
            '#title' => t('Address Line 1'),
            '#required' => TRUE,
            '#size' => 12,
            '#maxlength' => 50,
            '#default_value' => $address
        );
    }

    $testvariable = config_get('basic_cart.settings', 'basic_cart_address2');
    if ($testvariable > 0) {
        $address2                    = isset($form_state['values']['basic_cart_checkout_address2']) ? $form_state['values']['basic_cart_checkout_address2'] : '';
        $form['basic_cart_checkout_address2'] = array(
            '#type' => 'textfield',
            '#title' => t('Address Line 2'),
            '#size' => 12,
            '#maxlength' => 30,
            '#default_value' => $address2
        );
    }

    $testvariable = config_get('basic_cart.settings', 'basic_cart_city');
    if ($testvariable > 0) {
        $city                    = isset($form_state['values']['basic_cart_checkout_city']) ? $form_state['values']['basic_cart_checkout_city'] : '';
        $form['basic_cart_checkout_city'] = array(
            '#type' => 'textfield',
            '#title' => t('City or Town'),
            '#required' => TRUE,
            '#size' => 12,
            '#maxlength' => 30,
            '#default_value' => $city
        );
    }

    $testvariable = config_get('basic_cart.settings', 'basic_cart_county');
    if ($testvariable > 0) {
        $county                    = isset($form_state['values']['basic_cart_checkout_county']) ? $form_state['values']['basic_cart_checkout_county'] : '';
        $form['basic_cart_checkout_county'] = array(
            '#type' => 'textfield',
            '#title' => t('County'),
            '#required' => TRUE,
            '#size' => 12,
            '#maxlength' => 30,
            '#default_value' => $county
        );
    }

    $testvariable = config_get('basic_cart.settings', 'basic_cart_state');
    if ($testvariable > 0) {
        $state                    = isset($form_state['values']['basic_cart_checkout_state']) ? $form_state['values']['basic_cart_checkout_state'] : '';
        $form['basic_cart_checkout_state'] = array(
            '#type' => 'textfield',
            '#title' => t('State'),
            '#required' => TRUE,
            '#size' => 3,
            '#maxlength' => 3,
            '#default_value' => $state
        );
    }


    $testvariable = config_get('basic_cart.settings', 'basic_cart_zipcode');
    if ($testvariable > 0) {
        $zip                    = isset($form_state['values']['basic_cart_checkout_zipcode']) ? $form_state['values']['basic_cart_checkout_zipcode'] : '';
        $form['basic_cart_checkout_zipcode'] = array(
            '#type' => 'textfield',
            '#title' => t('Zip'),
            '#required' => TRUE,
            '#size' => 10,
            '#maxlength' => 10,
            '#default_value' => $zip
        );
    }
    
    $testvariable = config_get('basic_cart.settings', 'basic_cart_country');
    if ($testvariable > 0) {
        $country                    = isset($form_state['values']['basic_cart_checkout_country']) ? $form_state['values']['basic_cart_checkout_country'] : '';
        $form['basic_cart_checkout_country'] = array(
            '#type' => 'textfield',
            '#title' => t('Country'),
            '#required' => TRUE,
            '#size' => 10,
            '#maxlength' => 20,
            '#default_value' => $country
        );
    }
    
    $testvariable = config_get('basic_cart.settings', 'basic_cart_comment');
    if ($testvariable > 0) {
        $comment                    = isset($form_state['values']['basic_cart_checkout_comment']) ? $form_state['values']['basic_cart_checkout_comment'] : '';
        $form['basic_cart_checkout_comment'] = array(
            '#type' => 'textarea',
            '#description' => t('If you wish to add a comment about your order, please use this message area.'),
            '#required' => TRUE,
            '#default_value' => $comment
        );
    }

  $form['basic_cart_checkout_submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit order'),
  );

  return $form;
}

/**
 * Checkout form validation.
 */
function basic_cart_checkout_form_validate($form, &$form_state) { 
  if (!valid_email_address($form_state['values']['basic_cart_checkout_email'])) {
    form_set_error('basic_cart_checkout_email', t('Please enter a valid email address.'));
  }
}

/**
 * Checkout form submit proccess.
 * Register order and send emails.
 */
function basic_cart_checkout_form_submit($form, &$form_state) {
  $order_details = '';
  $cart = basic_cart_get_cart();
  $price = basic_cart_get_total_price();
  $total_price = basic_cart_price_format($price->total);
  
  $vat_is_enabled = (int) config_get('basic_cart.settings', 'basic_cart_vat_state');
  if (!empty ($vat_is_enabled) && $vat_is_enabled) {
    $vat = basic_cart_price_format($price->vat);
  }
  
  // Registering the new order to the database.
  if (module_exists('basic_cart_order')) {
    $name = $form_state['values']['basic_cart_checkout_name'];
    $email = $form_state['values']['basic_cart_checkout_email'];
    $order_details_ = array(
      'phone' => $form_state['values']['basic_cart_checkout_phone'],
      'address' => $form_state['values']['basic_cart_checkout_address'],
      'address2' => $form_state['values']['basic_cart_checkout_address2'],
      'city' => $form_state['values']['basic_cart_checkout_city'],
      'county' => $form_state['values']['basic_cart_checkout_county'],
      'state' => $form_state['values']['basic_cart_checkout_state'],
      'country' => $form_state['values']['basic_cart_checkout_country'],
      'zipcode' => $form_state['values']['basic_cart_checkout_zipcode'],
      'comment' => $form_state['values']['basic_cart_checkout_comment'],
    );
    $order = basic_cart_order_register_order($name, $email, $order_details_);
    
    // Order successfully placed. Custom hook for other modules.
    foreach (module_implements('basic_cart_order') as $module) {
      $function = $module . '_basic_cart_order';
      // Will call all modules implementing hook_basic_cart_order() with the order node as argument.
      $function($order);
    }
  }
  
  // Building the order details.
  $i = 0;
foreach ($cart as $cid => $value) {
if (is_numeric($cid))
{
$node = node_load($cart[$value]);
$unit_price = basic_cart_price_format($node->price["und"][0]["value"]);
$order_details .= ++$i . '. ' . $node->title . "\t" . $_SESSION['basic_cart']['cart'][$cid . 'quantity'] . ' x ' . $unit_price . "\n";
}
}
  $order_details .= "\n";
  $order_details .= "Total price: " . $total_price;
  $vat_is_enabled = (int) config_get('basic_cart.settings', 'basic_cart_vat_state');
  if ($vat_is_enabled) {
    $order_details .= "\n";
    $order_details .= "Total VAT: " . $vat;
  }
  // Pleaceholder replacement.
  $search = array(
    '%CUSTOMER_NAME', '%CUSTOMER_EMAIL', '%CUSTOMER_PHONE', '%CUSTOMER_ADDRESS', '%CUSTOMER_ADDRESS2', '%CUSTOMER_CITY', '%CUSTOMER_STATE',
    '%CUSTOMER_ZIPCODE', '%CUSTOMER_COUNTY', '%CUSTOMER_COUNTRY', '%CUSTOMER_COMMENT', '%ORDER_DETAILS',
  );
  $replace = array(
    $form_state['values']['basic_cart_checkout_name'], $form_state['values']['basic_cart_checkout_email'], $form_state['values']['basic_cart_checkout_phone'],
    $form_state['values']['basic_cart_checkout_address'], $form_state['values']['basic_cart_checkout_address2'],
    $form_state['values']['basic_cart_checkout_city'], $form_state['values']['basic_cart_checkout_state'], $form_state['values']['basic_cart_checkout_zipcode'],
    $form_state['values']['basic_cart_checkout_county'], $form_state['values']['basic_cart_checkout_country'],
    $form_state['values']['basic_cart_checkout_comment'], $order_details,
  );
  // Admin message.
  $message_html = config_get('basic_cart.settings', 'basic_cart_admin_message');
  $message_html = str_replace($search, $replace, $message_html);
  
  // Admin mail.
  $params['admin_message'] = $message_html;
  $site_mail = config_get('system.core', 'site_mail');
  $admin_emails = config_get('basic_cart.settings', 'basic_cart_admin_emails');
  if (empty ($admin_emails)) {
    // Sending mail to admin.
    $message = backdrop_mail('basic_cart', 'admin_mail', $site_mail, language_default(), $params);

    $mails_sent = 0;
    if ($message['result']) {
      $mails_sent++;
    }
  }
  else {
    $admin_emails = explode("\n", $admin_emails);
    if (is_array($admin_emails) && !empty($admin_emails)) {
      $ok = FALSE;
      foreach ($admin_emails as $admin_email) {
        // Sending mail to each admin.
        $message = backdrop_mail('basic_cart', 'admin_mail', $admin_email, language_default(), $params);
        
        // Verifing that the mail was sent for at least one email address.
        if ($message['result']) {
          $ok = TRUE;
        }
      }
      
      $mails_sent = 0;
      if ($ok) {
        $mails_sent++;
      }
    }
  }

  
  // User email.
  $send_user_mail = config_get('basic_cart.settings', 'basic_cart_send_user_message');
  if ($send_user_mail) {
    $message_html = config_get('basic_cart.settings', 'basic_cart_user_message');
    $message_html = str_replace($search, $replace, $message_html);
    $params['user_message'] = $message_html;
    
    // Sending mail.
    $message = backdrop_mail('basic_cart', 'user_mail', $form_state['values']['basic_cart_checkout_email'], language_default(), $params);

    if ($message['result']) {
      $mails_sent++;
    }
  }
  
  if ($mails_sent >= 1) {
    basic_cart_empty_cart();
    backdrop_goto('checkout/thank-you');
  }
  else {
    backdrop_set_message(t('There was a problem in submitting your order. Please try again later.'), 'error');
  }
}


/**
 * Implements hook_mail().
 */
function basic_cart_mail($key, &$message, $params) {
  switch ($key) {
    case 'admin_mail':
      $message['subject'] = check_plain(config_get('basic_cart.settings', 'basic_cart_admin_subject'));
      $message['body'][] = filter_xss($params['admin_message']);
      break;

    case 'user_mail':
      $message['subject'] = check_plain(config_get('basic_cart.settings', 'basic_cart_user_subject'));
      $message['body'][] = filter_xss($params['user_message']);
      break;
  }
}

/**
 * Callback for thank you page.
 */
function basic_cart_checkout_thank_you() {
  $title = config_get('basic_cart.settings', 'basic_cart_thank_you_title');
  backdrop_set_title($title);
  $message = config_get('basic_cart.settings', 'basic_cart_thank_you_message');
  return filter_xss($message);
}

