BASIC CART
===================

Basic cart is a very simple shopping cart/checkout process for Backdrop, that
just sends 2 emails after each order. Along with the shopping cart, this module
also provides a block where you can view your cart's content.

You must add your own payment processor or service if you wish to accept
payments on site using this module. However, a module entitled, "Basic Cart
Plus" is in development for 1.4/2016 which contains the payment processors built
in if you wish to have that. Thus, you will have two e-commerce module options
-- this module for a "pay in person on delivery" type of experience, and that
module which is a "pay in site for a ticket or content subscription" type of
experience. This still leaves room for a Drupal Commerce/Magento type of module
to be developed by others.

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

CONTENTS OF THIS FILE
---------------------

- Introduction
- Tested
- Known Issues
- Special Thanks
- Requirements
- Installation
- Coming From Drupal?
- Usage
- License
- Credits
- Maintainers

TESTED
-----

Working in Backdrop 1.1

KNOWN ISSUES
---------------------

This module is not supported in 2016 by @biolithic , the module port person.

Contact another developer on this list for support, or use at your own
discretion.

Some supported (but in development) options in 2016 are
https://github.com/codewombat/backshop and
https://github.com/biolithic/basic_cart_plus

NOTE: You must create your own Views using the content types this module
provides to have a record-keeping backend.

SPECIAL THANKS
--------------

- [dicix](https://www.drupal.org/u/dicix)
- [Jen Lampton](https://www.drupal.org/u/jenlampton)

REQUIREMENTS
------------

none

INSTALLATION
------------

Install this module using the official Backdrop CMS instructions at
https://backdropcms.org/guide/modules


COMING FROM DRUPAL?
-------------------

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

PERMISSIONS
------------

@todo


USAGE
-----

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

LICENSE
-------

This project is GPL v2 software. See the LICENSE.txt file in this directory for
complete text.

CREDITS
-----------

This module is based on the Basic Cart module for Drupal, originally written and
maintained by a large number of contributors, including:

- [dicix](https://www.drupal.org/u/dicix)

MAINTAINERS
-----------

- [Robert Garrigós](https://github.com/robertgarrigos)

Ported to Backdrop by:

- [biolithic](https://github.com/biolithic)
