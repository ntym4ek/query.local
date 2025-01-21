<?php

/**
 * Implements hook_views_data_alter().
 */
function ext_views_views_data_alter(&$data)
{
  // поле с Установками для списка Заявок
  $data['eck_query']['produce_units_form'] = array(
    'field' => array(
      'title' => t('Produce units'),
      'help' => t('View and select produce units.'),
      'handler' => 'query_handler_field_produce_units_form',
    ),
  );
  // поле с кнопками для списка Заявок
  $data['eck_query']['actions'] = array(
    'field' => array(
      'title' => t('Actions'),
      'help' => t('Conditional actions'),
      'handler' => 'query_handler_field_actions',
    ),
  );
}

/**
 * Implements hook_views_pre_render().
 */
function ext_views_views_pre_render(&$view)
{
  if ($view->name == 'queries' && $view->current_display == 'page_1') {
    // добавить кнопку Новая заявка
    $view->attachment_before = (empty($view->attachment_before) ? '' : $view->attachment_before) . '<a href="/admin/structure/entity-type/query/query/add" class="btn btn-info">Новая заявка</a>';
  }
}

/**
 * hook_preprocess_views_view_unformatted()
 */
function ext_views_preprocess_views_view_unformatted(&$vars)
{
  // добавить класс строке основного Запроса, если есть корректирующий
  if (in_array($vars['view']->name, ['queries', 'manager_queries']) && $vars['view']->current_display == 'page_1') {
    foreach ($vars["classes"] as $key => $value) {
//      $query_status_manager = $vars["view"]->result[$key]->field_field_query_status_manager[0]["raw"]["value"];
//      $query_status_client = $vars["view"]->result[$key]->field_field_query_status_client[0]["raw"]["value"];
      $query_author_id = empty($vars["view"]->result[$key]->eck_query_uid) ? 0 : $vars["view"]->result[$key]->eck_query_uid;
      $query_month_start = $vars["view"]->result[$key]->field_field_month[0]["raw"]["value"];
//      $query_created = $vars["view"]->result[$key]->eck_query_created;
      $query_nom_tid = $vars["view"]->result[$key]->field_field_nomenklatura[0]["raw"]["target_id"];
//      $query_type = $vars["view"]->result[$key]->field_field_query_type[0]["raw"]["value"];
      $query_id = $vars["view"]->result[$key]->eck_query_id;

      // есть ли любой план по номенклатуре после заявки
//      $efquery = new EntityFieldQuery();
//      $has_plan_after = $efquery
//        ->entityCondition('entity_type', 'query')
//        ->propertyCondition('created', $query_created, '>')
//        ->fieldCondition('field_month', 'value', $query_month_start)
//        ->fieldCondition('field_nomenklatura', 'target_id', $query_nom_tid)
//        ->fieldCondition('field_query_type', 'value', 'plan')
//        ->execute();

      // есть ли Заявки от пользователя после
//      $efquery = new EntityFieldQuery();
//      $has_client_query = $efquery
//        ->entityCondition('entity_type', 'query')
//        ->propertyCondition('id', $query_id, '>')
//        ->propertyCondition('created', $query_created, '>=')
//        ->propertyCondition('uid', $GLOBALS['user']->uid)
//        ->fieldCondition('field_month', 'value', $query_month_start)
//        ->fieldCondition('field_nomenklatura', 'target_id', $query_nom_tid)
//        ->execute();

      // есть ли актуальные Корректировки после
//      $efquery = new EntityFieldQuery();
//      $actual_queries = $efquery
//        ->entityCondition('entity_type', 'query')
//        ->propertyCondition('id', $query_id, '>')
//        ->propertyCondition('created', $query_created, '>=')
//        ->fieldCondition('field_month', 'value', $query_month_start)
//        ->fieldCondition('field_nomenklatura', 'target_id', $query_nom_tid)
//        ->fieldCondition('field_query_status_client', 'value', 'approved')
//        ->fieldCondition('field_query_status_manager', 'value', 'processing', '!=')
//        ->execute();
//      if (!$has_approved_plan_after) {
//        if ($has_approved_plan) {
//          if (($has_client_query && $actual_queries) || ($actual_queries && count($actual_queries['query']) > 1)) {
//            $vars["classes_array"][$key] .= ' stamp-archive';
//          } else {
//            if ($actual_queries) {
//
//              if ($query_author_id) {
//                $vars["classes_array"][$key] .= ' stamp-' . $query_status_manager;
//              } else {
//                $vars["classes_array"][$key] .= ' stamp-archive';
//              }
//            } else {
//              if ($query_status_manager == 'processing') {
//                $vars["classes_array"][$key] .= ' stamp-processing';
//              }
//            }
//          }
//        }
//      } else {
//        $vars["classes_array"][$key] .= ' stamp-archive';
//      }

      // утверждённая заявка
//      if (!$has_plan_after || $query_type == 'plan') {
//        if (!$actual_queries
//          && $query_status_manager != 'processing'
//          && $query_status_client == 'approved') {
//          $vars["classes_array"][$key] .= ' actual';
//        } elseif ($query_status_manager == 'processing' || $query_status_client == 'processing') {
//          $vars["classes_array"][$key] .= ' processing';
//        }
//      }

      // если нет плана, то последняя Processing, остальные в архив
      $query_actual = production_query_get_actual($query_month_start, $query_nom_tid);
      $query_approved_plan = production_query_approved_plan_get($query_month_start);
      if ($query_actual && $query_id == $query_actual->id) {
        $vars["classes_array"][$key] .= $query_approved_plan ? ' actual' : ' processing';
      }

      if (empty($vars["view"]->result[$key]->_field_data["id_1"]["entity"]->field_load_json["und"][0]["value"])
          || $vars["view"]->result[$key]->_field_data["id_1"]["entity"]->field_load_json["und"][0]["value"] == '[]') {
        $vars["classes_array"][$key] .= ' no-load';
      }


      // добавить класс, если заявка от Клиента
      if ($query_author_id) {
        $vars["classes_array"][$key] .= ' from-client';
      }
    }

    // обернуть Заявки до Актуальной в Архив
//    if (count($vars["rows"]) > 1) {
//      $start_row_key = -1;
//      foreach ($vars["rows"] as $key => $row) {
//        if ($start_row_key == -1) {
//          $start_row_key = $key + 1;
//        }
//      }
//      if ($start_row_key != -1) {
//        $end_row_key = array_key_last($vars["rows"]);
//        $vars["prefix_row_key"] = $start_row_key;
//        $vars["suffix_row_key"] = $end_row_key;
//      }
//    }
  }

  if ($vars['view']->name == 'produce_units_load_client' && $vars['view']->current_display == 'page') {
    $vars["view"]->attachment_before =  '<div class="produce-unit-legenda">' .
                                          '<span></span>- установка свободна <span class="loaded"></span>- установка загружена <span class="selected"></span>- производство продукции' .
                                        '</div>';

    // исключить пустые строки
//    $rows = [];
//    foreach ($vars["rows"] as $key => $row) {
//      if (strpos($row, '<div class="produce-unit">') !== false) {
//        $rows[] = $row;
//      }
//    }
//    $vars["rows"] = $rows;
  }
}


