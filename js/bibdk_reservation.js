(function($) {
  var BibdkReservation = {};

  BibdkReservation.redirectToUserStatus = function(context) {
    $('.redirect-parent', context).click(function(event) {
      event.preventDefault();
      if(opener != null) {
        opener.location.href = $('.redirect-parent').attr('href');
        close();
      }
      else {
        location.href = $('.redirect-parent').attr('href');
      }
    });
  };

  Drupal.behaviors.bibdk_reservation = {
    attach: function(context) {
      BibdkReservation.redirectToUserStatus(context);
    }
  };
}(jQuery));
