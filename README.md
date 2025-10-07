# BASIC CART

Basic cart is a very simple shopping cart/checkout process for Backdrop, that
just sends 2 emails after each order. Along with the shopping cart, this module
also provides a block where you can view your cart's content.

This module includes a payment processor sub module for Stripe.

It's ideal for small websites with only a few products or other content types
being sold, for example touristic offers. It is much simpler to get started for
simple or few products.

Features

* The possibility of choosing the content types that can be added to the cart.
* The possibility of sending, or not, an email to the customer once an order is
  placed.
* Custom email messages for both the site administrator and the customer, along
  with the order details.
* A block with the contents of your shopping cart.
* Payment process with Stripe.

## KNOWN ISSUES

NOTE: You must create your own Views using the content types this module
provides to have a record-keeping backend.

## SPECIAL THANKS

* [dicix](https://www.drupal.org/u/dicix)
* [Jen Lampton](https://www.drupal.org/u/jenlampton)

## INSTALLATION


Install this module using the official Backdrop CMS instructions at
https://backdropcms.org/guide/modules

## COMING FROM DRUPAL?

Main difference from Ubercart and Commerce

The main difference from Ubercart and Commerce is the possibility of choosing
the content types that can be added to the cart. It also doesn't bother you with
the Product SKU, that can be a pain when you have just a few products.

Basic Cart for Drupal stored node objects in the user `$_SESSION` variable and
used pre-made Views, Features, etc... The Backdrop port failed with this error
because of it:

> Fatal error: Call to undefined function module_implements() in
> /core/includes/bootstrap.inc on line 3467

This happens when you add a product to the cart.

Basic Cart for Backdrop stores node id's as strings in the user `$_SESSION`
variable and no pre-made etc...

## USAGE

Install the module.

If the goods you want to sell don't exist on your website, create a new content
type for them. If you already have a content type for them you can skip this
step.

Select the content type/s on the module configuration page: Configuration ->
Basic Cart or `admin/config/basic_cart/settings`.

Create a view for the content type/s to get the shop's overview. If you already
have it, you can skip this step.

You have a block with the cart preview. Activate it and place it in your
layouts ;-)

Start to add your products in the newly created content type or in the one you
already have.

## Issues

Bugs and feature requests should be reported in [the Issue Queue](https://github.com/backdrop-contrib/basic-cart/issues).

## Current Maintainers

* [Robert Garrigós](https://github.com/robertgarrigos).
* Collaborators and co-maintainers welcome!

## Credits

* Ported to Backdrop CMS by [biolithic](https://github.com/biolithic).
* Originally written for Drupal by [Alex Dicianu](https://www.drupal.org/user/664900).

## License

This project is GPL v2 software.
See the LICENSE.txt file in this directory for complete text.
