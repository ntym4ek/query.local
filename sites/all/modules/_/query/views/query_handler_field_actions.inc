<?php

/**
 * Description of what my handler does.
 */
class query_handler_field_actions extends views_handler_field
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
    $output = '';
    // вывести формы выбора дат для каждой установки
    $query_id = $values->id;
    $month_start = $values->field_field_month[0]["raw"]["value"];
    $today_start = gmmktime(0, 0, 0, date('n'), date('j'), date('Y'));
    $deadline_start = gmmktime(0, 0, 0, date('n', $month_start)-1, 26, date('Y', $month_start));

    // новая корректировка только при утверждённой основной и отсутствии созданных позже
    $efquery = new EntityFieldQuery();
    $has_correction = $efquery
      ->entityCondition('entity_type', 'query')
      ->propertyCondition('created', $values->eck_query_created, '>')
      ->fieldCondition('field_month', 'value', $month_start)
      ->fieldCondition('field_nomenklatura', 'target_id', $values->field_field_nomenklatura[0]["raw"]["target_id"])
      ->count()
      ->execute();
    // если нет Корректировок и прошло 25 число
    if (!$has_correction && $today_start >= $deadline_start) {
      // изменения за сутки до начала производства запретить
        // для клиента
        if (!user_has_role(ROLE_MANAGER)) {
//          if ($values->field_field_query_status_client[0]["raw"]["value"] == 'processing' && $values->field_field_query_type[0]["raw"]["value"] != 'main') {
//            $output .= '<a class="btn btn-success btn-xs" href="/query/' . $query_id . '/approve">согласовать</a>';
//          }
//          if (($values->field_field_query_type[0]["raw"]["value"] == 'correction'
//            || ($values->field_field_query_type[0]["raw"]["value"] == 'main' && $values->field_field_query_status_client[0]["raw"]["value"] == 'approved'))
//            && !query_check_query_produce_started($query_id)) {
//            $output .= '<a class="btn btn-danger btn-xs" href="/admin/structure/entity-type/query/query/add?type=cancel&last_query_id=' . $query_id . '">отменить производство</a>';
//          }
          if ($values->field_field_query_type[0]["raw"]["value"] == 'main'
            || ($values->field_field_query_status_client[0]["raw"]["value"] == 'approved' && $values->field_field_query_status_manager[0]["raw"]["value"] == 'approved')) {
            $output .= '<a class="btn btn-info btn-xs" href="/admin/structure/entity-type/query/query/add?type=correction&last_query_id=' . $query_id . '">создать корректировку</a>';
          }

          // для менеджера
        } else {
//          if ($values->field_field_query_status_manager[0]["raw"]["value"] != 'approved') {
//            $output .= '<a class="btn btn-success btn-xs" href="/query/' . $query_id . '/approve">утвердить</a>';
////            $output .= '<a class="btn btn-default btn-xs" href="/admin/structure/entity-type/query/query/' . $query_id . '/edit">редактировать</a>';
//          }
//          if ($values->field_field_query_type[0]["raw"]["value"] == 'correction'
//            || ($values->field_field_query_type[0]["raw"]["value"] == 'main' && $values->field_field_query_status_client[0]["raw"]["value"] == 'approved')
//            && !query_check_query_produce_finished($query_id)) {
//            $output .= '<a class="btn btn-danger btn-xs" href="/admin/structure/entity-type/query/query/add?type=cancel&last_query_id=' . $query_id . '">отменить производство</a>';
//          }
        }

        // для обоих
//        if ($values->field_field_query_type[0]["raw"]["value"] == 'correction'
//          || ($values->field_field_query_type[0]["raw"]["value"] == 'cancel' && ($values->field_field_query_status_client[0]["raw"]["value"] != 'approved' || $values->field_field_query_status_manager[0]["raw"]["value"] != 'approved'))
//          || ($values->field_field_query_type[0]["raw"]["value"] == 'main' && $values->field_field_query_status_client[0]["raw"]["value"] == 'approved')
//          || user_has_role(ROLE_MANAGER)) {

    }

    return $output;
  }
}
