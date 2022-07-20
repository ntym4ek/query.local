<?php


function production_queries_delete($month_start = null)
{
  $efquery = new EntityFieldQuery();
  $efquery->entityCondition('entity_type', 'query');
  if ($month_start) {
    $efquery->fieldCondition('field_month', 'value', $month_start);
  }
  $result = $efquery->execute();
  if ($result) {
    entity_delete_multiple('query', array_keys($result['query']));
    drupal_set_message('Заявки удалены.');
  }

  // техработы теперь в отдельных заявках типа service
  // вычистить технические работы
  $efquery = new EntityFieldQuery();
  $efquery->entityCondition('entity_type', 'load');
  if ($month_start) {
    $next_month_start = gmmktime(0,0,0, date('n', $month_start)+1, date('j', $month_start), date('Y', $month_start));
    $efquery->fieldCondition('field_produce_date', 'value', $month_start, '>=')
            ->fieldCondition('field_produce_date', 'value', $next_month_start, '<');
  }
  $result = $efquery->execute();
  if ($result) {
    entity_delete_multiple('load', array_keys($result['load']));
    drupal_set_message('Загрузка удалена.');
  }
}

function production_davalec_import($array)
{
  if (is_array($array) && !empty($array['load'])) {
    $got = [];
    foreach ($array['load'] as $key => $davalec) {
        if ($davalec['dav_id'] && $davalec['dav_name']) {
          $query = new EntityFieldQuery();
          $users = $query
            ->entityCondition('entity_type', 'user')
            ->fieldCondition('field_oc_davalec_id', 'value', $davalec['dav_id'])
            ->execute();

          if (!$users) {
            $user = user_save(NULL, [
              'name' => $davalec['dav_name'],
              'pass' => user_password(8),
              'status' => 1,
              'roles' => [DRUPAL_AUTHENTICATED_RID => TRUE],
            ]);
          } else {
            $user = current($users['user']);
          }

          $user_wr = entity_metadata_wrapper('user', $user->uid);
          $user_wr->name = $davalec['dav_name'];
          $user_wr->field_oc_davalec_id = $davalec['dav_id'];
          $user_wr->status = 1;
          $user_wr->save();

          $got[] = $user_wr->getIdentifier();
        }
    }

    // если полное обновление справочника, то запомнить отсутствующие записи и сделать их неактивными
    if ($array['reload']) {
      $query = new EntityFieldQuery();
      $users = $query
        ->entityCondition('entity_type', 'user')
        ->fieldCondition('field_oc_davalec_id', 'value', '', '<>')
        ->execute();

      if ($users) {
        foreach ($users['user'] as $uid => $user) {
          if (!in_array($uid, $got)) {
            $old_term_wr = entity_metadata_wrapper('user', $user->uid);
            $old_term_wr->status = 0;
            $old_term_wr->save();
          }
        }
      }
    }

    drupal_set_message('Давальцы импортированы.');
    watchdog('import', 'Давальцы импортированы.', [], WATCHDOG_INFO);
    return true;
  } else {
    drupal_set_message('Импорт давальцев не выполнен.', 'warning');
    watchdog('import', 'Импорт давальцев не выполнен.', [], WATCHDOG_ERROR);
    return false;
  }
}

function production_produce_units_import($array)
{
  if (is_array($array) && !empty($array['load'])) {
    $vid = taxonomy_vocabulary_machine_name_load('produce_units')->vid;

    $punits_got = [];
    foreach($array['load'] as $key => $punit) {
        if ($punit['ust_id'] && $punit['ust_name']) {
          $produce_unit = production_produce_unit_get_by_1cID($punit['ust_id']);

          if (!$produce_unit) {
            $produce_unit = (object)array(
              'vid' => $vid,
              'name' => $punit['ust_name'],
            );
            taxonomy_term_save($produce_unit);
          }

          // обновить значения полей
          $produce_unit_wr = entity_metadata_wrapper('taxonomy_term', $produce_unit);
          $produce_unit_wr->name = $punit['ust_name'];
          $produce_unit_wr->field_oc_produce_unit_id = $punit['ust_id'];
          $produce_unit_wr->field_status = 1;
          $produce_unit_wr->save();

          $punits_got[] = $produce_unit_wr->getIdentifier();
        }
    }

    // если полное обновление справочника, то запомнить отсутствующие записи и сделать их неактивными для клиента
    if ($array['reload']) {
      $dbr = db_select('taxonomy_term_data', 't');
      $dbr->condition('t.vid', $vid);
      $dbr->addField('t', 'tid');
      $punits_old = $dbr->execute()->fetchAllAssoc('tid');

      foreach ($punits_old as $pu_tid => $punit) {
        if (!in_array($pu_tid, $punits_got)) {
          $term_wr = entity_metadata_wrapper('taxonomy_term', $pu_tid);
          $term_wr->field_status = 0;
          $term_wr->save();
        }
      }
    }

    drupal_set_message('Установки импортированы.');
    watchdog('import', 'Установки импортированы.', [], WATCHDOG_INFO);
    return true;
  } else {
    drupal_set_message('Импорт установок не выполнен.', 'warning');
    watchdog('import', 'Импорт установок не выполнен.', [], WATCHDOG_ERROR);
    return false;
  }
}

