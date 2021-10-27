<?php

function query_import_form($form, &$form_state)
{
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type'  => 'submit',
    '#name'  => 'delete',
    '#value' => 'Сабмит',
  );

//  $form['#submit'][] = 'query_import_submit';

  return $form;
}


function query_delete_form($form, &$form_state)
{
  $form['month'] = array(
    '#type' => 'date_select',
    '#title' => t('Sample from'),
    '#date_format' => 'm.Y',
    '#default_value' => array(
      'month' => date('n'),
      'year' => date('Y'),
    ),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['delete'] = array(
    '#type'  => 'submit',
    '#name'  => 'delete',
    '#value' => 'Удалить заявки за месяц',
  );

  $form['actions']['delete_all'] = array(
    '#type'  => 'submit',
    '#name'  => 'delete_all',
    '#value' => 'Удалить все заявки',
  );

  $form['#submit'][] = 'query_delete_submit';

  return $form;
}

/**
 * Form submit callback.
 */
function query_delete_submit($form, &$form_state)
{
  if ($form_state["triggering_element"]["#name"] == 'delete') {
    $date_array = explode('-', $form_state["values"]["month"]);
    $month_start = gmmktime(0, 0, 0, $date_array[1], 1, $date_array[0]);
    query_queries_delete($month_start);
  } elseif ($form_state["triggering_element"]["#name"] == 'delete_all') {
    query_queries_delete();
  }
}

function query_plan_delete($month_start, $delete_approved = false)
{
  if ($month_start) {
    $efquery = new EntityFieldQuery();
    $efquery
      ->entityCondition('entity_type', 'query')
      ->fieldCondition('field_month', 'value', $month_start)
      ->fieldCondition('field_query_type', 'value', 'main');

    if (!$delete_approved) {
      $efquery->fieldCondition('field_query_status_client', 'value', 'approved', '<>');
    }

    $result = $efquery->execute();
    if ($result) {
      entity_delete_multiple('query', array_keys($result['query']));
    }

    // техработы теперь в плане
//    // вычистить технические работы
//    $next_month_start = gmmktime(0,0,0, date('n', $month_start)+1, date('j', $month_start), date('Y', $month_start));
//    $efquery = new EntityFieldQuery();
//    $result = $efquery
//      ->entityCondition('entity_type', 'load')
//      ->fieldCondition('field_produce_date', 'value', $month_start, '>=')
//      ->fieldCondition('field_produce_date', 'value', $next_month_start, '<')
//      ->fieldCondition('field_produce_output', 'value', 0)
//      ->execute();
//    if ($result) {
//      entity_delete_multiple('load', array_keys($result['load']));
//    }
  }
}

function query_queries_delete($month_start = null)
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

//function query_produce_service_import($array)
//{
//  if (is_array($array) && !empty($array['load'])) {
//    // удалить занятость месяца
//    $date_array = explode('.', $array['load'][0]['date']);
//    $month_start = gmmktime(0,0,0, $date_array[1], 1, $date_array[2]);
//    $next_month_start = gmmktime(0,0,0, date('n', $month_start)+1, 1, date('Y', $month_start));
//    $efquery = new EntityFieldQuery();
//    $result = $efquery
//      ->entityCondition('entity_type', 'load')
//      ->fieldCondition('field_produce_date', 'value', $month_start, '>=')
//      ->fieldCondition('field_produce_date', 'value', $next_month_start, '<')
//      ->fieldCondition('field_produce_output', 'value', 0)
//      ->execute();
//    if ($result) {
//      entity_delete_multiple('load', array_keys($result['load']));
//    }
//
//    foreach($array['load'] as $key => $buzy) {
//      if ($buzy['zan'] == 2) {
//        $produce_unit = query_get_produce_unit_by_1cID($buzy['ust_id']);
//        $date_array = explode('.', $buzy['date']);
//        $day_start = gmmktime(0, 0, 0, $date_array[1], $date_array[0], $date_array[2]);
//
//        $load = entity_create('load', ['type' => 'load']);
//        entity_save('query', $load);
//
//        $load_wr = entity_metadata_wrapper('load', $load);
//        $load_wr->field_produce_unit = $produce_unit->tid;
//        $load_wr->field_produce_date = $day_start;
//        $load_wr->field_produce_output = 0;
//        $load_wr->save();
//      }
//    }
//
//    drupal_set_message('Занятость установок импортирована.');
//    watchdog('import', 'Занятость установок импортирована.', [], WATCHDOG_INFO);
//    return true;
//  } else {
//    drupal_set_message('Импорт занятости установок не выполнен.', 'warning');
//    watchdog('import', 'Импорт занятости установок не выполнен.', [], WATCHDOG_ERROR);
//    return false;
//  }
//}


function query_davalec_import($array)
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

function query_produce_units_import($array)
{
  if (is_array($array) && !empty($array['load'])) {
    $vid = taxonomy_vocabulary_machine_name_load('produce_units')->vid;

    $punits_got = [];
    foreach($array['load'] as $key => $punit) {
        if ($punit['ust_id'] && $punit['ust_name']) {
          $produce_unit = query_get_produce_unit_by_1cID($punit['ust_id']);

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

function query_reasons_import($array)
{
  if (is_array($array) && !empty($array['load'])) {
    $vid = taxonomy_vocabulary_machine_name_load('reasons')->vid;

    $got = [];
    foreach($array['load'] as $key => $item) {
      if ($item['cause_id'] && $item['cause_name']) {
        $reason = query_get_reason_by_oc_id($item['cause_id']);

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

function query_nomenklatura_import($array)
{
  if (is_array($array) && !empty($array['load'])) {
    $vid = taxonomy_vocabulary_machine_name_load('nomenklatura')->vid;

    $got = [];
    foreach($array['load'] as $key => $nom) {
        if ($nom['name'] && $nom['art'] && $nom['dav_id']) {
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

          // найти пользователя
          $query = new EntityFieldQuery();
          $users = $query
            ->entityCondition('entity_type', 'user')
            ->fieldCondition('field_oc_davalec_id', 'value', $nom['dav_id'])
            ->execute();
          if ($users) {
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
            watchdog('import', 'Импорт номенклатуры не выполнен. Не найден давалец с ID: ' . $nom['dav_id'], [], WATCHDOG_ERROR);
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

function query_produce_plan_import($array)
{
  if (is_array($array) && !empty($array['load'])) {
    // подготовить массив с Планом для внесения в базу
    // определить Заявки и посчитать общее производимое количество
    $plan = [];
    $month_arr = explode('.', $array['month']);
    $month_start = gmmktime(0, 0, 0, $month_arr[1], 1, $month_arr[2]);
    $plan_import_time = time();

    //  если есть утверждённый план, импорт запретить
    $efquery = new EntityFieldQuery();
    $efquery
      ->entityCondition('entity_type', 'query')
      ->fieldCondition('field_month', 'value', $month_start)
      ->fieldCondition('field_query_type', 'value', 'main')
      ->fieldCondition('field_query_status_client', 'value', 'approved')
      ->fieldCondition('field_query_status_manager', 'value', 'approved');
    $result = $efquery->execute();
    if ($result) {
      watchdog('import', 'Утверждённый план уже загружен. Импорт плана не выполнен.', [], WATCHDOG_ERROR);
      return false;
    }

    $produce_unit_id_arr = [];
    foreach ($array['load'] as $key => $item) {
      if ($item['artikul_1c'] && $item['unit_id_1c'] && $item['date']) {
        if (($nom = query_get_nomenklatura_by_artikul($item['artikul_1c']))
          && ($produce_unit = query_get_produce_unit_by_1cID($item['unit_id_1c']))) {
          $date_arr = explode('.', $item['date']);
          $day_start = gmmktime(0, 0, 0, $date_arr[1], $date_arr[0], $date_arr[2]);

          if (!isset($plan[$nom->tid])) {
            $plan[$nom->tid] = [
              'field_month' => $month_start,
              'field_nomenklatura' => $nom->tid,
              'uid' => $nom->field_user['und'][0]['target_id'],
              'field_qty' => 0,
              'field_load' => [],
            ];
          }
          $plan[$nom->tid]['field_qty'] += $item['output'];
          $plan[$nom->tid]['field_load'][] = [
            'field_produce_unit' => $produce_unit->tid,
            'field_produce_date' => $day_start,
            'field_produce_output' => $item['output'],
          ];

          $plan[$nom->tid]['produce_units'][$produce_unit->tid] = $produce_unit->tid;
          $produce_unit_id_arr[$item['unit_id_1c']] = $produce_unit->tid;

        } else {
          if (empty($nom)) {
            watchdog('import', 'Импорт плана. Не найдена номенклатура с 1с ID: ' . $item['artikul_1c'], [], WATCHDOG_ERROR);
          }
          if (empty($produce_unit)) {
            watchdog('import', 'Импорт плана. Не найдена установка с 1с ID: ' . $item['unit_id_1c'], [], WATCHDOG_ERROR);
          }
//          return false; продолжаем импорт
        }
      }
    }

    // удалить старый план на месяц
    query_plan_delete($month_start);

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
      $query_wr->field_query_type = 'main';
      $query_wr->field_query_status_client = ($array['type'] == 'main' ? 'approved' : 'processing');
      $query_wr->field_query_status_manager = 'approved';
      $query_wr->field_query_period = ['value' => 0, 'value2' => 0];
      $query_wr->save();

      query_query_load_update($query->id, $plan_item['field_load']);

      if (empty($user_mails[$plan_item['uid']])) {
        $user_mails[$plan_item['uid']] = [
          'month' => $plan_item['field_month'],
          'company_uid' => $plan_item['uid']
        ];
      }
    }

    if (!empty($array['service'])) {
      $service_arr = [];
      foreach ($array['service'] as $item) {
        if ($produce_unit = query_get_produce_unit_by_1cID($item['unit_id_1c'])) {
          $date_arr = explode('.', $item['date']);
          $day_start = gmmktime(0, 0, 0, $date_arr[1], $date_arr[0], $date_arr[2]);
          $service_arr[] = [
            'field_produce_unit' => $produce_unit->tid,
            'field_produce_date' => $day_start,
            'field_produce_output' => 0,
          ];
        } else {
          watchdog('import', 'Импорт занятости при импорте плана. Не найдена установка "' . $item['unit_id_1c'] . '". ', [], WATCHDOG_ERROR);
          //return false;
        }
      }

      if ($service_arr) {
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
        $query_zan_wr->save();

        query_query_load_update($query_zan->id, $service_arr);
      }
    }

    // отправить письма об импорте плана
    if ($user_mails) {
      foreach ($user_mails as $user_id => $data) {
        $params['month'] = t(date('F', $data['month']));
        try {
          if ($users = ext_user_get_managers_by_company_uid($data['company_uid'])) {
            foreach ($users as $manager_account) {
              if ($manager_account->mail && empty($manager_account->field_notify_once_a_day["und"][0]["value"])) {
                drupal_mail('mailer', 'user_notify_plan_import', $manager_account->mail, user_preferred_language($manager_account), $params);
              }
            }
          }
        } catch (Exception $e) {
          watchdog('mail', 'Ошибка при отправке письма заявки. ' . $e->getMessage(), [],WATCHDOG_ERROR);
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

function query_drivers_import($array)
{
  if (is_array($array) && !empty($array['load'])) {
    $vid = taxonomy_vocabulary_machine_name_load('drivers')->vid;

    $drivers_got = [];
    foreach($array['load'] as $key => $drv) {
      $driver = null;
      if ($drv['driver_id'] || $drv['driver_1c_id']) {

        if ($drv['driver_id']) {
          $driver = taxonomy_term_load($drv['driver_id']);
        }
        if (!$driver) {
          $driver = query_get_driver_by_1cID($drv['driver_1c_id']);
        }

        if (!$driver) {
          $driver = (object)array(
            'vid' => $vid,
            'name' => $drv['licence'],
          );
          taxonomy_term_save($driver);
        }

        // обновить значения полей
        $driver_wr = entity_metadata_wrapper('taxonomy_term', $driver);
        $driver_wr->name = $drv['licence'];
        $driver_wr->field_oc_driver_id = $drv['driver_1c_id'];
        $driver_wr->field_truck_driver_surname = $drv['surname'];
        $driver_wr->field_truck_driver_name = $drv['name'];
        $driver_wr->field_truck_driver_name2 = $drv['name2'];
        $driver_wr->field_truck_driver_license = $drv['licence'];
        $driver_wr->field_dopog_license = $drv['dopog'];
        $dopog_valid_arr = explode('.', $array['dopog_valid']);
        $dopog_valid = gmmktime(0, 0, 0, $dopog_valid_arr[1], $dopog_valid_arr[0], $dopog_valid_arr[2]);
        $driver_wr->field_dopog_license_validity = $dopog_valid;
        $driver_wr->field_status = 1;
        $driver_wr->save();

        $drivers_got[] = $driver_wr->getIdentifier();
      }
    }

    // если полное обновление справочника, то запомнить отсутствующие записи и сделать их неактивными для клиента
    if ($array['reload']) {
      $dbr = db_select('taxonomy_term_data', 't');
      $dbr->condition('t.vid', $vid);
      $dbr->addField('t', 'tid');
      $drivers_old = $dbr->execute()->fetchAllAssoc('tid');

      foreach ($drivers_old as $d_tid => $drv) {
        if (!in_array($d_tid, $drivers_got)) {
          $term_wr = entity_metadata_wrapper('taxonomy_term', $d_tid);
          $term_wr->field_status = 0;
          $term_wr->save();
        }
      }
    }

    drupal_set_message('Водители импортированы.');
    watchdog('import', 'Водители импортированы.', [], WATCHDOG_INFO);
    return true;
  } else {
    drupal_set_message('Импорт водителей не выполнен.', 'warning');
    watchdog('import', 'Импорт водителей не выполнен.', [], WATCHDOG_ERROR);
    return false;
  }
}
