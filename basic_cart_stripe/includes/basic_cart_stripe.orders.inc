<?php
/**
 * @file
 * Order management for Basic Cart Stripe
 */

/**
 * Orders listing page
 */
function basic_cart_stripe_orders_page() {
  $output = '';

  // Table header.
  $header = array(
    array('data' => t('ID'), 'field' => 'order_id', 'sort' => 'desc'),
    array('data' => t('Date'), 'field' => 'created'),
    array('data' => t('Customer'), 'field' => 'name'),
    array('data' => t('Email'), 'field' => 'email'),
    array('data' => t('Total'), 'field' => 'total'),
    array('data' => t('Status'), 'field' => 'payment_status'),
    array('data' => t('Payment Intent')),
    array('data' => t('Actions')),
  );

  // Query with pagination and sorting.
  $query = db_select('basic_cart_stripe_orders', 'o');
  $query = $query->extend('PagerDefault');
  $query = $query->extend('TableSort');
  $query->fields('o');
  $query = $query->limit(50);
  $query = $query->orderByHeader($header);

  $result = $query->execute();

  // Rows.
  $rows = array();
  foreach ($result as $order) {
    $status_class = $order->payment_status == 'succeeded' ? 'success' : 'warning';

    $rows[] = array(
      $order->order_id,
      format_date($order->created, 'short'),
      check_plain($order->name),
      check_plain($order->email),
      '€' . number_format($order->total, 2),
      '<span class="label label-' . $status_class . '">' . check_plain($order->payment_status) . '</span>',
      l(
        substr($order->payment_intent_id, 0, 20) . '...',
        'https://dashboard.stripe.com/payments/' . $order->payment_intent_id,
        array('attributes' => array(
          'target' => '_blank',
          'title' => $order->payment_intent_id),
        )
      ),
      l(t('View'), 'admin/store/orders/' . $order->order_id),
    );
  }

  if (empty($rows)) {
    $output .= '<p>' . t('No orders yet.') . '</p>';
  } else {
    $output .= theme('table', array(
      'header' => $header,
      'rows' => $rows,
      'attributes' => array('class' => array('basic-cart-stripe-orders')),
    ));
    $output .= theme('pager');
  }

  return $output;
}

/**
 * Order detail page
 */
function basic_cart_stripe_order_view($order_id) {
  $order = db_select('basic_cart_stripe_orders', 'o')
    ->fields('o')
    ->condition('order_id', $order_id)
    ->execute()
    ->fetchObject();

  if (!$order) {
    backdrop_not_found();
    return;
  }

  // Unserialize cart.
  $cart = unserialize($order->cart);

  $output = '';

  // Customer information.
  $output .= '<div class="order-customer">';
  $output .= '<h3>' . t('Customer information') . '</h3>';
  $output .= '<dl>';
  $output .= '<dt>' . t('Name') . '</dt><dd>' . check_plain($order->name) . '</dd>';
  $output .= '<dt>' . t('Email') . '</dt><dd>' . check_plain($order->email) . '</dd>';
  if (!empty($order->phone)) {
    $output .= '<dt>' . t('Phone') . '</dt><dd>' . check_plain($order->phone) . '</dd>';
  }
  if (!empty($order->message)) {
    $output .= '<dt>' . t('Message') . '</dt><dd>' . check_plain($order->message) . '</dd>';
  }
  $output .= '</dl>';
  $output .= '</div>';

  // Order information.
  $output .= '<div class="order-info">';
  $output .= '<h3>' . t('Order information') . '</h3>';
  $output .= '<dl>';
  $output .= '<dt>' . t('Order number') . '</dt><dd>#' . $order->order_id . '</dd>';
  $output .= '<dt>' . t('Date') . '</dt><dd>' . format_date($order->created, 'long') . '</dd>';
  $output .= '<dt>' . t('Total') . '</dt><dd>€' . number_format($order->total, 2) . '</dd>';

  // Payment status with color.
  $status_class = $order->payment_status == 'succeeded' ? 'success' : 'warning';
  $output .= '<dt>' . t('Payment status') . '</dt><dd><span class="label label-' . $status_class . '">' . check_plain($order->payment_status) . '</span></dd>';

  $output .= '<dt>' . t('Payment Intent ID') . '</dt><dd>' . l(
    $order->payment_intent_id,
    'https://dashboard.stripe.com/payments/' . $order->payment_intent_id,
    array('attributes' => array('target' => '_blank'))
  ) . '</dd>';
  $output .= '</dl>';
  $output .= '</div>';

  // Products.
  $output .= '<div class="order-items">';
  $output .= '<h3>' . t('Products') . '</h3>';

  $header = array(
    t('Product'),
    t('Unit price'),
    t('Quantity'),
    t('Subtotal'),
  );

  $rows = array();
  foreach ($cart as $key => $value) {
    // Skip quantity keys.
    if (is_string($key) && substr($key, -8) === 'quantity') {
      continue;
    }

    // If it's a numeric nid.
    if (is_numeric($key) && is_numeric($value) && $key == $value) {
      $nid = $value;

      // Load complete node.
      $node = node_load($nid);
      if (!$node) {
        continue;
      }

      // Get quantity.
      $quantity_key = $nid . 'quantity';
      $quantity = isset($cart[$quantity_key]) ? $cart[$quantity_key] : 1;

      // Get price.
      $price = basic_cart_stripe_get_node_price($node);
      $subtotal = $price * $quantity;

      $rows[] = array(
        check_plain($node->title),
        '€' . number_format($price, 2),
        $quantity,
        '€' . number_format($subtotal, 2),
      );
    }
  }

  // Total row.
  $rows[] = array(
    array(
      'data' => '<strong>' . t('Total') . '</strong>', 'colspan' => 3,
      'class' => array('text-right'),
    ),
    '<strong>€' . number_format($order->total, 2) . '</strong>',
  );

  $output .= theme('table', array(
    'header' => $header,
    'rows' => $rows,
  ));
  $output .= '</div>';

  // Actions.
  $output .= '<div class="order-actions">';
  $output .= l(t('← Back to orders'), 'admin/store/orders', array('attributes' => array('class' => array('button'))));

  // Only allow refunds if payment was successful.
  if ($order->payment_status == 'succeeded') {
    $output .= ' ' . l(
      t('Refund'),
      'admin/store/orders/' . $order->order_id . '/refund',
      array('attributes' => array('class' => array('button', 'button-danger')))
    );
  }
  $output .= '</div>';

  return $output;
}

