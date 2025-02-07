<?php

function prod_client_import($array)
{
  if (is_array($array) && !empty($array['load'])) {
    $vid = taxonomy_vocabulary_machine_name_load('clients')->vid;
    $got = [];
    foreach ($array['load'] as $item) {
      if ($item['dav_id'] && $item['dav_name']) {
        $client = prod_company_get_by_1c_id($item['dav_id']);

        $client_wr = entity_metadata_wrapper('taxonomy_term', $client);
        $client_wr->name = $item['dav_name'];
        $client_wr->field_status = 1;
        $client_wr->save();

        $got[] = $client_wr->getIdentifier();
      }
    }

    // если полное обновление справочника, то запомнить отсутствующие записи и сделать их неактивными
    if ($array['reload']) {
      $dbr = db_select('taxonomy_term_data', 't');
      $dbr->condition('t.vid', $vid);
      $dbr->addField('t', 'tid');
      $client_old = $dbr->execute()->fetchAllAssoc('tid');

      foreach ($client_old as $tid => $client) {
        if (!in_array($tid, $got)) {
          $term_wr = entity_metadata_wrapper('taxonomy_term', $tid);
          $term_wr->field_status = 0;
          $term_wr->save();
        }
      }
    }

    // drupal_set_message('Клиенты импортированы.');
    watchdog('import', 'Клиенты импортированы.', [], WATCHDOG_INFO);
    return true;
  } else {
    // drupal_set_message('Импорт клиентов не выполнен.', 'warning');
    watchdog('import', 'Импорт клиентов не выполнен.', [], WATCHDOG_ERROR);
    return false;
  }
}