function production_reasons_import($array)
{
  if (is_array($array) && !empty($array['load'])) {
    $vid = taxonomy_vocabulary_machine_name_load('reasons')->vid;

    $got = [];
    foreach($array['load'] as $key => $item) {
      if ($item['cause_id'] && $item['cause_name']) {
        $reason = production_reason_get_by_oc_id($item['cause_id']);

        if (!$reason) {
          $reason = (object)array(
            'vid' => $vid,
            'name' => $item['cause_name'],
          );
          taxonomy_term_save($reason);
        }

        if (!is_numeric($item['cause_fault'])) {
          $item['cause_fault'] = 0;
        }

        // обновить значения полей
        $reason_wr = entity_metadata_wrapper('taxonomy_term', $reason);
        $reason_wr->name = $item['cause_name'];
        $reason_wr->field_reason_oc_id = $item['cause_id'];
        $reason_wr->field_fault = $item['cause_fault'];
        $reason_wr->field_status = 1;
        $reason_wr->save();

        $got[] = $reason_wr->getIdentifier();
      }
    }

    // если полное обновление справочника, то запомнить отсутствующие записи и сделать их неактивными для клиента
    if ($array['reload']) {
      $dbr = db_select('taxonomy_term_data', 't');
      $dbr->condition('t.vid', $vid);
      $dbr->addField('t', 'tid');
      $cause_old = $dbr->execute()->fetchAllAssoc('tid');

      foreach ($cause_old as $c_tid => $cause) {
        if (!in_array($c_tid, $got)) {
          $term_wr = entity_metadata_wrapper('taxonomy_term', $c_tid);
          $term_wr->field_status = 0;
          $term_wr->save();
        }
      }
    }

    drupal_set_message('Причины импортированы.');
    watchdog('import', 'Причины импортированы.', [], WATCHDOG_INFO);
    return true;
  } else {
    drupal_set_message('Импорт справочника причин не выполнен.', 'warning');
    watchdog('import', 'Импорт справочника причин не выполнен.', [], WATCHDOG_ERROR);
    return false;
  }
}