/**
 * hook_preprocess_views_view_unformatted()
 */
function ext_views_preprocess_views_view_table(&$vars)
{
  if ($vars['view']->name == 'nomenklatura' && $vars['view']->current_display == 'page') {
    $rows = [];
    foreach ($vars["rows"] as $key => $row) {
      // заменить на артикул и название давальца
      $nom_info = production_nomenklatura_get_info($vars["result"][$key]->tid);
      $row["name"] = $nom_info['label'];

      if ($row['produce_unit_load_amount']) $rows[] = $row;
    }
    $vars["rows"] = $rows;
  }

  if (in_array($vars['view']->name, ['report', 'report_by_unit']) && in_array($vars['view']->current_display, ['client', 'manager'])) {

    $client_count = $prod_count = $neutral_count = 0;
    foreach($vars["result"] as $item) {
      if ($item->field_field_fault[0]["raw"]["value"] == 1) $client_count++;
      elseif ($item->field_field_fault[0]["raw"]["value"] == 2) $prod_count++;
      else $neutral_count++;
    }
    $chart = array(
      '#type' => 'chart',
      '#title' => 'Инициатор изменений',
      '#title_color' => '#999',
      '#title_font_size' => 12, // Font size in pixels, e.g. 12.
      '#title_position' => 'in', // Options: in, out.
      '#colors' => user_has_role(ROLE_MANAGER_PRODUCTION) ? ['#5cb85c', '#d9534f', '#ccc'] : ['#d9534f', '#5cb85c', '#ccc'],
      '#chart_type' => 'pie',
      '#chart_library' => 'google',
      '#legend' => false,
      '#legend_position' => 'bottom',
      '#data_labels' => TRUE,
      '#width' => 300,
      '#tooltips' => false,
    );
    $chart['pie_data'] = array(
      '#type' => 'chart_data',
      '#title' => t('Gender'),
      '#labels' => ['Клиент', 'Агрохимикат', 'Нет'],
      '#data' => [$client_count, $prod_count, $neutral_count],
    );

    $vars['chart'] = $chart;
  }
}

