<?php

/**
 * Description of what my handler does.
 */
class query_handler_field_produce_units_form extends views_handler_field
{
  function query()
  {
    $this->ensure_my_table();
    $this->add_additional_fields();
  }

  /**
   * Renders the field handler.
   */
  function render($values)
  {
    // вывести выбранные даты для каждой установки
    $output = '';
    $query_id = $values->id;

    $query_wr = entity_metadata_wrapper('query', $query_id);
    $month_start = $query_wr->field_month->value();
    $nom_tid = $query_wr->field_nomenklatura->tid->value();
    $nom_client_id = $query_wr->field_nomenklatura->field_user->uid->value();

    // значения Загрузки по умолчанию
    $produce_units = [];
    $current_load = [];
    $current_load_days = 0;
    foreach ($query_wr->field_load->getIterator() as $load_wr) {
      if ($load_wr->value()) {
        $pu_tid = $load_wr->field_produce_unit->tid->value();
        $current_load[$pu_tid][$load_wr->field_produce_date->value()] = $load_wr->field_produce_output->value();
        $current_load_days++;

        // добавить установки из Загрузки (пока нет связи Номенклатура-Установка)
        if (empty($produce_units[$pu_tid])) {
          $produce_units[$pu_tid] = [
            'tid' => $pu_tid,
            'name' => $load_wr->field_produce_unit->name->value(),
            'performance' => query_get_product_unit_performance_by_nomenklatura($pu_tid, $nom_tid),
          ];
        }
      }
    }

    foreach ($produce_units as $pu_tid => $produce_unit) {
      $produce_unit_load = query_get_produce_unit_load($pu_tid, $month_start);

      $output .=
        '<div class="produce-unit">' .
          '<div class="produce-unit-name">' .
            '<label class="views-label" for="edit-name--2">Установка</label>' .
            '<h3>' . $produce_unit['name'] . '</h3>' .
          '</div>' .
          '<div class="produce-unit-dates">' .
            '<label class="views-label">Даты</label>' .
            '<div class="form-checkboxes">';
      for ($i = 1; $i <= date("t", $month_start); $i++) {
        $day_start = $month_start + ($i - 1) * 60 * 60 * 24;

        // для текущей Номенклатуры занятость не выводим (в случае заполнения корректирующей Заявки)
        $produce_unit_is_busy = !empty($produce_unit_load[$day_start]) && $produce_unit_load[$day_start]['nomenklatura']['tid'] != $nom_tid;
        $produce_unit_has_current_load = !empty($current_load[$pu_tid][$day_start]);
        // загрузка установки этим же клиентом
        $produce_unit_is_busy_same_company = !empty($produce_unit_load[$day_start]) && $produce_unit_load[$day_start]['nomenklatura']['client_id'] == $nom_client_id;


        // вывести подсказку при наведении с выходом продукции в этот день
        $classes = [];
        if ($produce_unit_has_current_load) {
          $classes[] = 'selected';
        } else {
          if ($produce_unit_is_busy_same_company) {
            $classes[] = 'company-loaded';
          } else {
            if ($produce_unit_is_busy) $classes[] = 'loaded';
          }
        }

        $tooltip = empty($current_load[$pu_tid][$day_start]) ? '' : ' data-toggle="tooltip" data-placement="top" title="' . $current_load[$pu_tid][$day_start] . '"';

        $output .= '<span class="c-box ' . implode(' ', $classes) . '"' . $tooltip . '>' . $i . '</span>';
      }
      $output .=
            '</div>' .
            '<div class="produce-unit-legenda"><span></span>- установка свободна <span class="loaded"></span>- установка занята <span class="company-loaded"></span>- другая продукция компании <span class="selected"></span>- выбранные даты</div>' .
          '</div>' .
        '</div>';
    }
    return $output;
  }
}