function production_nomenklatura_import($array)
{
  if (is_array($array) && !empty($array['load'])) {
    $vid = taxonomy_vocabulary_machine_name_load('nomenklatura')->vid;

    $got = [];
    foreach($array['load'] as $key => $nom) {
        if ($nom['name'] && $nom['art'] && $nom['dav_id']) {
          // найти пользователя
          $query = new EntityFieldQuery();
          $users = $query
            ->entityCondition('entity_type', 'user')
            ->fieldCondition('field_oc_davalec_id', 'value', $nom['dav_id'])
            ->execute();
          if ($users) {
            // проверить наличие записи Номенклатуры
            $query = new EntityFieldQuery();
            $terms = $query
              ->entityCondition('entity_type', 'taxonomy_term')
              ->propertyCondition('vid', $vid)
              ->fieldCondition('field_oc_artikul', 'value', $nom['art'])
              ->execute();

            if (!$terms) {
              $term = (object)array(
                'vid' => $vid,
                'name' => $nom['name'],
              );
              taxonomy_term_save($term);
            } else {
              $term = current($terms['taxonomy_term']);
            }

            // обновить значения полей
            if ($user = current($users['user'])) {
              $term_wr = entity_metadata_wrapper('taxonomy_term', $term);
              $term_wr->name = $nom['name'];
              $term_wr->field_oc_artikul = $nom['art'];
              $term_wr->field_oc_name_davalec = $nom['nnd'];
              $term_wr->field_oc_artikul_davalec = $nom['anr'];
              $term_wr->field_user = $user->uid;
              $term_wr->field_status = 1;
              $term_wr->save();

              $got[] = $term_wr->getIdentifier();
            }

          } else {
            drupal_set_message('Не найден давалец с ID: ' . $nom['dav_id'], 'error');
            watchdog('import', 'Импорт номенклатуры не выполнен. Не найден давалец с ID: %dav_id', ['%dav_id' => $nom['dav_id']], WATCHDOG_ERROR);
            return false;
          }
        }
    }

    // если полное обновление справочника, то запомнить отсутствующие записи и сделать их неактивными для клиента
    if ($array['reload']) {
      $dbr = db_select('taxonomy_term_data', 't');
      $dbr->condition('t.vid', $vid);
      $dbr->addField('t', 'tid');
      $noms_old = $dbr->execute()->fetchAllAssoc('tid');

      foreach ($noms_old as $n_tid => $nom_old) {
        if (!in_array($n_tid, $got)) {
          $term_wr = entity_metadata_wrapper('taxonomy_term', $n_tid);
          $term_wr->field_status = 0;
          $term_wr->save();
        }
      }
    }

    drupal_set_message('Номенклатура импортирована.');
    watchdog('import', 'Номенклатура импортирована.', [], WATCHDOG_INFO);
    return true;
  } else {
    drupal_set_message('Импорт номенклатуры не выполнен.', 'warning');
    watchdog('import', 'Импорт номенклатуры не выполнен.', [], WATCHDOG_ERROR);
    return false;
  }
}