/**
 * hook_preprocess_views_view_field()
 */
function ext_views_preprocess_views_view_field(&$vars)
{
  if (in_array($vars["view"]->name, ['queries', 'manager_queries']) && isset($vars["field"]->field_alias) && $vars["field"]->field_alias == 'eck_query_uid') {
    if (empty($vars["row"]->eck_query_uid))
      $vars["output"] = 'Производство';
    else
      $vars["output"] = 'Клиент';
  }

  if ($vars["view"]->name == 'queries' && isset($vars["field"]->field_info["field_name"])) {
    if ($vars["field"]->field_info["field_name"] == 'field_nomenklatura') {
      $nom_info = production_nomenklatura_get_info($vars["row"]->field_field_nomenklatura[0]["raw"]["target_id"]);
      $vars["output"] = $nom_info['label'];
    }

    if ($vars["field"]->field_info["field_name"] == 'field_query_reason') {
      if (empty($vars["row"]->field_field_query_reason[0]["raw"]["tid"])) $vars["output"] = '';
    }

    if ($vars["field"]->field_info["field_name"] == 'field_query_status_manager') {
      if ($vars["row"]->field_field_query_type[0]["raw"]["value"] == 'plan' && $vars["row"]->field_field_query_status_client[0]["raw"]["value"] == 'approved') {
        $vars["output"] = 'Утверждён';
      }
      elseif (!empty($vars["row"]->eck_query_uid)) {
        if (in_array($vars["row"]->field_field_query_type[0]["raw"]["value"], ['correction', 'request']) && $vars["row"]->field_field_query_status_manager[0]["raw"]["value"] == 'approved') {
          $vars["output"] = 'Утверждена';
        }
      }
      elseif ($vars["row"]->field_field_query_type[0]["raw"]["value"] != 'deny' && $vars["row"]->field_field_query_status_client[0]["raw"]["value"] == 'processing') {
        $vars["output"] = 'На согласовании';
      }
      elseif ($vars["row"]->field_field_query_status_client[0]["raw"]["value"] == 'approved'
        && $vars["row"]->field_field_query_status_manager[0]["raw"]["value"] == 'approved'
        && $vars["row"]->field_field_qty[0]["raw"]["value"] == 0) {
        $vars["output"] = 'Производство отменено';
      }
      else {
        $vars["output"] = '';
      }
    }
  }

  if ($vars["view"]->name == 'report' && isset($vars["field"]->field_info["field_name"])) {
    if ($vars["field"]->field_info["field_name"] == 'field_nomenklatura') {
      $nom_info = production_nomenklatura_get_info($vars["row"]->taxonomy_term_data_field_data_field_nomenklatura_tid);
      $vars["output"] = $nom_info['label'];
    }
  }
}
