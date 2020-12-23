<?php

function query_import_form($form, &$form_state)
{

  $form['davalec'] = array(
    '#type' => 'managed_file',
    '#title' => 'Справочник давальцев',
    '#description' => 'Выберите файл с расширением csv',
    '#upload_location' => "public://import/",
    '#upload_validators' => array(
      'file_validate_extensions' => array('csv'),
    ),
    '#element_validate' => array('file_managed_file_validate'),
  );

  $form['produce_units'] = array(
    '#type' => 'managed_file',
    '#title' => 'Справочник установок',
    '#description' => 'Выберите файл с расширением csv',
    '#upload_location' => "public://import/",
    '#upload_validators' => array(
      'file_validate_extensions' => array('csv'),
    ),
    '#element_validate' => array('file_managed_file_validate'),
  );

  $form['nomenklatura'] = array(
    '#type' => 'managed_file',
    '#title' => 'Справочник номенклатуры',
    '#description' => 'Выберите файл с расширением csv',
    '#upload_location' => "public://import/",
    '#upload_validators' => array(
      'file_validate_extensions' => array('csv'),
    ),
    '#element_validate' => array('file_managed_file_validate'),
  );

  $form['buzy'] = array(
    '#type' => 'managed_file',
    '#title' => 'Занятость установок',
    '#description' => 'Выберите файл с расширением csv',
    '#upload_location' => "public://import/",
    '#upload_validators' => array(
      'file_validate_extensions' => array('csv'),
    ),
    '#element_validate' => array('file_managed_file_validate'),
  );

  $form['plan'] = array(
    '#type' => 'managed_file',
    '#title' => 'План',
    '#description' => 'Выберите файл с расширением csv',
    '#upload_location' => "public://import/",
    '#upload_validators' => array(
      'file_validate_extensions' => array('csv'),
    ),
    '#element_validate' => array('file_managed_file_validate'),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type'  => 'submit',
    '#name'  => 'import',
    '#value' => 'Импорт',
  );
  $form['actions']['delete'] = array(
    '#type'  => 'submit',
    '#name'  => 'delete',
    '#value' => t('Удалить все заявки'),
  );


  $form['#validate'][] = 'query_import_validate';
  $form['#submit'][] = 'query_import_submit';

  return $form;
}


/**
 * Form validate callback.
 */
function query_import_validate($form, &$form_state)
{
}

/**
 * Form submit callback.
 */
function query_import_submit($form, &$form_state)
{
  if ($form_state['triggering_element']['#name'] == 'import') {
    if (!empty($form_state['values']['davalec'])) {
      $file = file_load($form_state['values']['davalec']);
      $array = file($file->uri);
      query_davalec_import($array);
    }
    if (!empty($form_state['values']['produce_units'])) {
      $file = file_load($form_state['values']['produce_units']);
      $array = file($file->uri);
      query_produce_units_import($array);
    }
    if (!empty($form_state['values']['nomenklatura'])) {
      $file = file_load($form_state['values']['nomenklatura']);
      $array = file($file->uri);
      query_nomenklatura_import($array);
    }

    if (!empty($form_state['values']['buzy'])) {
      $file = file_load($form_state['values']['buzy']);
      $array = file($file->uri);
      query_produce_buzy_import($array);
    }

    if (!empty($form_state['values']['plan'])) {
      $file = file_load($form_state['values']['plan']);
      $array = file($file->uri);
      query_produce_plan_import($array);
    }
  }
  if ($form_state['triggering_element']['#name'] == 'delete') {
    query_plan_delete();
  }
}



function query_plan_delete()
{
  $efquery = new EntityFieldQuery();
  $result = $efquery
    ->entityCondition('entity_type', 'query')
    ->execute();
  if ($result) {
    entity_delete_multiple('query', array_keys($result['query']));
  }

  $efquery = new EntityFieldQuery();
  $result = $efquery
    ->entityCondition('entity_type', 'load')
    ->execute();
  if ($result) {
    entity_delete_multiple('load', array_keys($result['load']));
  }

  drupal_set_message('Заявки удалены.');
}

