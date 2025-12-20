<?php
/**
 * @file
 * Basic Cart Stripe administration page
 */

/**
 * Configuration form
 */
function basic_cart_stripe_settings_form($form, &$form_state) {
  $config = config('basic_cart_stripe.settings');

  $form['#config'] = 'basic_cart_stripe.settings';

  $form['info'] = array(
    '#markup' => '<p>' . t('This module uses the API key configuration from the Stripe module. ') .
    l(t('Configure the keys here'), 'admin/config/services/stripe') . '</p>',
  );

  // Verify if Stripe module is configured.
  $stripe_config = config('stripe.settings');
  $stripe_secret_config = config('stripe_secret.settings');
  $stripe_mode = $stripe_config->get('mode');

  $is_configured = FALSE;

  if ($stripe_mode == 'test') {
    $test_secret = $stripe_secret_config->get('test_secret_key');
    $test_public = $stripe_config->get('test_public_key');
    if (!empty($test_secret) && !empty($test_public)) {
      $is_configured = TRUE;
      backdrop_set_message(t('Stripe is configured in test mode.'), 'status');
    }
  } elseif ($stripe_mode == 'live') {
    $live_secret = $stripe_secret_config->get('live_secret_key');
    $live_public = $stripe_config->get('live_public_key');
    if (!empty($live_secret) && !empty($live_public)) {
      $is_configured = TRUE;
      backdrop_set_message(t('Stripe is configured in live mode.'), 'status');
    }
  }

  if (!$is_configured) {
    backdrop_set_message(t('The Stripe module is not configured. Please !link first.', array(
      '!link' => l(t('configure the API keys'), 'admin/config/services/stripe'),
    )), 'warning');
  }

  $form['webhook'] = array(
    '#type' => 'fieldset',
    '#title' => t('Stripe Webhook'),
    '#description' => t('Basic Cart Stripe uses the webhook system from the Stripe module. No need to configure a specific webhook for this module.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['webhook']['webhook_info'] = array(
    '#markup' => '<div class="messages info">' .
    t('This module listens to Stripe events through the webhook configured in the Stripe module. ') .
    t('Make sure the webhook is configured at !stripe_config and listens to the following events:', array(
      '!stripe_config' => l(t('admin/config/services/stripe'), 'admin/config/services/stripe'),
    )) .
    '<ul>' .
    '<li><code>payment_intent.succeeded</code></li>' .
    '<li><code>payment_intent.payment_failed</code></li>' .
    '</ul>' .
    '</div>',
  );

  $form['settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('General settings'),
  );

  // We use currency settings from Stripe module.
  $config_stripe = config('stripe.settings');
  $form['settings']['currency'] = array(
    '#type' => 'textfield',
    '#title' => t('Currency'),
    '#size' => 10,
    '#default_value' => $config_stripe->get('currency'),
    '#description' => t('Default currency for payments. Using currency settings from the !stripe module.', array('!stripe' => l('Stripe', 'admin/config/services/stripe/settings'))),
    '#disabled' => TRUE,
  );

  $form['orders'] = array(
    '#type' => 'fieldset',
    '#title' => t('Orders'),
  );

  $form['orders']['view_orders'] = array(
    '#markup' => '<p>' . l(t('View all orders'), 'admin/store/orders') . '</p>',
  );

  return system_settings_form($form);
}

/**
 * Configuration form validation
 */
function basic_cart_stripe_settings_form_validate($form, &$form_state) {
  // Verify that Stripe is configured.
  $stripe_config = config('stripe.settings');
  $stripe_secret_config = config('stripe_secret.settings');
  $stripe_mode = $stripe_config->get('mode');

  $is_configured = FALSE;

  if ($stripe_mode == 'test') {
    $test_secret = $stripe_secret_config->get('test_secret_key');
    $test_public = $stripe_config->get('test_public_key');
    if (!empty($test_secret) && !empty($test_public)) {
      $is_configured = TRUE;
    }
  } elseif ($stripe_mode == 'live') {
    $live_secret = $stripe_secret_config->get('live_secret_key');
    $live_public = $stripe_config->get('live_public_key');
    if (!empty($live_secret) && !empty($live_public)) {
      $is_configured = TRUE;
    }
  }

  if (!$is_configured) {
    form_set_error('', t('The Stripe module is not configured correctly. Configure the API keys first at !link.', array(
      '!link' => l(t('admin/config/services/stripe'), 'admin/config/services/stripe'),
    )));
  }
}
