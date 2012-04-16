<?php
/**
 * @file
 * basic_cart_order_details.tpl.php
 */

?>
<p><?php print l(t('&laquo; Back to order list'), 'admin/structure/orders', array('html' => TRUE)); ?></p>
<table class="basic-cart-order-details" border="0" cellpadding="0" cellspacing="0">
  <tr>
    <td class="basic-cart-order-label"><?php print t('Name'); ?>:</td>
    <td class="basic-cart-order-value"><?php print $name; ?></td>
  </tr>
  <tr>
    <td class="basic-cart-order-label"><?php print t('Email'); ?>:</td>
    <td class="basic-cart-order-value"><?php print $email; ?></td>
  </tr>
  <tr>
    <td class="basic-cart-order-label"><?php print t('Phone'); ?>:</td>
    <td class="basic-cart-order-value"><?php print $phone; ?></td>
  </tr>
  <tr>
    <td class="basic-cart-order-label"><?php print t('Address'); ?>:</td>
    <td class="basic-cart-order-value"><?php print $address; ?></td>
  </tr>
  <tr>
    <td class="basic-cart-order-label"><?php print t('Date'); ?>:</td>
    <td class="basic-cart-order-value"><?php print date('Y-m-d G:i', $timestamp); ?></td>
  </tr>
  <tr>
    <td class="basic-cart-order-label" colspan="2"><?php print t('Message'); ?>:</td>
  </tr>
  <tr>
    <td class="basic-cart-order-value" colspan="2"><?php print $message; ?></td>
  </tr>
  <tr>
    <td class="basic-cart-order-label" colspan="2"><?php print t('Products'); ?>:</td>
  </tr>
  <?php foreach ($products as $product): ?>
  <tr>
    <td class="basic-cart-order-value" colspan="2">
      <?php print l($product->title, 'node/' . $product->nid); ?>
      <span class="basic-cart-order-product-price">
        <?php print $product->basic_cart_quantity; ?> x <strong><?php print $product->price; ?> <?php print $currency; ?></strong>
      </span>
    </td>
  </tr>
  <?php endforeach; ?>
  <tr>
    <td colspan="2" class="basic-cart-order-label">
      <span class="basic-cart-order-total-price">
        <?php print t('Total'); ?>: <?php print $total_price; ?> <?php print $currency; ?>
      </span>
    </td>
  </tr>
</table>