function query_produce_buzy_import($buzy_array)
{
  if (is_array($buzy_array)) {
    foreach($buzy_array as $key => $buzy) {
      if (!$key) continue;

      if ($buzy = iconv('Windows-1251', 'UTF-8', trim($buzy))) {
        list($produce_unit_1cID, $date_str, $status) = explode('|', $buzy);

        if ($status == 2) {
          $produce_unit = query_get_produce_unit_by_1cID($produce_unit_1cID);
          $date = strtotime($date_str);
          $day_start = gmmktime(0, 0, 0, date('n', $date), date('j', $date), date('Y', $date));

          $efquery = new EntityFieldQuery();
          $result = $efquery
            ->entityCondition('entity_type', 'load')
            ->fieldCondition('field_produce_date', 'value', $day_start)
            ->fieldCondition('field_produce_unit', 'tid', $produce_unit->tid)
            ->execute();

          if (!$result) {
            $load = entity_create('load', ['type' => 'load']);
            entity_save('query', $load);
          } else {
            $load = current($result['load']);
          }

          $load_wr = entity_metadata_wrapper('load', $load);
          $load_wr->field_produce_unit = $produce_unit->tid;
          $load_wr->field_produce_date = $day_start;
          $load_wr->field_produce_output = 0;
          $load_wr->save();
        }
      }
    }
    drupal_set_message('Занятость установок импортирована.');
  } else {
    drupal_set_message('Импорт занятости установок не выполнен.', 'warning');
  }
}

function query_produce_plan_import($plan_array_1c)
{
  if (is_array($plan_array_1c)) {

    // подготовить массив с Планом для внесения в базу
    // определить Заявки и посчитать общее производимое количество
    $plan = [];
    $user_mails = []; // массив с пользователями, кому нужно отправить уведомление об импорте плана
    foreach ($plan_array_1c as $key => $plan_item_1c) {
      if (!$key) continue;

      if ($plan_item_1c = iconv('Windows-1251', 'UTF-8', trim($plan_item_1c))) {
        list($articul, $produce_unit_1cID, $date_str, $output) = explode('|', $plan_item_1c);

        if ($articul && $produce_unit_1cID && $date_str && $output) {
          if (($nom = query_get_nomenklatura_by_artikul($articul))
            && ($produce_unit = query_get_produce_unit_by_1cID($produce_unit_1cID))) {
            $date = strtotime($date_str);
            $month_start = gmmktime(0, 0, 0, date('n', $date), 1, date('Y', $date));
            $day_start = gmmktime(0, 0, 0, date('n', $date), date('j', $date), date('Y', $date));

            if (isset($plan[$nom->tid])) {
              $plan[$nom->tid]['field_qty'] += $output;
              $plan[$nom->tid]['field_load'][] = [
                'field_produce_unit' => $produce_unit->tid,
                'field_produce_date' => $day_start,
                'field_produce_output' => $output,
              ];
            } else {
              $plan[$nom->tid] = [
                'field_month' => $month_start,
                'field_nomenklatura' => $nom->tid,
                'uid' => $nom->field_user['und'][0]['target_id'],
                'field_qty' => $output,
                'field_load' => [
                  [
                    'field_produce_unit' => $produce_unit->tid,
                    'field_produce_date' => $day_start,
                    'field_produce_output' => $output,
                  ]
                ],
              ];
            }
          } else {
            if (empty($nom)) {
              drupal_set_message('Не найдена номенклатура с 1с ID: ' . $articul, 'error');
            }
            if (empty($produce_unit)) {
              drupal_set_message('Не найдена установка с 1с ID: ' . $produce_unit_1cID, 'error');
            }
            return false;
          }
        }
      }
    }

    // создание или обновление Запросов на месяц
    foreach ($plan as $plan_item) {
      // проверить наличие Запроса на месяц даты
      $efquery = new EntityFieldQuery();
      $result = $efquery
        ->entityCondition('entity_type', 'query')
        ->fieldCondition('field_month', 'value', $plan_item['field_month'])
        ->fieldCondition('field_nomenklatura', 'target_id', $plan_item['field_nomenklatura'])
        ->fieldCondition('field_query_type', 'value', 'main')
        ->execute();

      if (!$result) {
        $query = entity_create('query', [
          'type' => 'query',
          'uid' => $plan_item['uid'],
        ]);
        entity_save('query', $query);
      } else {
        $query = current($result['query']);
      }

      $query_wr = entity_metadata_wrapper('query', $query);
      // утверждённые заявки не заменять
      if ($query_wr->field_query_status_client->value() != 'approved' || $query_wr->field_query_status_manager->value() != 'approved') {
        $query_wr->field_month = $plan_item['field_month'];
        $query_wr->field_nomenklatura = $plan_item['field_nomenklatura'];
        $query_wr->field_qty = $plan_item['field_qty'];
        $query_wr->field_query_type = 'main';
        $query_wr->field_query_status_client = 'processing';
        $query_wr->field_query_status_manager = 'approved';
        $query_wr->save();

        query_query_load_update($query->id, $plan_item['field_load']);

        // если импортирована заявка, внести в список пользователей, кому отправить email
        $uid = $query_wr->field_nomenklatura->field_user->uid->value();
        $user_mails[$uid] = [
          'month' => $plan_item['field_month'],
          'account' => $query_wr->field_nomenklatura->field_user->value()
        ];
      }
    }

    // отправить письма об импорте плана
    if ($user_mails) {
      foreach ($user_mails as $user_id => $data) {
        $params['month'] = t(date('F', $data['month']));
        drupal_mail('mailer', 'user_notify_plan_import', $data['account']->mail, user_preferred_language($data['account']), $params);
      }
    }

    drupal_set_message('План производства импортирован.');
  } else {
    drupal_set_message('Импорт плана не выполнен.', 'warning');
  }
}