function production_produce_plan_import($array)
{
  if (is_array($array) && !empty($array['load'])) {
    // подготовить массив с Планом для внесения в базу
    // определить Заявки и посчитать общее производимое количество
    $plan = [];
    $month_arr = explode('.', $array['month']);
    $month_start = gmmktime(0, 0, 0, $month_arr[1], 1, $month_arr[2]);
    $plan_import_time = REQUEST_TIME;

    //  если есть утверждённый план, импорт запретить
    if ($has_approved_plan = production_query_approved_plan_get($month_start)) {
      watchdog('import', 'Утверждённый план уже загружен. Импорт плана %month не выполнен.', ['%month' => $array['month']], WATCHDOG_ERROR);

      // уведомить об ошибке для устранения причин
      drupal_mail('system', 'mail', 'mega_comp@mail.ru', language_default(), array(
        'context' => array(
          'subject' => 'Производство Агрохимикат. Ошибка импорта',
          'message' => 'Утверждённый план уже загружен. Импорт плана ' . $array['month'] . ' не выполнен. Возможно, в базе есть ошибочные утверждённые заявки плана, которые не дают импортировать реальный план.',
        )
      ));
      return false;
    }

    $produce_unit_id_arr = [];
    foreach ($array['load'] as $key => $item) {
      if ($item['artikul_1c'] && $item['unit_id_1c'] && $item['date']) {
        if (($nom = production_nomenklatura_get_by_artikul($item['artikul_1c']))
          && ($produce_unit = production_produce_unit_get_by_1cID($item['unit_id_1c']))) {
          $date_arr = explode('.', $item['date']);
          $day_start = gmmktime(0, 0, 0, $date_arr[1], $date_arr[0], $date_arr[2]);

          if (!isset($plan[$nom->tid])) {
            $plan[$nom->tid] = [
              'field_month' => $month_start,
              'field_nomenklatura' => $nom->tid,
              'uid' => $nom->field_user['und'][0]['target_id'],
              'field_qty' => 0,
              'load' => [],
            ];
          }

          $output = round($item['output']); // общее количество в заявке - целочисленное поле
          $plan[$nom->tid]['field_qty'] += $output;
          if (!isset($plan[$nom->tid]['load'][$produce_unit->tid])) $plan[$nom->tid]['load'][$produce_unit->tid] = [
            'pu_tid' => $produce_unit->tid,
            'pu_load' => [],
          ];
          $plan[$nom->tid]['load'][$produce_unit->tid]['pu_load'][$day_start] = $output;

          $plan[$nom->tid]['produce_units'][$produce_unit->tid] = $produce_unit->tid;
          $produce_unit_id_arr[$item['unit_id_1c']] = $produce_unit->tid;

        } else {
          if (empty($nom)) {
            watchdog('import', 'Импорт плана. Не найдена номенклатура с 1с ID: %artikul_1c', ['%artikul_1c' => $item['artikul_1c']], WATCHDOG_ERROR);
          }
          if (empty($produce_unit)) {
            watchdog('import', 'Импорт плана. Не найдена установка с 1с ID: %unit_id_1c', ['%unit_id_1c' => $item['unit_id_1c']], WATCHDOG_ERROR);
          }
//          return false; продолжаем импорт
        }
      }
    }

    // удалить старый план на месяц
    production_plan_delete($month_start);

    // создание Заявок на месяц
    $user_mails = [];
    foreach ($plan as $plan_item) {

      $query = entity_create('query', [
        'type' => 'query',
        'created' => $plan_import_time,
        'uid' => 0,
      ]);
      entity_save('query', $query);

      $query_wr = entity_metadata_wrapper('query', $query);
      $query_wr->field_month = $plan_item['field_month'];
      $query_wr->field_nomenklatura = $plan_item['field_nomenklatura'];
      $query_wr->field_qty = $plan_item['field_qty'];
      $query_wr->field_query_type = 'plan';
      $query_wr->field_query_status_client = ($array['type'] == 'main' ? 'approved' : 'processing'); // план может быть предварительным "pred" и утверждённым "main"
      $query_wr->field_query_status_manager = 'approved';
      $query_wr->field_query_period = ['value' => 0, 'value2' => 0];
      $query_wr->field_load_json = drupal_json_encode($plan_item['load']);
      $query_wr->save();

      if (empty($user_mails[$plan_item['uid']])) {
        $user_mails[$plan_item['uid']] = [
          'month' => $plan_item['field_month'],
          'company_uid' => $plan_item['uid']
        ];
      }
    }

    if (!empty($array['service'])) {
      $service_load_arr = [];
      foreach ($array['service'] as $item) {
        if ($produce_unit = production_produce_unit_get_by_1cID($item['unit_id_1c'])) {
          $date_arr = explode('.', $item['date']);
          $day_start = gmmktime(0, 0, 0, $date_arr[1], $date_arr[0], $date_arr[2]);
          if (!isset($service_load_arr[$produce_unit->tid])) $service_load_arr[$produce_unit->tid] = [
            'pu_tid' => $produce_unit->tid,
            'pu_load' => [],
          ];
          $service_load_arr[$produce_unit->tid]['pu_load'][$day_start] = 0;
        } else {
          watchdog('import', 'Импорт занятости при импорте плана. Не найдена установка "%unit_id_1c". ', ['%unit_id_1c' => $item['unit_id_1c']], WATCHDOG_ERROR);
          //return false;
        }
      }

      if ($service_load_arr) {
        $query_zan = entity_create('query', [
          'type' => 'query',
          'created' => $plan_import_time,
          'uid' => 0,
        ]);
        entity_save('query', $query_zan);

        $query_zan_wr = entity_metadata_wrapper('query', $query_zan);
        $query_zan_wr->field_month = $month_start;
        $query_zan_wr->field_nomenklatura = TID_SERVICE_WORK; // tid Технических работ
        $query_zan_wr->field_query_type = 'service';
        $query_zan_wr->field_load_json = drupal_json_encode($service_load_arr);
        $query_zan_wr->save();
      }
    }

    // отправить письма об импорте плана
    if ($user_mails) {
      foreach ($user_mails as $user_id => $data) {
        $params['month'] = t(date('F', $data['month']));
        $params['year'] = date('Y', $data['month']);
        try {
          if ($users = ext_user_managers_get_by_company_id($data['company_uid'])) {
            foreach ($users as $manager_account) {
              if ($manager_account->mail && empty($manager_account->field_notify_once_a_day["und"][0]["value"])) {
                drupal_mail('mailer', 'user_notify_plan_import', $manager_account->mail, user_preferred_language($manager_account), $params);
              }
            }
          }
        } catch (Exception $e) {
          watchdog('mail', 'Ошибка при отправке письма заявки. %mess', ['%mess' => $e->getMessage()],WATCHDOG_ERROR);
          return false;
        }
      }
    }

    watchdog('import', 'План производства импортирован.', [], WATCHDOG_INFO);
    return $plan;
  } else {
    watchdog('import', 'Импорт плана не выполнен.', [], WATCHDOG_ERROR);
    return false;
  }
}


