(function ($) {
  Drupal.behaviors.bosa_voucher = {
    attach: function (context, settings) {

      var run,
          id;

      function resize(){
        if(window.innerWidth < 800){
          $('.region-page-top').remove();
          $('.above-header').remove();
          $('footer').remove();
          $('#page').addClass('mobile');
        }
      }

      function order_id_click(event){
        id = $(this).parent().data('id');
        $('.order_id-'+id).toggleClass('hide');
      }


      function reload(){
        $('#bosa_voucher_load').show();

        path = document.location.pathname;
        n = path.split('/');

        $('#bosa_voucher_attendees').load('/bosa/voucher/product/ajax/'+n[5], function(){
          $('#bosa_voucher_load').hide();
          run = resize();

          $('.order_id').bind('click', order_id_click);

        });
      }

      run = resize();
      run = reload();

      window.setInterval(function(){reload();}, 60000);
    }
  };
}(jQuery));