/**
 * Refund form
 */
function basic_cart_stripe_refund_form($form, &$form_state, $order_id) {
  $order = db_select('basic_cart_stripe_orders', 'o')
    ->fields('o')
    ->condition('order_id', $order_id)
    ->execute()
    ->fetchObject();

  if (!$order || $order->payment_status != 'succeeded') {
    backdrop_set_message(t('This order cannot be refunded.'), 'error');
    backdrop_goto('admin/commerce/orders/' . $order_id);
  }

  $form['order_id'] = array(
    '#type' => 'value',
    '#value' => $order_id,
  );

  $form['payment_intent_id'] = array(
    '#type' => 'value',
    '#value' => $order->payment_intent_id,
  );

  $form['info'] = array(
    '#markup' => '<p>' . t('You are about to refund order #@order for an amount of €@total', array(
      '@order' => $order_id,
      '@total' => number_format($order->total, 2),
    )) . '</p>',
  );

  $form['amount'] = array(
    '#type' => 'textfield',
    '#title' => t('Amount to refund'),
    '#default_value' => number_format($order->total, 2),
    '#size' => 10,
    '#required' => TRUE,
    '#description' => t('Amount in euros. Leave the default value to refund the total.'),
  );

  $form['reason'] = array(
    '#type' => 'textarea',
    '#title' => t('Refund reason'),
    '#rows' => 3,
    '#description' => t('This reason will be visible in the Stripe dashboard.'),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Refund'),
    '#attributes' => array('class' => array('button-danger')),
  );
  $form['actions']['cancel'] = array(
    '#markup' => l(t('Cancel'), 'admin/store/orders/' . $order_id, array('attributes' => array('class' => array('button')))),
  );

  return $form;
}

/**
 * Refund form validation
 */
function basic_cart_stripe_refund_form_validate($form, &$form_state) {
  $amount = str_replace(',', '.', $form_state['values']['amount']);

  if (!is_numeric($amount) || $amount <= 0) {
    form_set_error('amount', t('Amount must be a positive number.'));
  }
}

/**
 * Refund form submit
 */
function basic_cart_stripe_refund_form_submit($form, &$form_state) {
  $order_id = $form_state['values']['order_id'];
  $payment_intent_id = $form_state['values']['payment_intent_id'];
  $amount = str_replace(',', '.', $form_state['values']['amount']);
  $reason = $form_state['values']['reason'];

  try {
    // Convert to cents.
    $amount_cents = round($amount * 100);

    // Call Stripe API to process refund using stripe_api_call()
    $refund = stripe_api_call('refund', 'create', [
      'payment_intent' => $payment_intent_id,
      'amount' => $amount_cents,
      'reason' => 'requested_by_customer',
      'metadata' => [
        'order_id' => $order_id,
        'reason' => $reason,
      ],
    ]);

    if (!$refund) {
      throw new Exception('Could not create refund');
    }

    // Update order status.
    db_update('basic_cart_stripe_orders')
      ->fields(array('payment_status' => 'refunded'))
      ->condition('order_id', $order_id)
      ->execute();

    backdrop_set_message(t('Refund processed successfully. ID: @id', array('@id' => $refund->id)));
    watchdog('basic_cart_stripe', 'Refund processed for order @order: @refund', array(
      '@order' => $order_id,
      '@refund' => $refund->id,
    ));
  } catch (Exception $e) {
    backdrop_set_message(t('Error processing refund: @error', array('@error' => $e->getMessage())), 'error');
    watchdog('basic_cart_stripe', 'Error refunding order @order: @error', array(
      '@order' => $order_id,
      '@error' => $e->getMessage(),
    ), WATCHDOG_ERROR);
  }

  $form_state['redirect'] = 'admin/store/orders/' . $order_id;
}
