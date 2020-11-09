(function ($) {

  var form_states = {};

  Drupal.behaviors.query = {
    attach: function (context, settings) {

      // сохранить начальное состояние чекбоксов
      $("form.produce-unit-form").each(function() {
        $(this).find("button[id^=edit-submit]").hide();
        var form_id = $(this).attr("id");
        $(this).find("input[type=checkbox]").each(function() {
          var input_id = $(this).attr("id");
          var input_value = $(this).is(':checked');
          if (form_states[form_id] == undefined) form_states[form_id] = {};
          form_states[form_id][input_id] = input_value;
        });
      });

      // в зависимости от наличия изменений чекбоксов вывести или спрятать кнопку Сохранить
      $(".produce-unit-form input[type=checkbox]").click(function() {
        var form = $(this).closest("form");
        var is_changed = false;
        var days_count = 0;
        $(this).closest("form").find("input[type=checkbox]").each(function() {
          var input_id = $(this).attr("id");
          var input_val = $(this).is(':checked');
          if (input_val != form_states[form.attr("id")][input_id]) {
            is_changed = true;
          }
          if (input_val) days_count++;
        });
        if (is_changed) $(form).find("button[id^=edit-submit]").show();
        else $(form).find("button[id^=edit-submit]").hide();

        // todo пересчёт количества от выбранных дней
        var amount_el = $(this).closest("form").find(".produce-unit-performance .amount");
        var amount = amount_el.data("performance") * days_count;
        amount_el.find("span").text(amount);
      });


    }
  };
})(jQuery);
