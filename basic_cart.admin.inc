<?php

/**
 * @file
 * Basic cart admin settings forms.
 */

/**
 * Callback for the admin configuration page.
 */
function basic_cart_admin_content_type() {
  $node_types = node_type_get_types();
  if (empty($node_types)) {
    return NULL;
  }

  $options = array();
  foreach ($node_types as $node_type => $type) {
    if ($node_type == 'order' && module_exists('basic_cart_order')) {
      continue;
    }
    $options[$node_type] = check_plain($type->name);
  }

  $form['content_type'] = array(
    '#title' => t('Content type selection'),
    '#type' => 'fieldset',
    '#description' => t('Please select the content types for which you wish to have the "Add to cart" option.'),
  );

  $form['content_type']['basic_cart_content_types'] = array(
    '#title' => t('Content types'),
    '#type' => 'checkboxes',
    '#options' => $options,
    '#default_value' => variable_get('basic_cart_content_types', array()),
  );

  $form['currency'] = array(
    '#title' => t('Currency'),
    '#type' => 'fieldset',
    '#description' => t('Please select the currency in which the prices will be calculated.'),
  );

  $form['currency']['basic_cart_currency'] = array(
    '#title' => t('Currency'),
    '#type' => 'textfield',
    '#description' => t("Please choose the currency."),
    '#default_value' => variable_get('basic_cart_currency'),
  );

  $form['redirect'] = array(
    '#title' => t('Redirect user after adding an item to the shopping cart'),
    '#type' => 'fieldset',
    //'#description' => t('Please select the redirect option below.'),
  );

  $form['redirect']['basic_cart_redirect_user_after_add_to_cart'] = array(
    '#title' => t('Redirect the user to the shopping cart page after he clicked the Add to cart button.'),
    '#type' => 'checkbox',
    //'#description' => t('Check if you want to '),
    '#default_value' => variable_get('basic_cart_redirect_user_after_add_to_cart'),
  );

  $form['messages'] = array(
    '#title' => t('Email messages'),
    '#type' => 'fieldset',
    '#description' => t('Here you can customize the mails sent to the site administrator and customer, after an order is placed.'),
  );

  $form['messages']['basic_cart_admin_subject'] = array(
    '#title' => t('Subject'),
    '#type' => 'textfield',
    '#description' => t("Subject field for the administrator's email."),
    '#default_value' => variable_get('basic_cart_admin_subject'),
  );

  $form['messages']['basic_cart_admin_message'] = array(
    '#title' => t('Admin email'),
    '#type' => 'textarea',
    '#description' => t('This email will be sent to the site administrator just after an order is placed. Availabale tokes: %CUSTOMER_NAME, %CUSTOMER_EMAIL, %CUSTOMER_PHONE, %CUSTOMER_ADDRESS, %CUSTOMER_MESSAGE, %ORDER_DETAILS'),
    '#default_value' => variable_get('basic_cart_admin_message'),
  );

  $form['messages']['basic_cart_send_user_message'] = array(
    '#title' => t('Send an email to the customer after an order is placed'),
    '#type' => 'checkbox',
    '#default_value' => variable_get('basic_cart_send_user_message'),
  );

  $form['messages']['basic_cart_user_subject'] = array(
    '#title' => t('Subject'),
    '#type' => 'textfield',
    '#description' => t("Subject field for the user's email."),
    '#default_value' => variable_get('basic_cart_user_subject'),
  );

  $form['messages']['basic_cart_user_message'] = array(
    '#title' => t('User email'),
    '#type' => 'textarea',
    '#description' => t('This email will be sent to the user just after an order is placed. Availabale tokes: %CUSTOMER_NAME, %CUSTOMER_EMAIL, %CUSTOMER_PHONE, %CUSTOMER_ADDRESS, %CUSTOMER_MESSAGE, %ORDER_DETAILS'),
    '#default_value' => variable_get('basic_cart_user_message'),
  );

  $form['thank_you'] = array(
    '#title' => t('Thank you page'),
    '#type' => 'fieldset',
    '#description' => t('Thank you page customization.'),
  );

  $form['thank_you']['basic_cart_thank_you_title'] = array(
    '#title' => t('Title'),
    '#type' => 'textfield',
    '#description' => t('Thank you page title.'),
    '#default_value' => variable_get('basic_cart_thank_you_title'),
  );

  $form['thank_you']['basic_cart_thank_you_message'] = array(
    '#title' => t('Text'),
    '#type' => 'textarea',
    '#description' => t('Thank you page text.'),
    '#default_value' => variable_get('basic_cart_thank_you_message'),
  );

  $form['actions']['#type'] = 'actions';
  $form['actions']['submit'] = array('#type' => 'submit', '#value' => t('Save configuration'));

  return $form;
}

/**
 * Callback for the admin configuration page.
 */
function basic_cart_admin_content_type_submit($form, &$form_state) {
  // Remove internal Form API values.
  unset($form_state['values']['form_id'], $form_state['values']['form_token'], $form_state['values']['form_build_id'], $form_state['values']['op']);
  // Save variables.
  foreach ($form_state['values'] as $key => $value) {
    if (is_array($value) && isset($form_state['values']['array_filter'])) {
      $value = array_keys(array_filter($value));
    }
    variable_set($key, $value);
  }

  // Setting up the price field for the selected content types.
  foreach ($form_state['values']['basic_cart_content_types'] as $type => $checked) {
    // If a node type is checked, then create the price field.
    if ($checked) {
      // Foreach checked content type, we must assign the price field to the content type.
      $instance = field_info_instance('node', 'price', $type);
      if (empty($instance)) {
        $instance = array(
          'field_name' => 'price',
          'label' => t('Price'),
          'description' => t('Please enter the price for this item.'),
          'entity_type' => 'node',
          'bundle' => $type,
        );
        // It doesn't exist. Create it.
        field_create_instance($instance);
      }
    }
    // If not, then delete the price field.
    else {
      $instance = field_info_instance('node', 'price', $type);
      if (!empty($instance)) {
        field_delete_instance($instance);
      }
    }
  }
  drupal_set_message(t('The configuration options have been saved.'));
}