// типы заявок (query_type)
//  request - согласования до принятия плана
//  plan - согласованный план
//  correction - корректировки после принятия
function production_query_import($array)
{
  $query = null;
  $old_query_id = empty($array['ID']) ? 0 : $array['ID'];
  $query_oc_id = empty($array["ID_1c"]) ? '' : $array["ID_1c"];
  $month_value = empty($array['month']) ? '' : $array['month'];
  $query_import_time = REQUEST_TIME;

  if (!$month_value) {
    watchdog('import', 'Пустое поле меcяца в файле экспорта. (%query_oc_id)', ['%query_oc_id' => $query_oc_id], WATCHDOG_ERROR);
    return false;
  }
  $artikul = empty($array['artikul_1c']) ? '' : $array['artikul_1c'];
  $nom = production_nomenklatura_get_by_artikul($artikul);
  if (!$nom) {
    watchdog('import', 'Не найдена номенклатура по артикулу "%artikul". (%query_oc_id)', ['%artikul' => $artikul, '%query_oc_id' => $query_oc_id], WATCHDOG_ERROR);
    return false;
  }

  $query_status_manager = empty($array['status_manager']) ? '' : $array['status_manager'];
  if (!in_array($query_status_manager, ['processing', 'approved', 'deny'])) {
    watchdog('import', 'Не найден статус менеджера "%query_status_manager". (%$query_oc_id)', ['%query_status_manager' => $query_status_manager, '%query_oc_id' => $query_oc_id], WATCHDOG_ERROR);
    return false;
  }

  // получить Загрузку
  $load_arr = [];
  $query_qty = 0;
  if (!empty($array['load'])) {
    foreach($array['load'] as $item) {
      if ($produce_unit = production_produce_unit_get_by_1cID($item['unit_id_1c'])) {
        $date_arr = explode('.', $item['date']);
        $day_start = gmmktime(0, 0, 0, $date_arr[1], $date_arr[0], $date_arr[2]);
        if (!isset($load_arr[$produce_unit->tid])) $load_arr[$produce_unit->tid] = [
          'pu_tid' => $produce_unit->tid,
          'pu_load' => [],
        ];
        $output = round($item['output']); // общее количество в заявке - целочисленное поле
        $load_arr[$produce_unit->tid]['pu_load'][$day_start] = $output;

        $query_qty += $output;
      } else {
        watchdog('import', 'Импорт заявки. Не найдена установка "%unit_id_1c". (%query_oc_id)', ['%unit_id_1c' => $item['unit_id_1c'], '%query_oc_id' => $query_oc_id],WATCHDOG_ERROR);
        return false;
      }
    }
  }

  // в Отказе должно быть количество, так как Загрузка отсутствует
  if (!$query_qty && $query_status_manager == 'deny' && !empty($array['volume'])) {
    $query_qty = $array['volume'];
  }

  // проверить наличие исходной Заявки по id и сменить статус менеджера
  if ($old_query_id) {
    try {
      $old_query_wr = entity_metadata_wrapper('query', $old_query_id);
      if ($old_query_wr->value()) {
        // проверить, совпадает ли артикул из новой заявки со старой (просили одинэсники)
        $old_artikul_1c = $old_query_wr->field_nomenklatura->field_oc_artikul->value();
        if ($old_artikul_1c != $artikul) {
          watchdog('import', 'Артикул из старой Заявки: %old_artikul_1c не совпадает с артикулом из ответа: %artikul. (%query_oc_id)', ['%old_artikul_1c' => $old_artikul_1c, '%artikul' => $artikul, '%query_oc_id' => $query_oc_id],WATCHDOG_ERROR);
          return false;
        }
        // может быть approved или deny
        $old_query_wr->field_query_status_manager = $query_status_manager;
        $old_query_wr->save();
      }
    } catch (Exception $e) {}
  }

  // найти заявку по ID_1c, если передан
  $date_arr = explode('.', $month_value);
  $month_start = gmmktime(0, 0, 0, $date_arr[1], 1, $date_arr[2]);
  if ($query_oc_id && $month_start) {
    $query = production_query_get_by_oc_id($query_oc_id, $month_start);
  }

  // если нет Заявки, создать
  if (!$query) {
    $query = entity_create('query', [
      'type' => 'query',
      'created' => $query_import_time,
      'uid' => 0,
    ]);
    entity_save('query', $query);
  }

  // подготовить поля
  $query_comment = empty($array['comment']) ? '' : $array['comment'];
  $query_period_value = empty($array['period_begin']) ? 0 : strtotime($array['period_begin']);
  $query_period_value2 = empty($array['period_end']) ? 0 : strtotime($array['period_end']);

  $query_reason_tid = 0;
  $query_reason_1c = empty($array['reason_1c']) ? '' : $array['reason_1c'];
  if ($query_reason_1c && $query_reason = production_reason_get_by_oc_id($query_reason_1c)) {
    $query_reason_tid = $query_reason->tid;
  }

  $has_approved_plan = production_query_approved_plan_get($month_start);
  if ($has_approved_plan && $array['type'] == 'pred') {
    watchdog('import', 'Импорт занятости. Попытка импорта предварительной заявки при наличии утверждённого плана. (%query_oc_id)', ['%query_oc_id' => $query_oc_id],WATCHDOG_ERROR);

    // уведомить об ошибке для устранения причин
    drupal_mail('system', 'mail', 'mega_comp@mail.ru', language_default(), array(
      'context' => array(
        'subject' => 'Производство Агрохимикат. Ошибка импорта',
        'message' => 'Импорт занятости. Попытка импорта предварительной заявки при наличии утверждённого плана ' . $array['month'] . '.',
      )
    ));

    return false;
  }

  // заполнить поля
  try {
    $query_wr = entity_metadata_wrapper('query', $query);
    $query_wr->changed = $query_import_time;
    $query_wr->field_month = $month_start;
    $query_wr->field_nomenklatura = $nom->tid;
    $query_wr->field_query_type = $array['type'] == 'pred'? 'request' : 'correction';
    $query_wr->field_qty = $query_qty;
    $query_wr->field_query_status_client = $has_approved_plan ? 'approved' : 'processing';
    $query_wr->field_query_status_manager = $query_status_manager;
    if ($query_reason_tid) $query_wr->field_query_reason = $query_reason_tid;
    $query_wr->field_comment = $query_comment;
    $query_wr->field_oc_id = $query_oc_id;
    $query_wr->field_query_period = ['value' => $query_period_value, 'value2' => $query_period_value2 ];
    $query_wr->field_load_json = drupal_json_encode($load_arr);
    $query_wr->save();

    // уведомления по Установкам
    // удалить старые, на случай, если правим существующую
    foreach($query_wr->field_query_notifications->getIterator() as $old_notif_wr) {
      $old_notif_wr->delete();
    }
    $query_wr->field_query_notifications->set([]);

    // зададим новые
    if ($array['notification']) {
      foreach ($array['notification'] as $notification) {
        if ($produce_unit = production_produce_unit_get_by_1cID($notification['unit_id_1c'])) {
          $notification_field = entity_create('field_collection_item', ['field_name' => 'field_query_notifications']);
          $notification_field_wr = entity_metadata_wrapper('field_collection_item', $notification_field);
          $notification_field->setHostEntity('query', $query);
          $notification_field_wr->field_qn_produce_unit->set($produce_unit->tid);
          $notification_field_wr->field_qn_notification->set($notification['message']);
          $notification_field_wr->save();
        }
      }
    }

  } catch (Exception $e) {
    watchdog('import', 'Ошибка при сохранении заявки. %mess', ['%mess' => $e->getMessage()],WATCHDOG_ERROR);
    return false;
  }

  // если пришёл отказ, сделать копию последней актуальной заявки (созданной до отказа)
  // тк актуальная заявка должна быть всегда сверху
  if ($query_status_manager == 'deny') {
    $efquery = new EntityFieldQuery();
    $efquery
      ->entityCondition('entity_type', 'query')
      ->propertyCondition('created', $query_wr->created->value(), '<')
      ->fieldCondition('field_month', 'value', $month_start)
      ->fieldCondition('field_nomenklatura', 'target_id', $nom->tid)
      ->fieldCondition('field_query_status_client', 'value', 'approved')
      ->fieldCondition('field_query_status_manager', 'value', 'approved')
      ->propertyOrderBy('id', 'DESC');
    $result = $efquery->execute();
    if ($result) {
      production_query_make_copy(current($result['query']));
    }
  }

  // если пришла Занятость
  if (!empty($array['service'])) {
    $service_load_arr = [];
    foreach($array['service'] as $item) {
      if ($produce_unit = production_produce_unit_get_by_1cID($item['unit_id_1c'])) {
        $date_arr = explode('.', $item['date']);
        $day_start = gmmktime(0, 0, 0, $date_arr[1], $date_arr[0], $date_arr[2]);
        if (!isset($service_load_arr[$produce_unit->tid])) $service_load_arr[$produce_unit->tid] = [
          'pu_tid' => $produce_unit->tid,
          'pu_load' => [],
        ];
        $service_load_arr[$produce_unit->tid]['pu_load'][$day_start] = 0;
      } else {
        watchdog('import', 'Импорт занятости. Не найдена установка "%unit_id_1c". (%query_oc_id)', ['%unit_id_1c' => $item['unit_id_1c'], '%query_oc_id' => $query_oc_id],WATCHDOG_ERROR);
        //return false;
      }
    }

    if ($service_load_arr) {
      $query_zan = entity_create('query', [
        'type' => 'query',
        'created' => $query_import_time,
        'uid' => 0,
      ]);
      entity_save('query', $query_zan);

      $query_zan_wr = entity_metadata_wrapper('query', $query_zan);
      $query_zan_wr->field_month = $month_start;
      $query_zan_wr->field_nomenklatura = TID_SERVICE_WORK; // tid Технических работ
      $query_zan_wr->field_query_type = 'service';
      $query_zan_wr->field_load_json = drupal_json_encode($service_load_arr);
      $query_zan_wr->save();
    }
  }

  // сразу уведомить клиента о новой заявке, если не установлена отправка раз в сутки
  try {
    $params = [
      'qid' => $query_wr->getIdentifier(),
      'month' => t(date('F', $query_wr->field_month->value())),
      'year' => date('Y', $query_wr->field_month->value()),
      'product' => $query_wr->field_nomenklatura->name->value(),
      'volume' => $query_wr->field_qty->value(),
      'product_owner_account' => $query_wr->field_nomenklatura->field_user->value(),
      'status_client' => $query_wr->field_query_status_client->label(),
      'status_manager' => $query_wr->field_query_status_manager->label(),
      'client_approve' => $query_wr->field_query_client_approve->value(),
      'type' => $query_wr->field_query_type->value(),
      'reason' => $query_wr->field_query_reason->value() ? $query_wr->field_query_reason->name->value() : '',
      'comment' => $query_wr->field_comment->value(),
    ];

    // Уведомления по Установкам
    $params['notification'] =  '';
    if ($query_wr->field_query_notifications->count()) {
      $params['notification'] =  '<br />ВНИМАНИЕ!<br />';
      foreach($query_wr->field_query_notifications->getIterator() as $notif_wr) {
        $notif_arr[$notif_wr->field_qn_produce_unit->tid->value()] = [
          'name' => $notif_wr->field_qn_produce_unit->name->value(),
          'message' => $notif_wr->field_qn_notification->value(),
        ];
        $params['notification'] .=  '<strong>' . $notif_wr->field_qn_produce_unit->name->value() . '</strong>. ' . $notif_wr->field_qn_notification->value() . '<br />';
      }
    }

    if ($users = ext_user_managers_get_by_company_id($params['product_owner_account']->uid)) {
      foreach ($users as $manager_account) {
        if (empty($manager_account->field_notify_once_a_day["und"][0]["value"])) {
          drupal_mail('mailer', 'client_notify_query_correction_new', $manager_account->mail, user_preferred_language($manager_account), $params);
        }
      }
    }

  } catch (Exception $e) {
    watchdog('mail', 'Ошибка при отправке письма заявки. %mess', ['%mess' => $e->getMessage()],WATCHDOG_ERROR);
    return false;
  }

  return true;
}
