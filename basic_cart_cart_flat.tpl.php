<?php
/**
 * @file
 * Basic cart shopping cart html template
 */

?>

<?php if (empty($cart)): ?>
  <p><?php print t('Your shopping cart is empty.'); ?></p>
<?php else: ?>
  <div class="basic-cart-cart basic-cart-grid">
    <?php if(is_array($cart) && count($cart) >= 1): ?>
      <?php foreach($cart as $nid => $node): ?>
        <div class="basic-cart-cart-contents row">
            <div class="basic-cart-cart-quantity cell">              
              <div class="cell"><?php print $node->basic_cart_quantity; ?></div>
              <div class="cell basic-cart-cart-x">x</div>
            </div>
            <div class="basic-cart-cart-node-title cell">
              <?php print l($node->title, 'node/' . $node->nid); ?><br />
              <span class="basic-cart-cart-node-summary">
                <?php if (isset($node->basic_cart_node_description)): ?>
                  <?php if(drupal_strlen($node->basic_cart_node_description) > 50): ?>
                    <?php print truncate_utf8($node->basic_cart_node_description, 50); ?> ...
                  <?php else: ?>
                    <?php print $node->basic_cart_node_description; ?>
                  <?php endif; ?>
                <?php endif; ?>
              </span>
            </div>
        </div>
      <?php endforeach; ?>
    <?php endif; ?>
  </div>
<?php endif; ?>
