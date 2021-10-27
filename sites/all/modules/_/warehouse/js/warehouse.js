(function ($) {
  Drupal.behaviors.warehouse = {
    attach: function (context, settings) {

      $(".warehouse-form .day").on("click", function() {
        if ($(this).hasClass("active")) {
          $(this).removeClass("active");
        } else {
          $(".warehouse-form .day").each(function() {
            $(this).removeClass("active");
          });

          $(this).addClass("active");
          updateTime();
        }
      });

      $(".warehouse-form .time-content").on("click", function() {
        if ($(this).hasClass("active")) {
          $(this).removeClass("active");
        } else {
          if ($(this).hasClass("free")) {
            $(".warehouse-form .time-content").each(function() {
              $(this).removeClass("active");
              $("input[name=time]").val(0);
            });

            // выделить свободное время и занести данные в форму
            $(this).addClass("active");
            updateTime();
          }
        }
      });

      function updateTime() {
        var $date = $(".warehouse-form .day.active").data("date");
        var $start_offset = $(".warehouse-form  .time-content.active").data("start-offset") * 60;

        $("input[name=time]").val($date + $start_offset);
      }

    }
  }
})(jQuery);
