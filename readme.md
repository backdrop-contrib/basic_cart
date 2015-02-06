BASIC CART
===========

CONTENTS OF THIS FILE
---------------------

 - Introduction
 - Requirements
 - Installation
 - Permissions
 - Usage
 - Sponsors

INTRODUCTION
------------

Basic cart is a very simple shopping cart / checkout process for Backdrop,
that just sends 2 emails after each order. Along with the shopping cart,
this module also provides a block where you can view your cart's content.

You must add your own payment processor or service.

It's ideal for small websites with only a few products or other content types
being sold, for example touristic offers.

Main difference from Ubercart and Commerce

The main difference from Ubercart and Commerce is the possibility of choosing
the content types that can be added to the cart. It also doesn't bother you
with the Product SKU, that can be a pain when you have just a few products.

Features

* The possibility of choosing the content types that can be added to the cart.
* The possibility of sending, or not, an email to the customer once an order is placed.
* Custom email messages for both the site administrator and the customer, along with the order details.
* A block with the contents of your shopping cart.

TESTED
-----

Does not work yet on Backdrop 1.0


KNOWN ISSUES
---------------------

Fatal error: Call to undefined function module_implements() in /Applications/MAMP/htdocs/backdrop/core/includes/bootstrap.inc on line 3467
This happens when you add a product to the cart.


REQUIREMENTS
------------

@todo

INSTALLATION
------------

@todo


PERMISSIONS
------------

@todo

USAGE
-----

@todo

Install the module.
If the goods you want to sell don't exist on your website, create a new content type for them. If you already have a content type for them you can skip this step.
Select the content type/s on the module configuration page: Configuration - Basic Cart or admin/config/basic_cart/settings.
Create a view for the content type/s to get the shop's overview. If you already have it, you can skip this step.
You have a block with the cart preview. Activate it ;-)
Start to add you're products in the newly created content type or in the one you already have.

License
-------

This project is GPL v2 software. See the LICENSE.txt file in this directory for
complete text.

Maintainers
-----------

- biolithic <https://github.com/biolithic>

Current Maintainers on Drupal:

 - dicix <https://www.drupal.org/u/dicix>

Ported to Backdrop by:

 - biolithic <https://github.com/biolithic>