function prod_produce_units_import($array)
{
  if (is_array($array) && !empty($array['load'])) {
    $vid = taxonomy_vocabulary_machine_name_load('produce_units')->vid;

    $punits_got = [];
    foreach($array['load'] as $key => $punit) {
        if ($punit['ust_id'] && $punit['ust_name']) {
          $produce_unit = prod_produce_unit_get_by_1c_id($punit['ust_id']);

          // обновить значения полей
          $produce_unit_wr = entity_metadata_wrapper('taxonomy_term', $produce_unit);
          $produce_unit_wr->name = $punit['ust_name'];
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

    // drupal_set_message('Установки импортированы.');
    watchdog('import', 'Установки импортированы.', [], WATCHDOG_INFO);
    return true;
  } else {
    // drupal_set_message('Импорт установок не выполнен.', 'warning');
    watchdog('import', 'Импорт установок не выполнен.', [], WATCHDOG_ERROR);
    return false;
  }
}

function prod_changes_import($array)
{
  if (is_array($array) && !empty($array['load'])) {
    $vid = taxonomy_vocabulary_machine_name_load('changes')->vid;

    // если полное обновление справочника, очистить справочник
    if ($array['reload'] && $array['reload'] != 'false') {
      $dbr = db_select('taxonomy_term_data', 't');
      $dbr->condition('t.vid', $vid);
      $dbr->addField('t', 'tid');
      $items_old = $dbr->execute()->fetchCol();
      entity_delete_multiple('taxonomy_term', $items_old);
    }

    foreach($array['load'] as $item) {
      if ($item['id'] && $item['name']) {
        $change = prod_change_get_by_1c_id($item['id']);

        // обновить значения полей
        $change_wr = entity_metadata_wrapper('taxonomy_term', $change);
        $change_wr->name = $item['name'];
        $change_wr->save();
      }
    }

    // drupal_set_message('Причины импортированы.');
    watchdog('import', 'Изменения импортированы.', [], WATCHDOG_INFO);
    return true;
  } else {
    // drupal_set_message('Импорт справочника причин не выполнен.', 'warning');
    watchdog('import', 'Импорт справочника изменений не выполнен.', [], WATCHDOG_ERROR);
    return false;
  }
}

function prod_reasons_import($array)
{
  if (is_array($array) && !empty($array['load'])) {
    $vid = taxonomy_vocabulary_machine_name_load('reasons')->vid;

    $got = [];
    foreach($array['load'] as $key => $item) {
      if ($item['cause_id'] && $item['cause_name']) {
        $reason = prod_reason_get_by_1c_id($item['cause_id']);

        if (!is_numeric($item['cause_fault'])) {
          $item['cause_fault'] = 0;
        }

        // обновить значения полей
        $reason_wr = entity_metadata_wrapper('taxonomy_term', $reason);
        $reason_wr->name = $item['cause_name'];
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

    // drupal_set_message('Причины импортированы.');
    watchdog('import', 'Причины импортированы.', [], WATCHDOG_INFO);
    return true;
  } else {
    // drupal_set_message('Импорт справочника причин не выполнен.', 'warning');
    watchdog('import', 'Импорт справочника причин не выполнен.', [], WATCHDOG_ERROR);
    return false;
  }
}

function prod_nomenklatura_import($array)
{
  if (is_array($array) && !empty($array['load'])) {
    $vid = taxonomy_vocabulary_machine_name_load('nomenklatura')->vid;

    $got = [];
    foreach($array['load'] as $nom) {
      if (($nom['art'] || !empty($nom['id'])) && $nom['name'] && $nom['dav_id']) {
        // найти пользователя
        $company_id = prod_company_get_by_1c_id($nom['dav_id']);
        // проверить наличие записи Номенклатуры
        if (empty($nom['id'])) $nom['id'] = '';
        $term = prod_nomenklatura_get_by_1c_params($nom['id'], $nom['art']);

        // обновить значения полей
        $term_wr = entity_metadata_wrapper('taxonomy_term', $term);
        $term_wr->name = $nom['name'];
        $term_wr->field_oc_artikul = $nom['art'];
        $term_wr->field_oc_name_davalec = $nom['nnd'];
        $term_wr->field_oc_artikul_davalec = $nom['anr'];
        $term_wr->field_client = $company_id;
        $term_wr->field_status = 1;
        $term_wr->save();

        $got[] = $term_wr->getIdentifier();
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

    // drupal_set_message('Номенклатура импортирована.');
    watchdog('import', 'Номенклатура импортирована.', [], WATCHDOG_INFO);
    return true;
  } else {
    // drupal_set_message('Импорт номенклатуры не выполнен.', 'warning');
    watchdog('import', 'Импорт номенклатуры не выполнен.', [], WATCHDOG_ERROR);
    return false;
  }
}

function prod_plan_import($array)
{
  if (is_array($array) && !empty($array['load'])) {
    $month_start = helper_datetime_to_timestamp($array['month']);

    //  если есть утверждённый план, импорт запретить
    if (prod_plan_has_approved($month_start)) {
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

    // подготовить массив с Планом для внесения в базу
    // определить Заявки и посчитать общее производимое количество
    $plan = [];
    foreach ($array['load'] as $item) {
      if (!empty($item['nom_id']) && $item['unit_id_1c'] && $item['date']) {
        $nom          = prod_nomenklatura_get_by_1c_params($item['nom_id']);
        $produce_unit = prod_produce_unit_get_by_1c_id($item['unit_id_1c']);
        $day_start    = helper_datetime_to_timestamp($item['date']);
        $client_id    = $nom->field_client['und'][0]['tid'];

        if (!isset($plan[$nom->tid])) {
          $plan[$nom->tid] = [
            'month_ts' => $month_start,
            'nom_id' => $nom->tid,
            'client_id' => $client_id,
            'volume' => 0,
            'date' => 0,
            'load' => [],
          ];
        }
        if (!isset($plan[$nom->tid]['load'][$produce_unit->tid])) $plan[$nom->tid]['load'][$produce_unit->tid] = [
          'pu_tid' => $produce_unit->tid,
          'pu_load' => [],
        ];

        // определить объём и дату начала
        $plan[$nom->tid]['volume'] += $item['output'];
        if (!$plan[$nom->tid]['date'] || $plan[$nom->tid]['date'] > $day_start) $plan[$nom->tid]['date'] = $day_start;

        $plan[$nom->tid]['load'][$produce_unit->tid]['pu_load'][$day_start] = $item['output'];
        $plan[$nom->tid]['produce_units'][$produce_unit->tid] = $produce_unit->tid;
      }
    }

    // удалить старый План на месяц, включая сервисную запись
    prod_plan_record_delete_all($month_start);

    // добавление Плана на месяц
    $user_mails = [];
    foreach ($plan as $plan_item) {
      $plan_rec = prod_plan_record_create([
        'import_time_ts' => REQUEST_TIME,
        'month_ts' => $month_start,
        'nom_id' => $plan_item['nom_id'],
        'type' => $array['type'] == 'appr' ? PROD_PLAN_TYPE_APPR : PROD_PLAN_TYPE_PRED,
        'load' => $plan_item['load'],
      ]);

      // при утверждении плана создать Сообщение
      if ($array['type'] == 'appr') {
        prod_message_create([
          'created' => REQUEST_TIME,
          'month_ts' => $month_start,
          'nom_id' => $plan_item['nom_id'],
          'changes' => [PROD_CHANGE_APPROVE],
          'plan_rec' => isset($plan_rec) ? $plan_rec->id : null,
          'author' => PROD_MESSAGE_AUTHOR_SYS_PROD,
          'params' => ['volume' => ['from' => $plan_item['volume'], 'to' => $plan_item['volume']], 'date' => ['from' => $plan_item['date'], 'to' => $plan_item['date']]],
        ]);
      }

      if (empty($user_mails[$plan_item['client_id']])) {
        $user_mails[$plan_item['client_id']] = [
          'month_ts' => $plan_item['month_ts'],
          'client_id' => $plan_item['client_id'],
        ];
      }
    }

    if (!empty($array['service'])) {
      $service_load = [];
      foreach ($array['service'] as $item) {
        $produce_unit = prod_produce_unit_get_by_1c_id($item['unit_id_1c']);
        $day_start = helper_datetime_to_timestamp($item['date']);
        if (!isset($service_load[$produce_unit->tid])) $service_load[$produce_unit->tid] = [
          'pu_tid' => $produce_unit->tid,
          'pu_load' => [],
        ];
        $service_load[$produce_unit->tid]['pu_load'][$day_start] = 0;
      }

      if ($service_load) {
        prod_plan_record_create([
          'import_time_ts' => REQUEST_TIME,
          'month_ts' => $month_start,
          'nom_id' => PROD_SERVICE_NOM_ID,                          // сервисная запись отличается только Номенклатурой
          'type' => $array['type'] == 'appr' ? PROD_PLAN_TYPE_APPR : PROD_PLAN_TYPE_PRED,
          'load' => $service_load,
        ]);
      }
    }

    // отправить письма об импорте Плана
    if ($user_mails) {
      foreach ($user_mails as $data) {
        $params['month'] = t(date('F', $data['month_ts']));
        $params['year'] = date('Y', $data['month_ts']);
        try {
          if ($users = ext_user_get_managers_by_client_id($data['client_id'], ROLE_USER_PRODUCTION)) {
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

/**
 * Импортировать входящее Сообщение
 *
 * @param $array - массив данных
 * @return bool - результат выполнения
 */
function prod_message_import($array)
{
  $parent_id      = empty($array['ID']) ? 0 : $array['ID'];
  $message_id_1c  = empty($array["ID_1c"]) ? '' : $array["ID_1c"];
  $created        = empty($array["created"]) ? REQUEST_TIME : helper_datetime_to_timestamp($array['created']);
  $nom_id_1c      = empty($array['nom_id']) ? '' : $array['nom_id'];
  $nom_artikul    = empty($array['artikul_1c']) ? '' : $array['artikul_1c'];
  $warning        = empty($array['warning']) || $array['warning'] === "false" ? null : PROD_WARNING_RED;
  $comment        = empty($array['comment']) ? '' : $array['comment'];
  $month_start  = helper_datetime_to_timestamp($array['month']);
  $nom_id       = prod_nomenklatura_get_by_1c_params($nom_id_1c, $nom_artikul)->tid;
  $reason       = prod_reason_get_by_1c_id($array['reason_id']);

  // получить Загрузку, общий Объём и Дату начала выпуска
  $load = [];
  $volume = $date = 0;
  if (!empty($array['load'])) {
    foreach($array['load'] as $item) {
      $produce_unit = prod_produce_unit_get_by_1c_id($item['unit_id_1c']);
      $day_start = helper_datetime_to_timestamp($item['date']);
      if (!isset($load[$produce_unit->tid])) $load[$produce_unit->tid] = [
        'pu_tid' => $produce_unit->tid,
        'pu_load' => [],
      ];
      $volume += $item['output'];
      $output = $item['output'];
      $load[$produce_unit->tid]['pu_load'][$day_start] = $output;
      if (!$date || $date > $day_start) $date = $day_start;
    }
  }

  // определить, какие произошли Изменения
  $changes = [];
  $volume_from = $date_from = null;
    // предыдущая Запись плана
  if ($plan_rec_prev = prod_plan_record_get_last($month_start, $nom_id)) {
    $plan_rec_prev_info = prod_plan_record_get_info($plan_rec_prev->id);
  }
    // если нет Записей плана, то это Включение в план или Отказ
  if (empty($plan_rec_prev_info)) {
    if ($load)              $changes[] = PROD_CHANGE_ADD_TO_PLAN;             // внесение в план
    // если по инициативе клиента
    elseif($reason->tid == 553) {
                            $changes[] = PROD_CHANGE_CANCEL;                  // отмена
    } else                  $changes[] = PROD_CHANGE_DENY;                    // отказ
  }
    // если есть Записи плана
  else {
    $volume_from = $plan_rec_prev_info['volume'];
    $date_from = $plan_rec_prev_info['date'];
    if ($volume == 0) {
                            $changes[] = PROD_CHANGE_CANCEL;                  // отмена выпуска
    } else {
      if ($plan_rec_prev_info['volume'] == 0) {
                            $changes[] = PROD_CHANGE_ADD_TO_PLAN;             // снова внесение в план
      }
      else {
        if ($plan_rec_prev_info['volume'] != $volume) {
                            $changes[] = PROD_CHANGE_VOLUME;                  // изменение объёма
        }
        // вместе с изменением Объёма может быть изменение Даты или Графика
        if ($date) {
          if ($plan_rec_prev_info['date'] != $date) {
                            $changes[] = PROD_CHANGE_DATE;                   // изменение даты начала
          }
          elseif ($plan_rec_prev_info['load'] != $load) {
                            $changes[] = PROD_CHANGE_SCHEDULE;               // изменение графика выпуска
          }
        }
      }
    }
  }
  if (!$changes) {
    watchdog('import', 'Импорт Сообщения. Не определено Изменение Сообщение %message_id_1c', ['%message_id_1c' => $message_id_1c],WATCHDOG_ERROR);
  }

  // сохранить изменения объёма и даты в массиве параметров
  $params['volume'] = ['from' => $volume_from, 'to' => $volume];
  $params['date'] = ['from' => $date_from, 'to' => $date];

  // создать записи плана и сообщения
  try {
    $plan_rec = null;
    if (prod_plan_has_approved($month_start) && $array['type'] == 'pred') {
      watchdog('import', 'Импорт плана. Попытка импорта предварительного плана при наличии утверждённого. Сообщение %message_id_1c', ['%message_id_1c' => $message_id_1c],WATCHDOG_ERROR);

      // уведомить об ошибке для устранения причин
      drupal_mail('system', 'mail', 'mega_comp@mail.ru', language_default(), array(
        'context' => array(
          'subject' => 'Производство Агрохимикат. Ошибка импорта',
          'message' => 'Импорт занятости. Попытка импорта предварительной заявки при наличии утверждённого плана ' . $array['month'] . '.',
        )
      ));

    // todo добавить в комментарий, что план не изменён (о чём это?)

    } else {
      // в заявках план или Предварительный или Оперативный
      // Утверждённый приходит только в файле плана
      $plan_rec = prod_plan_record_create([
        'import_time_ts' => REQUEST_TIME,
        'month_ts' => $month_start,
        'nom_id' => $nom_id,
        'type' => $array['type'] == 'pred' ? PROD_PLAN_TYPE_PRED : PROD_PLAN_TYPE_OPER,
        'load' => $load,
      ]);
    }

    // Так как нужно подсвечивать, с чьей стороны возник риск срыва,
    // создать системное сообщение для клиента для следующих причин:
    //  - отсутствие сырья
    //  - отсутствие материалов
    if ($reason && in_array($reason->tid, [551, 552])) {
      prod_message_create([
        'created' => $created-1,   // сообщение должно размещаться до сообщения от производства
        'month_ts' => $month_start,
        'nom_id' => $nom_id,
        'comment' => $reason->name,
        'author' => PROD_MESSAGE_AUTHOR_SYS_CLIENT,
        'warning' => PROD_WARNING_RED,
      ]);
    }

    prod_message_create([
      'created' => $created,
      'month_ts' => $month_start,
      'nom_id' => $nom_id,
      'comment' => $comment,
      '1c_id' => $message_id_1c,
      'parent_id' => !empty($parent_id) ? $parent_id : null,
      'changes' => !empty($changes) ? $changes : [],
      'reason' => !empty($reason) ? $reason->tid : null,
      'plan_rec' => !empty($plan_rec) ? $plan_rec->id : null,
      'author' => PROD_MESSAGE_AUTHOR_PROD,
      'params' => $params,
      'warning' => !empty($warning) ? $warning : null,
    ]);

    // если пришла Занятость
    if (!empty($array['service'])) {
      $service_load = [];
      foreach($array['service'] as $item) {
        $produce_unit = prod_produce_unit_get_by_1c_id($item['unit_id_1c']);
        $day_start = helper_datetime_to_timestamp($item['date']);
        if (!isset($service_load[$produce_unit->tid])) $service_load[$produce_unit->tid] = [
          'pu_tid' => $produce_unit->tid,
          'pu_load' => [],
        ];
        $service_load[$produce_unit->tid]['pu_load'][$day_start] = 0;
      }

      if ($service_load) {
        // удалить старую сервисную запись
        // история плана не нужна, поэтому достаточно текущей записи.
        prod_plan_record_delete_service($month_start);

        prod_plan_record_create([
          'import_time_ts' => REQUEST_TIME,
          'month_ts' => $month_start,
          'nom_id' => PROD_SERVICE_NOM_ID,                          // сервисная запись отличается только Номенклатурой
          'type' => $array['type'] == 'pred' ? PROD_PLAN_TYPE_PRED : PROD_PLAN_TYPE_OPER,
          'load' => $service_load,
        ]);
      }
    }
  } catch (Exception $e) {
    watchdog('import', 'Ошибка при сохранении сообщения %message_id_1c. %errmess', ['%errmess' => $e->getMessage(), '%message_id_1c' => $message_id_1c],WATCHDOG_ERROR);
    return false;
  }


  // todo переделать
  //      сразу уведомить клиента о новой заявке, если не установлена отправка раз в сутки
  if (false && $message) {
    $params = [
      'qid' => $message_wr->getIdentifier(),
      'month' => t(date('F', $message_wr->field_month->value())),
      'year' => date('Y', $message_wr->field_month->value()),
      'product' => $message_wr->field_nomenklatura->name->value(),
      'volume' => $message_wr->field_qty->value(),
      'product_owner_id' => $message_wr->field_nomenklatura->field_client->getIdentifier(),
      'type' => $message_wr->field_query_type->value(),
      'reason' => $message_wr->field_query_reason->value() ? $message_wr->field_query_reason->name->value() : '',
      'comment' => $message_wr->field_comment->value(),
    ];

    if ($users = ext_user_get_managers_by_client_id($params['product_owner_id'], ROLE_USER_PRODUCTION)) {
      foreach ($users as $manager_account) {
        if (empty($manager_account->field_notify_once_a_day["und"][0]["value"])) {
          drupal_mail('mailer', 'client_notify_query_correction_new', $manager_account->mail, user_preferred_language($manager_account), $params);
        }
      }
    }
  }

  return true;
}
