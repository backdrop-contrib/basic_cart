(function ($) {
  'use strict';

  Backdrop.behaviors.basicCartStripe = {
    attach: function (context, settings) {
      // Only execute once
      var cardElement = $('#card-element', context);
      if (cardElement.once('basic-cart-stripe').length === 0) {
        return;
      }

      // Get configuration
      var config = settings.basic_cart_stripe;
      if (!config || !config.publishable_key) {
        console.error('Stripe configuration not available');
        return;
      }

      console.log('Initializing Stripe with publishable key');

      // Initialize Stripe
      var stripe = Stripe(config.publishable_key);
      var elements = stripe.elements();

      // Styles for Stripe Elements
      var style = {
        base: {
          color: '#32325d',
          fontFamily: '"Helvetica Neue", Helvetica, sans-serif',
          fontSmoothing: 'antialiased',
          fontSize: '16px',
          '::placeholder': {
            color: '#aab7c4'
          }
        },
        invalid: {
          color: '#fa755a',
          iconColor: '#fa755a'
        }
      };

      // Create card element
      var card = elements.create('card', {
        style: style
      });
      card.mount('#card-element');

      console.log('Stripe Elements mounted successfully');

      // Handle validation errors
      card.on('change', function (event) {
        var displayError = document.getElementById('card-errors');
        if (event.error) {
          displayError.textContent = event.error.message;
        } else {
          displayError.textContent = '';
        }
      });

      // Find the form (may have different IDs)
      var form = $('#card-element').closest('form')[0];
      if (!form) {
        console.error('Form not found');
        return;
      }

      console.log('Form found:', form.id);

      var submitButton = $(form).find('input[type="submit"]')[0];
      if (!submitButton) {
        console.error('Submit button not found');
        return;
      }

      var originalButtonValue = submitButton.value;
      var paymentIntentCreated = false;
      var clientSecret = null;
      var formSubmitting = false;

      $(form).on('submit', function (event) {
        // If already processing, do nothing
        if (formSubmitting) {
          return true;
        }

        event.preventDefault();
        console.log('Form submitted, starting payment process');

        // Disable button
        submitButton.disabled = true;
        submitButton.value = Backdrop.t('Processing...');

        // If Payment Intent already created, confirm directly
        if (paymentIntentCreated && clientSecret) {
          console.log('Payment Intent already created, confirming payment');
          confirmPayment();
          return false;
        }

        // Create Payment Intent via AJAX
        console.log('Creating Payment Intent...');
        $.ajax({
          url: config.create_payment_intent_url,
          type: 'POST',
          dataType: 'json',
          success: function (response) {
            console.log('Server response:', response);

            if (response.error) {
              showError(response.error);
              return;
            }

            clientSecret = response.client_secret;
            paymentIntentCreated = true;
            console.log('Payment Intent created, confirming payment');
            confirmPayment();
          },
          error: function (xhr, status, error) {
            console.error('AJAX error:', status, error);
            showError(Backdrop.t('There was an error creating the payment.'));
          }
        });

        return false;
      });

      function confirmPayment() {
        // Get form data
        // Basic Cart uses different field names: basic_cart_checkout_email, basic_cart_checkout_name, etc.
        var emailField = $(form).find('input[name="basic_cart_checkout_email"]')[0];
        var nameField = $(form).find('input[name="basic_cart_checkout_name"]')[0];

        if (!emailField) {
          showError(Backdrop.t('Email field not found'));
          return;
        }

        var billingDetails = {
          email: emailField.value
        };

        if (nameField && nameField.value) {
          billingDetails.name = nameField.value;
        }

        console.log('Confirming payment with Stripe...', billingDetails);

        // Confirm payment with Stripe
        stripe.confirmCardPayment(clientSecret, {
          payment_method: {
            card: card,
            billing_details: billingDetails
          }
        }).then(function (result) {
          if (result.error) {
            console.error('Stripe error:', result.error);
            showError(result.error.message);
          } else {
            console.log('Payment confirmed:', result.paymentIntent);

            if (result.paymentIntent.status === 'succeeded') {
              console.log('Payment successful, submitting form');
              submitButton.value = Backdrop.t('Finalizing order...');

              // Mark that we're submitting the form
              formSubmitting = true;

              // Remove event handler to allow normal submit
              $(form).off('submit');

              // Submit form
              form.submit();
            }
          }
        }).catch(function (error) {
          console.error('Exception during confirmation:', error);
          showError(Backdrop.t('Unexpected error during payment'));
        });
      }

      function showError(message) {
        console.error('Showing error:', message);
        var errorElement = document.getElementById('card-errors');
        if (errorElement) {
          errorElement.textContent = message;
        }

        // Re-enable button
        submitButton.disabled = false;
        submitButton.value = originalButtonValue;
        paymentIntentCreated = false;
        clientSecret = null;
        formSubmitting = false;
      }
    }
  };

})(jQuery);