function query_davalec_import($davalec_array)
{
  if (is_array($davalec_array)) {
    foreach ($davalec_array as $key => $davalec) {
      if (!$key) continue;

      if ($davalec = iconv('Windows-1251', 'UTF-8', trim($davalec))) {
        list($davalec_id, $davalec_name) = explode('|', $davalec);

        if ($davalec_id && $davalec_name) {
          $query = new EntityFieldQuery();
          $users = $query
            ->entityCondition('entity_type', 'user')
            ->fieldCondition('field_oc_davalec_id', 'value', $davalec_id)
            ->execute();

          if (!$users) {
            $user = user_save(NULL, [
              'name' => $davalec_name,
              'pass' => user_password(8),
              'status' => 1,
              'roles' => [DRUPAL_AUTHENTICATED_RID => TRUE],
            ]);
          } else {
            $user = current($users['user']);
          }

          $user_wr = entity_metadata_wrapper('user', $user->uid);
          $user_wr->name = $davalec_name;
          $user_wr->field_oc_davalec_id = $davalec_id;
          $user_wr->save();
        }
      }
    }
    drupal_set_message('Давальцы импортированы.');
  } else {
    drupal_set_message('Импорт давальцев не выполнен.', 'warning');
  }
}

function query_produce_units_import($pu_array)
{
  if (is_array($pu_array)) {
    foreach($pu_array as $key => $punit) {
      if (!$key) continue;

      if ($punit = iconv('Windows-1251', 'UTF-8', trim($punit))) {
        list($punit_id, $punit_name) = explode('|', $punit);

        if ($punit_id && $punit_name) {
          $produce_unit = query_get_produce_unit_by_1cID($punit_id);

          if (!$produce_unit) {
            $produce_unit = (object)array(
              'vid' => taxonomy_vocabulary_machine_name_load('produce_units')->vid,
              'name' => $punit_name,
            );
            taxonomy_term_save($produce_unit);
          }

          // обновить значения полей
          $produce_unit_wr = entity_metadata_wrapper('taxonomy_term', $produce_unit);
          $produce_unit_wr->name = $punit_name;
          $produce_unit_wr->field_oc_produce_unit_id = $punit_id;
          $produce_unit_wr->save();
        }
      }
    }
    drupal_set_message('Установки импортированы.');
  } else {
    drupal_set_message('Импорт установок не выполнен.', 'warning');
  }
}

function query_nomenklatura_import($nom_array)
{
  if (is_array($nom_array)) {
    $vid = taxonomy_vocabulary_machine_name_load('nomenklatura')->vid;

    foreach($nom_array as $key => $nom) {
      if (!$key) continue;

      if ($nom = iconv('Windows-1251', 'UTF-8', trim($nom))) {
        list($artikul, $name, $name_davalec, $artikul_davalec, $davalec_id, $davalec_name) = explode('|', $nom);

        if ($name && $artikul && $davalec_id) {
          // проверить наличие записи Номенклатуры
          $query = new EntityFieldQuery();
          $terms = $query
            ->entityCondition('entity_type', 'taxonomy_term')
            ->propertyCondition('vid', $vid)
            ->fieldCondition('field_oc_artikul', 'value', $artikul)
            ->execute();

          if (!$terms) {
            $term = (object)array(
              'vid' => $vid,
              'name' => $name,
            );
            taxonomy_term_save($term);
          } else {
            $term = current($terms['taxonomy_term']);
          }

          // найти пользователя
          $query = new EntityFieldQuery();
          $users = $query
            ->entityCondition('entity_type', 'user')
            ->fieldCondition('field_oc_davalec_id', 'value', $davalec_id)
            ->execute();
          if ($users) {
            // обновить значения полей
            if ($user = current($users['user'])) {
              $term_wr = entity_metadata_wrapper('taxonomy_term', $term);
              $term_wr->name = $name;
              $term_wr->field_oc_artikul = $artikul;
              $term_wr->field_oc_name_davalec = $name_davalec;
              $term_wr->field_oc_artikul_davalec = $artikul_davalec;
              $term_wr->field_user = $user->uid;
              $term_wr->save();
            }

          } else {
            drupal_set_message('Не найден давалец с ID: ' . $davalec_id, 'error');
            return false;
          }
        }
      }
    }
    drupal_set_message('Номенклатура импортирована.');
  } else {
    drupal_set_message('Импорт номенклатуры не выполнен.', 'warning');
  }
}
