(function ($) {

  var isMouseDown = false;

  Drupal.behaviors.query = {
    attach: function (context, settings) {

      // форма создания Заявки, сделать кнопку сабмита неактивной после нажатия
      $(".query-form [name=eck_submit]").on("click", function() {
        // без таймаута не даст форме сработать событию сабмита
        setTimeout(function () {
          $(".query-form [name=eck_submit]").prop('disabled', true).text("Отправка...");
        }, 0);
      });


      // выделение нескольких чекбоксов проведением над ними -----------------------------------------------------------
      $("body").once(function() {
        $("body").mousedown(function() {
          isMouseDown = true
        });
        $("body").mouseup(function() {
          isMouseDown = false
        });
      });

      $(".query-form").once(function() {
        $(".produce-unit input[type=checkbox]+span").mousedown(function (e) {
          isMouseDown = true;
          var input = $(this).closest('.form-type-checkbox').find('input');
          checkCheck(input);
        });
        $(".produce-unit input[type=checkbox]+span").mouseover(function () {
          var input = $(this).closest('.form-type-checkbox').find('input');
          checkCheck(input)
        });
        $('.produce-unit input[type="checkbox"]').click(function (e) {
          e.preventDefault();
          e.stopPropagation();
        });
      });

      function checkCheck(el) {
        if (isMouseDown) {
          var input_value = el.is(':checked');
          var is_disabled = el.is(':disabled');
          if (!is_disabled) {
            el.prop("checked", !input_value);
          }
        }
      }

    }
  };
})(jQuery);
