(function($) {
    var BibdkReservation = {};

    BibdkReservation.redirectToUserStatus = function(context) {
      $('.redirect-parent', context).click(function(event) {
        event.preventDefault();
        var href = $(this).attr('href');
        var target = $(this).attr('target');

        if(window.opener != null) {
          window.opener.open(href, target);
          close();
        }
        else {
          window.open(href, target);
        }
      });

    };

    Drupal.behaviors.bibdk_reservation = {
      attach: function(context) {
        BibdkReservation.redirectToUserStatus(context);
      }
    };

    Drupal.behaviors.chgLibLink = {
        attach: function(context, settings) {
            $('.chg-lib-link', context).click(function(event) {
              event.preventDefault();
              $('#edit-prev').click();
            });
        }
    };

})(jQuery);

