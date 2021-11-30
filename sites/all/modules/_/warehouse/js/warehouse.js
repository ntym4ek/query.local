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

      // - маски ввода на полях ----------------------------------------------------------------------------------------
      setMasks();
      $('#edit-foreign').click(function() {
        if ($(this).prop('checked'))  unsetMasks();
        else                          setMasks();
      });
      function setMasks() {
        $('[id*=field-truck-number]').prop('placeholder', 'А000АА');
        $('[id*=field-truck-number]').attr('data-original-title', 'только номер, без региона');
        $('[id*=field-truck-number]').mask('а999аа');
        $('[id*=field-truck-region]').show().prop('disabled', '');
        $('[id*=field-truck-trailer-number]').prop('placeholder', 'АА 0000');
        $('[id*=field-truck-trailer-number]').mask('аа 9999');
      }
      function unsetMasks() {
        $('[id*=field-truck-number]').prop('placeholder', '');
        $('[id*=field-truck-number]').attr('data-original-title', 'иностранный номер полностью');
        $('[id*=field-truck-number]').mask('******?****');
        $('[id*=field-truck-region]').val('').prop('disabled', 'disabled');
        $('[id*=field-truck-trailer-number]').prop('placeholder', '');
        $('[id*=field-truck-trailer-number]').mask('******?****');
      }

      // - виджет ввода даты-времени -----------------------------------------------------------------------------------
      if (typeof $.fn.datetimepicker === 'function') {
        $("[name^=field_wwork_time]").datetimepicker({
          lang: 'ru',
          format: 'd.m.Y - H:i',
          dayOfWeekStart: 1,
          allowBlank: false,
          validateOnBlur: false,
          step: 15,
          minTime: "07:00",
          maxTime: "18:15",
          defaultSelect: false
        });
      }

    }
  }
})(jQuery);
